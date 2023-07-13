#pragma once

#include <cstdint>

namespace nibi {

//! \brief Something that will be reference counted
class ref_counted_c {
public:
  ref_counted_c() {}
  virtual ~ref_counted_c() {}

  const ref_counted_c *acquire() const {
    ref_count_++;
    return this;
  }
  int release() const { return --ref_count_; }
  int refCount() const { return ref_count_; }

private:
  ref_counted_c(const ref_counted_c &);
  ref_counted_c &operator=(const ref_counted_c &);

  mutable std::size_t ref_count_{0};
};

//! \brief A wrapper that performs operations
//!        for and around a reference counted object
template <class T> class ref_counted_ptr_c {
public:
  ref_counted_ptr_c() {}

  ref_counted_ptr_c(T *object) { acquire(object); }

  ref_counted_ptr_c(const ref_counted_ptr_c &rhs) : object_(0) {
    acquire(rhs.object_);
  }

  const ref_counted_ptr_c &operator=(const ref_counted_ptr_c &rhs) {
    acquire(rhs.object_);
    return *this;
  }

  T &operator*() const { return *object_; }

  bool operator==(const ref_counted_ptr_c &rhs) const {
    return object_ == rhs.object_;
  }

  bool operator!=(const ref_counted_ptr_c &rhs) const {
    return object_ != rhs.object_;
  }

  bool operator==(std::nullptr_t) const { return object_ == nullptr; }

  bool operator!=(std::nullptr_t) const { return object_ != nullptr; }

  operator bool() const { return object_ != nullptr; }

  ~ref_counted_ptr_c() { release(); }

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
    if ((object_ != nullptr) && (object_->release() == 0)) {
      delete object_;
    }
  }

  T *object_{nullptr};
};

} // namespace nibi
