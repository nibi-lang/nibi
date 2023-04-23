#pragma once

/*
  This is a simple mark and sweep garbage collector.
  The idea is to have each env context have a reference
  to the global controller, and to use it to create
  any new objects. When the env context is destroyed,
  it will mark all of the objects using the markable_if
  interface, and then the global controller will sweep
  when its ready
*/

#include <forward_list>

namespace memory {

//! \brief An interface for objects that can be marked as in use
//! \note Items are marked as in use by default
class markable_if {
public:
  virtual ~markable_if() = default;

  //! \brief Mark this object
  //! \param marked Whether or not this object is marked
  void mark_as_in_use(const bool marked) { marked_ = marked; }

  //! \brief Check if this object is marked as in use
  bool is_marked() const { return marked_; }
private:
  bool marked_{true};
};

//! \brief A controller for allocating and deallocating memory
//! \note  `T` must be declared as <MY_TYPE> not <MY_TYPE*>
template<typename T>
class controller_c {
public:
  static constexpr std::size_t DEFAULT_ALLOCATIONS_BEFORE_SWEEP = 256;

  //! \brief Construct a new controller object
  controller_c(){};

  //! \brief Construct a new controller object with custom allocs before sweep
  //! \param allocs_trigger The number of allocations before a sweep is triggered
  controller_c(const std::size_t allocs_trigger)
    : allocations_before_sweep(allocs_trigger) {};

  //! \brief Destroy the controller object
  //! \note This will destroy all marked and unmarked objects
  //!       that were allocated by this controller
  ~controller_c() {
    for (auto *item : markables_) {
      delete item;
    }
  }

  //! \brief Allocate a new object
  //! \param args The arguments to pass to the constructor
  //! \return A pointer to the newly allocated object
  //! \note The object bust inheret from markable_if
  template<typename... Args>
  T* allocate(Args&&... args) {

    // Check to see if we need to sweep
    if (++allocations_trigger >= allocations_before_sweep) {

      // Perform sweep
      sweep();

      // Even if sweep doesn't sweep anything,
      // we want to reset the trigger
      allocations_trigger = 0;
    }

    auto* item = new T(std::forward<Args>(args)...);
    markables_.push_front(item);
    return item;
  }

private:
  std::forward_list<markable_if*> markables_;
  std::size_t allocations_trigger{0};
  std::size_t allocations_before_sweep{DEFAULT_ALLOCATIONS_BEFORE_SWEEP};
  void sweep() {
    for (auto *item : markables_) {
      if (!item->is_marked()) {
        delete item;
      }
    }
  }
};

}