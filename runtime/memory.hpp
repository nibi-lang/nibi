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

#include "profile/config.hpp"
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

private:
  bool marked_{true};
};

//! \brief A controller for allocating and deallocating memory
//! \note  `T` must be declared as <MY_TYPE> not <MY_TYPE*>
template <typename T> class controller_c {
public:
  static constexpr std::size_t DEFAULT_ALLOCATIONS_BEFORE_SWEEP = 250;

  //! \brief Construct a new controller object
  controller_c(){};

  //! \brief Destroy the controller object
  //! \note This will destroy all marked and unmarked objects
  //!       that were allocated by this controller
  ~controller_c() {
    for (auto *item : markables_) {
      if (item) {
        delete item;
        item = nullptr;
      }
    }
  }

#if PROFILE_ALLOCATOR
  void flush() {
    for (auto *item : markables_) {
      if (item) {
        delete item;
        item = nullptr;
        ++num_items_flushed;
      }
    }
    markables_.clear();
  }
#endif

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
#if PROFILE_ALLOCATOR
    ++num_std_allocations;
#endif
    auto *item = new T(std::forward<Args>(args)...);
    markables_.push_front(item);
    return item;
  }

  //! \brief Allocate a new object without triggering a sweep
  //! \param args The arguments to pass to the constructor
  //! \return A pointer to the newly allocated object
  //! \note This method should only be used for objects that are
  //!       allocated in bulk and will be handed back to the allocator
  //!       via `take_ownership`
  template <typename... Args> T *allocate_no_sweep(Args &&...args) {
#if PROFILE_ALLOCATOR
    ++num_no_sweep_allocations;
#endif
    // This is trivial but we want to ensure we can change how cells are
    // allocated and deallocated without changing the interface
    return new T(std::forward<Args>(args)...);
  }

  //! \brief Take ownership of an object
  //! \param item The item to take ownership of
  //! \note Once this is called on an item, the controller will
  //!       assume ownership of the item and will delete it when
  //!       it is no longer in use
  void take_ownership(markable_if *item) {
#if PROFILE_ALLOCATOR
    ++num_ownderships_taken;
#endif
    markables_.push_front(item);
  }

#if PROFILE_ALLOCATOR
  std::size_t num_std_allocations{0};
  std::size_t num_no_sweep_allocations{0};
  std::size_t num_frees{0};
  std::size_t num_sweeps{0};
  std::size_t num_ownderships_taken{0};
  std::size_t num_items_flushed{0};
#endif

private:
  std::forward_list<markable_if *> markables_;
  std::size_t allocations_trigger{0};
  std::size_t allocations_before_sweep{DEFAULT_ALLOCATIONS_BEFORE_SWEEP};

  void sweep() {
#if PROFILE_ALLOCATOR
    ++num_sweeps;
    markables_.remove_if([this](markable_if *item) {
#else
    // If we aren't profiling we don't want to capture `this`
    markables_.remove_if([](markable_if *item) {
#endif
      // Check if the item is marked, and if it is, delete it
      // and return true to remove it from the list
      if (!item->is_marked()) {
#if PROFILE_ALLOCATOR
        ++num_frees;
#endif
        delete item;
        item = nullptr;
        return true;
      }
      return false;
    });
  }
};

} // namespace memory