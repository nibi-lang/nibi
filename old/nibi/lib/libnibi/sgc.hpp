#pragma once

#include <chrono>
#include <cstdint>
#include <list>
#include <mutex>

#include <atomic>
#include <thread>

namespace sgc {

class managed_c;
class collector_c {
  public:
    //! \brief Time between collection cycles
    static constexpr uint16_t COLLECTION_TIME_INTERVAL_SEC = 4;

    //! \brief Create a collector
    collector_c() {
      last_collection_ = std::chrono::high_resolution_clock::now();
      active_list_ = &alpha_;
      inactive_list_ = &beta_;
    }

    //! \brief Manually trigger a collection cycle
    //! \note Don't call this too often as it will cause 
    //!       a momentary pause in the ability to register
    //!       new objects
    void trigger_collection();

    //! \brief Register an object with the collector
    //! \note The object given to the collector will have its
    //!       deletion managed so manually calling delete
    //!       will not be required
    void register_object(managed_c* object);

    //! \brief Start the collector
    //! \returns true iff the collector is running
    //! \post The collector will start collecting
    //!       objects at regular intervals of COLLECTION_TIME_INTERVAL_SEC
    bool start();

    //! \brief Shutdown the collection process
    //! \retuns true iff the collector can be shutdown
    //! \post All objects managed by collector will be cleaned
    //! \note Once shutdown is called attempting to register
    //!       objects until start() is called again may result
    //!       in undefined behavior. 
    bool shutdown();

  private:
    // Mutex that manages active/inactive pointer
    // swapping
    std::mutex swap_mutex_;

    // Two lists that rotate. 
    // Registered items will go into whatever the "active"
    // list is. When a collection cycle happens, a
    // momentary block occurs to swap pointers between
    // alpha and beta and the collector thread will
    // check the "inactive" list for items not 
    // being referenced anymore
    std::list<managed_c*> alpha_;
    std::list<managed_c*> beta_;

    // Active list - the list that is currently having
    // items placed in it from register_object
    std::list<managed_c*> *active_list_{nullptr};

    // Inactive list is the list that is not currently
    // being added to by the `register_object` method,
    // not inactive in the sense that the items are alive
    std::list<managed_c*> *inactive_list_{nullptr};

    // The time point of the last collection
    std::chrono::time_point<
      std::chrono::high_resolution_clock> last_collection_;

    bool manual_override_{false};

    std::atomic<bool> running_{false};
    std::thread thread_;

    void run();
    void swap_active();
    void collect();
    void query_collect();
    double seconds_since_last_collect() const;
};

//! \brief An object that will have its end of life
//!        handled by a collector
class managed_c {
public:
  managed_c() {}
  virtual ~managed_c() {}

  const managed_c *acquire() const {
    ref_count_++;
    return this;
  }

  void release() const {
    if (0 == ref_count_) return;
    --ref_count_;
  }

  int64_t refCount() const { return ref_count_; }

private:
  managed_c(const managed_c &);
  managed_c &operator=(const managed_c &);

  mutable std::uint32_t ref_count_{0};
};

//! \brief Pointer type of managed item
template<class T>
class managed_ptr_c {
public:
  managed_ptr_c() {}

  managed_ptr_c(T *object) { acquire(object); }

  managed_ptr_c(const managed_ptr_c &rhs) : object_(0) {
    acquire(rhs.object_);
  }

  const managed_ptr_c &operator=(const managed_ptr_c &rhs) {
    acquire(rhs.object_);
    return *this;
  }

  T &operator*() const { return *object_; }

  T *get() const { return object_; }

  bool operator==(const managed_ptr_c &rhs) const {
    return object_ == rhs.object_;
  }

  bool operator!=(const managed_ptr_c &rhs) const {
    return object_ != rhs.object_;
  }

  operator bool() const { return object_ != nullptr; }

  ~managed_ptr_c() { release(); }

  T *operator->() const { return object_; }
  T *ptr() const { return object_; }

private:
  void acquire(T *object) {
    if (object != nullptr) {
      object->acquire();
    }
    release();
    object_ = object;
  }

  void release() {
    if (object_) {
      object_->release();
    }
  }

  T *object_{nullptr};
};

} // namespace sgc
