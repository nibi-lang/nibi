#include "cpp-net-lib/netlib.hpp"
#include "lib.hpp"
#include <chrono>
#include <future>
#include <libnibi/macros.hpp>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace {

constexpr uint8_t STATUS_READY = 1;
constexpr uint8_t STATUS_TIMEOUT = 2;
constexpr uint8_t STATUS_UNKNOWN = 3;

struct task_s {
  nibi::env_c env;
  nibi::source_manager_c sm;
  nibi::interpreter_c interpreter;
  nibi::cell_ptr job{nullptr};

  task_s() : interpreter(env, sm) {}
};

class thread_data_c {
public:
  thread_data_c() { start_time_ = std::chrono::high_resolution_clock::now(); }

  nibi::cell_ptr get() {
    if (!complete) {
      return nibi::allocate_cell(nibi::cell_type_e::NIL);
    }

    // Calling get more than once on a a future is UB
    // so we just make sure users can't do that
    if (!result_) {
      result_ = data_.get();
    }

    return result_;
  }

  std::future_status wait_for(size_t ms) {
    if (complete) {
      return std::future_status::ready;
    }
    std::lock_guard<std::mutex> lock(mut_);
    return data_.wait_for(std::chrono::microseconds(ms));
  }

  bool is_ready() {
    if (complete) {
      return true;
    }
    return wait_for(1) == std::future_status::ready;
  }

  size_t runtime() {
    if (complete) {
      return std::chrono::duration_cast<std::chrono::milliseconds>(end_time_ -
                                                                   start_time_)
          .count();
    }

    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::high_resolution_clock::now() - start_time_)
        .count();
  }

  void kill() {
    task_.interpreter.terminate();
    if (complete) {
      return;
    }
    complete_ind();
  }

  void complete_ind() {
    std::lock_guard<std::mutex> lock(mut_);
    end_time_ = std::chrono::high_resolution_clock::now();
    complete = true;
  }

  bool is_complete() const { return complete; }

  task_s task_;
  std::mutex mut_;
  std::future<nibi::cell_ptr> data_;

private:
  bool complete{false};
  nibi::cell_ptr result_{nullptr};
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
  std::chrono::time_point<std::chrono::high_resolution_clock> end_time_;
};

using thread_data_ptr = std::shared_ptr<thread_data_c>;

// -------------

struct controller_s {
  uint64_t id_base{0};
  std::mutex launch_mutex;
  netlib::thread_pool thread_pool;
  std::mutex mut;
  std::unordered_map<uint64_t, thread_data_ptr> thread_map;
};

std::shared_ptr<controller_s> controller_{nullptr};

std::shared_ptr<controller_s> get_controller() {
  if (!controller_) {
    controller_ = std::make_shared<controller_s>();
  }
  return controller_;
}

// ------------

nibi::cell_ptr run_task(thread_data_ptr tcell_actual) {
  tcell_actual->task_.interpreter.instruction_ind(tcell_actual->task_.job);
  tcell_actual->complete_ind();
  return tcell_actual->task_.interpreter.get_last_result();
}

uint64_t launch_task(nibi::cell_ptr cell) {

  auto list = cell->as_list_info().list;
  if (list.empty()) {
    throw nibi::interpreter_c::exception_c(
        "Can not thread an empty instruction set", cell->locator);
  }

  if (!list[0]->as_function_info().isolate) {
    throw nibi::interpreter_c::exception_c(
        "Only `isolated` functions may be threaded, use threads::fn",
        cell->locator);
  }

  auto controller = get_controller();
  std::lock_guard<std::mutex> lock(controller->mut);

  auto tcell_actual = std::make_shared<thread_data_c>();
  tcell_actual->task_.job = cell;
  tcell_actual->data_ =
      controller->thread_pool.add_task(run_task, tcell_actual);

  uint64_t id = controller->id_base++;
  controller->thread_map[id] = tcell_actual;
  return id;
}

} // namespace

nibi::cell_ptr nibi_threads_active(nibi::interpreter_c &ci,
                                   nibi::cell_list_t &list, nibi::env_c &env) {

  NIBI_LIST_ENFORCE_SIZE("{threads nibi_threads_active}", ==, 1)

  uint64_t active = 0;
  auto controller = get_controller();
  {
    std::lock_guard<std::mutex> lock(controller->mut);
    for (auto &[id, cell] : controller->thread_map) {
      if (cell && (!cell->is_ready()))
        active++;
    }
  }
  return nibi::allocate_cell((uint64_t)active);
}

