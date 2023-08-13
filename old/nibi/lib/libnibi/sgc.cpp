#include "sgc.hpp"

namespace sgc {

bool collector_c::start() {
  if (running_.load()) {
    return true;
  }

  running_.store(true);
  thread_ = std::thread(&collector_c::run, this);
  return true;
}

bool collector_c::shutdown() {
  if (running_.load() == false) {
    return true;
  }

  running_.store(false);
  if (thread_.joinable()) {
    thread_.join();
  }

  for(auto item : alpha_) {
    if (item) {
    //  delete item;
    }
  }
  alpha_.clear();

  for(auto item : beta_) {
    if (item) {
     // delete item;
    }
  }
  beta_.clear();

  return true;
}

void collector_c::run() {
  using namespace std::chrono_literals;
  while(running_.load()) {
    std::this_thread::sleep_for(500ms);
    query_collect();
  }
}

void collector_c::trigger_collection() {
  manual_override_ = true;
  query_collect();
  manual_override_ = false;
}

void collector_c::register_object(managed_c *object) {
  if (!object) return;
  const std::lock_guard<std::mutex> lock(swap_mutex_);
  active_list_->push_back(object);
}

void collector_c::swap_active() {
  const std::lock_guard<std::mutex> lock(swap_mutex_);
  if (reinterpret_cast<size_t>(active_list_) == 
     reinterpret_cast<size_t>(&this->alpha_)) {
    active_list_ = &this->beta_;
    inactive_list_ = &this->alpha_;
  } else {
    active_list_ = &this->alpha_;
    inactive_list_ = &this->beta_;
  }
}

void collector_c::collect() {
  inactive_list_->remove_if([](managed_c* obj){
    if (!obj) { return true; }

    if (obj->refCount() == 0) {
      delete obj;
      return true;
    }

    return false;
  });
}

void collector_c::query_collect() {
    if (manual_override_ ||
       (seconds_since_last_collect() >= COLLECTION_TIME_INTERVAL_SEC)) {

    // Momentarily lock access for producer and swap queue pointers
    swap_active();

    // Perform collection
    collect();

    // Update last collection
    last_collection_ = std::chrono::high_resolution_clock::now();
  }
}

double collector_c::seconds_since_last_collect() const {
  return std::chrono::duration_cast<
          std::chrono::duration<double, std::ratio<1>>>
     ( std::chrono::high_resolution_clock::now() - last_collection_).count();
}

}
