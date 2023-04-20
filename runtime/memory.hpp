#pragma once

/*
  This is a simple mark and weed garbage collector.
  The idea is to have each env context have a reference
  to the global controller, and to use it to create
  any new objects. When the env context is destroyed,
  it will mark all of the objects using the markable_if
  interface, and then the global controller will sweep
  when its ready
*/

#include <forward_list>

namespace memory {

//! \brief An interface for objects that can be marked for deletion
class markable_if {
public:
  virtual ~markable_if() = default;

  //! \brief Mark this object for deletion
  void mark_for_deletion() { marked_ = true; }

  //! \brief Check if this object is marked for deletion
  bool is_marked() const { return marked_; }
private:
  bool marked_{false};
};

//! \brief A controller for allocating and deallocating memory
template<class T>
class controller_c {
public:
  controller_c() = default;
  template<class... Args>
  T* allocate(Args&&... args) {
    static_assert(std::is_base_of<markable_if, T>::value, "Type `T` must be derived from markable_if");
    auto *item = new T(std::forward<Args>(args)...);
    ++allocations;
    return item;
  }

  void mark_all() {
    for (auto *item : collectables_) {
      item->mark_for_deletion();
    }
  }

  void sweep() {
    for (auto *item : markables_) {
      if (!item->is_marked()) {
        delete item;
        --allocations;
        ++deletions;
      }
    }
  }

  std::size_t num_allocations() const { return allocations; }
  std::size_t num_deletions() const { return deletions; }

private:
  std::forward_list<markable*> markables_;
  std::size_t allocations{0};
  std::size_t deletions{0};
};

}