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

//! \brief An interface for an object that has say over
//!        what objects are in use
class marker_if {
public:
  //! \brief Mark all items that are in use by this object
  virtual void mark_items_in_use() = 0;
};

//! \brief A controller for allocating and deallocating memory
//! \note  `T` must be declared as <MY_TYPE> not <MY_TYPE*>
template<typename T>
class controller_c {
public:
  controller_c(){};

  //! \brief Create a new controller with an interface
  //!        for marking items as in use
  //! \param marker The marker interface that decides 
  //!               what items are in use
  controller_c(marker_if* marker) : marker_{marker} {}

  //! \brief Allocate a new object
  //! \param args The arguments to pass to the constructor
  //! \return A pointer to the newly allocated object
  //! \note The object bust inheret from markable_if
  template<typename... Args>
  T* allocate(Args&&... args) {
    auto* item = new T(std::forward<Args>(args)...);
    ++allocations;
    markables_.push_front(item);
    return item;
  }

  //! \brief Mark all objects as in use
  void mark_all_as_in_use() {
    for (auto *item : markables_) {
      item->mark_as_in_use(true);
    }
  }

  //! \brief Unmark all objects as in use
  void mark_all_as_unused() {
    for (auto *item : markables_) {
      item->mark_as_in_use(false);
    }
  }

  //! \brief Sweep all objects that are not marked as in use
  void sweep() {
    for (auto *item : markables_) {
      if (!item->is_marked()) {
        delete item;
        --allocations;
        ++deletions;
      }
    }
  }

  //! \brief Get the number of allocations
  std::size_t num_allocations() const { return allocations; }

  //! \brief Get the number of deletions
  std::size_t num_deletions() const { return deletions; }

private:
  std::forward_list<markable_if*> markables_;
  std::size_t allocations{0};
  std::size_t deletions{0};
  marker_if* marker_{nullptr};
};

}