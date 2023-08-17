#include "lib.hpp"
#include <iostream>
#include <libnibi/api.hpp>
#include <libnibi/macros.hpp>
#include <sstream>
#include <memory>
#include <thread>
#include <mutex> 
#include <future>
#include <chrono>
#include <mutex>
#include "cpp-net-lib/netlib.hpp"

namespace {

constexpr uint8_t STATUS_READY = 1;
constexpr uint8_t STATUS_TIMEOUT = 2;
constexpr uint8_t STATUS_UNKNOWN = 3;
constexpr uint8_t THREAD_KILL_WAIT_TIME_SEC = 10;

struct task_s {
  nibi::env_c env;
  nibi::source_manager_c sm;
  nibi::interpreter_c interpreter;
  nibi::cell_ptr job{nullptr};

  task_s() : interpreter(env, sm) {}
};

struct controller_s {
  size_t aberrant_tag{0};
  std::mutex launch_mutex;
  netlib::thread_pool thread_pool;
  std::mutex mut;
};

std::shared_ptr<controller_s> controller_{nullptr};

std::shared_ptr<controller_s> get_controller() {
  if (!controller_) {
    controller_ = std::make_shared<controller_s>();
    controller_->aberrant_tag = nibi::api::get_aberrant_id();
  }
  return controller_;
}

// --- cell ---

class thread_cell_c final : public nibi::aberrant_cell_if {
public:
  thread_cell_c()
    : aberrant_cell_if(get_controller()->aberrant_tag) {
        start_time_ = std::chrono::high_resolution_clock::now();
      }

  // We don't wan't to actually clone this, as we can't
  // quite do that with a future. Instead, we pass the "this"
  // pointer so all cells will point to the same future.
  // The means it can squeeze through thread boundary
  // so we mutex all access to the data
  aberrant_cell_if *clone() { return this; }

  std::string represent_as_string() {
    std::string result = "<THREADS::THREAD_CELL:";
    if (complete) {
      result += "COMPLETED:";
    }

    result += std::to_string(runtime());
    result += "ms>";
    return result;
  }

  nibi::cell_ptr get() {
    if (!complete) {
      return nibi::allocate_cell(nibi::cell_type_e::NIL);
    }
    return data_.get();
  }

  std::future_status wait_for(size_t ms) {
      if (complete) { return std::future_status::ready; }
      std::lock_guard<std::mutex> lock(mut_);
      return data_.wait_for(std::chrono::microseconds(ms));
  }

  bool is_ready() {
    if (complete) { return true; }
    return wait_for(1) == std::future_status::ready;
  }

  size_t runtime() {
    if (complete) {
      return std::chrono::duration_cast<std::chrono::milliseconds>(
            end_time_ - start_time_).count();
    }

    return std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::high_resolution_clock::now() - start_time_).count();
  }

  bool kill() {
    if (complete) { return true; }
    bool killed{false};
    {
      std::lock_guard<std::mutex> lock(mut_);
      killed = task_.interpreter.terminate(THREAD_KILL_WAIT_TIME_SEC);
    }
    if (killed) {
      complete_ind();
    }
    return killed;
  }

  void complete_ind() {
    std::lock_guard<std::mutex> lock(mut_);
    end_time_ =  std::chrono::high_resolution_clock::now();
    complete = true;
  }

  task_s task_;
  std::mutex mut_;
  std::future<nibi::cell_ptr> data_;

private:
  bool complete{false};
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
  std::chrono::time_point<std::chrono::high_resolution_clock> end_time_;
};

// ------------

nibi::cell_ptr run_task(nibi::cell_ptr tcell) {

  auto tcell_actual = reinterpret_cast<thread_cell_c*>(
      tcell->data.aberrant);

  tcell_actual->task_.interpreter.instruction_ind(
      tcell_actual->task_.job);

  tcell_actual->complete_ind();

  return tcell_actual->task_.interpreter.get_last_result();
}

