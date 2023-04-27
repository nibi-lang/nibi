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

#include <exception>
#include <forward_list>

namespace memory {

//! \brief An interface for objects that can be marked as in use
//! \note Items are marked NOT in use by default, and must be marked as in use
//!       within the number of allocations before sweep assigned to controller_c
class markable_if {
public:
  virtual ~markable_if() {}

  //! \brief Mark this object
  //! \param marked Whether or not this object is marked
  void mark_as_in_use(const bool marked) { marked_ = marked; }

  //! \brief Check if this object is marked as in use
  bool is_marked() const { return marked_; }

  //! \brief If the object is marked as in use, then this method will be
  //!        called so the object can check to see if it is still
  //!        in use.
  //! \note This is called by the controller_c when it sweeps and the
  //!       object indicates that it is marked as in use, to verify
  //!       that it is still in use. This is to prevent memory leaks.
  //!                         "trust but verify"
  virtual void verify_marked_status() = 0;

private:
  bool marked_{false};
};

//! \brief A controller for allocating and deallocating memory
//! \note  `T` must be declared as <MY_TYPE> not <MY_TYPE*>
template <typename T> class controller_c {
public:
  static constexpr std::size_t MINIMUM_ALLOCATIONS_BEFORE_SWEEP = 8;
  static constexpr std::size_t DEFAULT_ALLOCATIONS_BEFORE_SWEEP = 64;

  //! \brief Construct a new controller object
  controller_c(){};

  //! \brief Construct a new controller object with custom allocs before sweep
  //! \param allocs_trigger The number of allocations before a sweep is
  //! triggered
  //! \note If this is less than MINIMUM_ALLOCATIONS_BEFORE_SWEEP, it will be
  //! set to MINIMUM_ALLOCATIONS_BEFORE_SWEEP
  controller_c(const std::size_t allocs_trigger)
      : allocations_before_sweep(allocs_trigger) {
    // Ensure there is an arbitrary minimum enforced
    if (allocations_before_sweep < MINIMUM_ALLOCATIONS_BEFORE_SWEEP) {
      allocations_before_sweep = MINIMUM_ALLOCATIONS_BEFORE_SWEEP;
    }
  };

  //! \brief Destroy the controller object
  //! \note This will destroy all marked and unmarked objects
  //!       that were allocated by this controller
  ~controller_c() {
    for (auto *item : markables_) {
      if (item)
        delete item;
      item = nullptr;
    }
  }

  //! \brief Allocate a new object
  //! \param args The arguments to pass to the constructor
  //! \return A pointer to the newly allocated object
  //! \note The object bust inheret from markable_if
  template <typename... Args> T *allocate(Args &&...args) {

    // Check to see if we need to sweep
    if (++allocations_trigger >= allocations_before_sweep) {

      // Perform sweep
      sweep();

      // Even if sweep doesn't sweep anything,
      // we want to reset the trigger
      allocations_trigger = 0;
    }

    auto *item = new T(std::forward<Args>(args)...);
    markables_.push_front(item);
    return item;
  }

private:
  std::forward_list<markable_if *> markables_;
  std::size_t allocations_trigger{0};
  std::size_t allocations_before_sweep{DEFAULT_ALLOCATIONS_BEFORE_SWEEP};
  void sweep() {
    markables_.remove_if([](markable_if *item) {
      // Check if the item is marked, and if it is, delete it
      // and return true to remove it from the list
      if (!item->is_marked()) {
        delete item;
        item = nullptr;
        return true;
      }

      // Since the item is marked, we need to ask it to check its status
      // and maybe unmark itself
      item->verify_marked_status();
      return false;
    });
  }
};

} // namespace memory