nibi::cell_ptr nibi_threads_shutdown(nibi::interpreter_c &ci,
                                     nibi::cell_list_t &list,
                                     nibi::env_c &env) {

  NIBI_LIST_ENFORCE_SIZE("{threads nibi_threads_shutdown}", ==, 1)

  auto controller = get_controller();
  {
    std::lock_guard<std::mutex> lock(controller->mut);
    for (auto &[id, cell] : controller->thread_map) {
      cell->kill();
    }
    std::this_thread::sleep_for(std::chrono::microseconds(500));
    for (auto &[id, cell] : controller->thread_map) {
      cell->get();
    }
  }
  controller_ = nullptr;
  return nibi::allocate_cell((uint64_t)1);
}

nibi::cell_ptr nibi_threads_future(nibi::interpreter_c &ci,
                                   nibi::cell_list_t &list, nibi::env_c &env) {

  NIBI_LIST_ENFORCE_SIZE("{threads nibi_threads_future}", ==, 2)

  auto &list_info = list[1]->as_list_info();
  if (list_info.type != nibi::list_types_e::INSTRUCTION) {
    throw nibi::interpreter_c::exception_c(
        "Parameter to future expected to be an instruction list",
        list[1]->locator);
  }

  // Launch the task with a cloned cell so the thread can't access
  // members of other threads

  return nibi::allocate_cell((int64_t)launch_task(list[1]->clone(env)));
}

thread_data_ptr as_thread_data(nibi::cell_ptr &cell) {
  if (!cell->is_integer()) {
    throw nibi::interpreter_c::exception_c(
        "Expected integer id for loading thread information", cell->locator);
  }

  thread_data_ptr data{nullptr};
  {
    auto controller = get_controller();
    std::lock_guard<std::mutex> lock(controller->mut);
    auto it = controller->thread_map.find(cell->data.u64);
    if (it != controller->thread_map.end()) {
      data = (*it).second;
    }
  }
  return data;
}

nibi::cell_ptr nibi_threads_future_get(nibi::interpreter_c &ci,
                                       nibi::cell_list_t &list,
                                       nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{threads nibi_threads_future_get}", ==, 2)
  auto processed = ci.process_cell(list[1], env);
  auto tcell = as_thread_data(processed);
  if (!tcell) {
    return nibi::allocate_cell(nibi::cell_type_e::NIL);
  }
  auto value = tcell->get();

  if (tcell->is_complete()) {
    auto controller = get_controller();
    std::lock_guard<std::mutex> lock(controller->mut);
    auto it = controller->thread_map.find(processed->data.u64);
    if (it != controller->thread_map.end()) {
      controller->thread_map.erase(it);
    }
  }
  return value;
}

nibi::cell_ptr nibi_threads_future_wait_for(nibi::interpreter_c &ci,
                                            nibi::cell_list_t &list,
                                            nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{threads nibi_threads_future_wait_for}", ==, 3)
  auto processed = ci.process_cell(list[1], env);
  auto tcell = as_thread_data(processed);
  if (!tcell) {
    return nibi::allocate_cell(nibi::cell_type_e::NIL);
  }
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
                                            nibi::cell_list_t &list,
                                            nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{threads nibi_threads_future_is_ready}", ==, 2)
  auto processed = ci.process_cell(list[1], env);
  auto tcell = as_thread_data(processed);
  if (!tcell) {
    return nibi::allocate_cell(nibi::cell_type_e::NIL);
  }
  return nibi::allocate_cell((int64_t)(tcell->is_ready()));
}

nibi::cell_ptr nibi_threads_future_runtime(nibi::interpreter_c &ci,
                                           nibi::cell_list_t &list,
                                           nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{threads nibi_threads_future_runtime}", ==, 2)
  auto processed = ci.process_cell(list[1], env);
  auto tcell = as_thread_data(processed);
  if (!tcell) {
    return nibi::allocate_cell(nibi::cell_type_e::NIL);
  }
  return nibi::allocate_cell((int64_t)(tcell->runtime()));
}

nibi::cell_ptr nibi_threads_future_kill(nibi::interpreter_c &ci,
                                        nibi::cell_list_t &list,
                                        nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{threads nibi_threads_future_kill}", ==, 2)
  auto processed = ci.process_cell(list[1], env);
  auto tcell = as_thread_data(processed);
  if (!tcell) {
    return nibi::allocate_cell(nibi::cell_type_e::NIL);
  }
  tcell->kill();
  return nibi::allocate_cell((int64_t)1);
}