nibi::cell_ptr launch_task(nibi::cell_ptr cell) {

  auto controller = get_controller();
  std::lock_guard<std::mutex> lock(controller->mut);

  auto tcell = nibi::allocate_cell(nibi::cell_type_e::ABERRANT);
  auto tcell_actual = new thread_cell_c();
  tcell_actual->task_.job = cell;
  
  tcell->data.aberrant = tcell_actual;

  tcell_actual->data_ = controller->thread_pool.add_task(run_task, tcell);

  return tcell;
}

} // namespace

nibi::cell_ptr nibi_threads_future(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                          nibi::env_c &env) {

  NIBI_LIST_ENFORCE_SIZE("{threads nibi_threads_future}", ==, 2)

  auto &list_info = list[1]->as_list_info();
  if (list_info.type != nibi::list_types_e::INSTRUCTION) {
    throw nibi::interpreter_c::exception_c(
        "Parameter to future expected to be an instruction list", list[1]->locator);
  }

  // Launch the task with a cloned cell so the thread can't access 
  // members of other threads
  return launch_task(list[1]->clone(env));
}

thread_cell_c* as_thread_cell(nibi::cell_ptr &cell) {
  if (cell->type != nibi::cell_type_e::ABERRANT) {
    throw nibi::interpreter_c::exception_c(
        "Expected aberrant cell holding thread information",
        cell->locator);
  }
  if (!(cell->data.aberrant)) {
    throw nibi::interpreter_c::exception_c(
        "Expected aberrant cell holding thread information",
        cell->locator);
  }
  auto controller = get_controller();
  if ((!cell->data.aberrant->is_tagged()) ||
      cell->data.aberrant->get_tag() != get_controller()->aberrant_tag) {
    throw nibi::interpreter_c::exception_c(
        "Aberrant cell is not a thread_cell",
        cell->locator);
  }

  return reinterpret_cast<thread_cell_c*>(
      cell->data.aberrant);
}

nibi::cell_ptr nibi_threads_future_get(nibi::interpreter_c &ci,
                                   nibi::cell_list_t &list, nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{threads nibi_threads_future_get}", ==, 2)
  auto processed = ci.process_cell(list[1], env);
  auto tcell = as_thread_cell(processed);
  return tcell->get();
}

nibi::cell_ptr nibi_threads_future_wait_for(nibi::interpreter_c &ci,
                                   nibi::cell_list_t &list, nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{threads nibi_threads_future_wait_for}", ==, 3)
  auto processed = ci.process_cell(list[1], env);
  auto tcell = as_thread_cell(processed);
  auto time = ci.process_cell(list[2], env)->to_integer();
  auto result = tcell->wait_for(time);

  if (result == std::future_status::ready) {
    return nibi::allocate_cell((int64_t)STATUS_READY);
  }

  if (result == std::future_status::timeout) {
    return nibi::allocate_cell((int64_t)STATUS_TIMEOUT);
  }

  return nibi::allocate_cell((int64_t)STATUS_UNKNOWN);
}

nibi::cell_ptr nibi_threads_future_is_ready(nibi::interpreter_c &ci,
                                   nibi::cell_list_t &list, nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{threads nibi_threads_future_is_ready}", ==, 2)
  auto processed = ci.process_cell(list[1], env);
  auto tcell = as_thread_cell(processed);
  return nibi::allocate_cell((int64_t)(tcell->is_ready()));
}

nibi::cell_ptr nibi_threads_future_runtime(nibi::interpreter_c &ci,
                                   nibi::cell_list_t &list, nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{threads nibi_threads_future_runtime}", ==, 2)
  auto processed = ci.process_cell(list[1], env);
  auto tcell = as_thread_cell(processed);
  return nibi::allocate_cell((int64_t)(tcell->runtime()));
}

nibi::cell_ptr nibi_threads_future_kill(nibi::interpreter_c &ci,
                                   nibi::cell_list_t &list, nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{threads nibi_threads_future_kill}", ==, 2)
  auto processed = ci.process_cell(list[1], env);
  auto tcell = as_thread_cell(processed);
  return nibi::allocate_cell((int64_t)(tcell->kill()));
}

