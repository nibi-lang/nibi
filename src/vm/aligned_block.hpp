#pragma once

#include <vector>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace memory {

  //! \brief An observer that is informed of allocation
  //!        failures
  class observer_if {
  public:
    virtual void alloc_failure_ind(
        const char* what,
        size_t total_bytes_allocated,
        size_t attempted_allocation) = 0;
  };

//! \brief An object that manages the allocation
//!        of a block of memory such-that the data
//!        stays aligned in memory.
template<typename T>
class aligned_block_c {
public:

  aligned_block_c() = delete;
  aligned_block_c(observer_if &observer)
    : observer_(observer){}
  ~aligned_block_c() { clear(); }

  //! \brief Retrieve the address of the start of 
  //!        allocated memory
  T* data() const { return data_; }

  //! \brief Retrieve the number of T objects that
  //!        the block can currently contain
  size_t count() const { return num_t_allocated_; }

  //! \brief Retrieve the number of bytes allocated
  size_t size() const { return sizeof(T) * num_t_allocated_; }

  //! \brief Check of there has been an error 
  bool has_error() const { return error_present_; }

  //! \brief Free all memory and errors
  void clear() {
    num_t_allocated_ = 0;
    error_present_ = false;
    if (!data_) {
      return;
    }
    delete [] data_;
    data_ = nullptr;
    return;
  }

  //! \brief Attempt to grow the block of memory
  //!        by a given number of items
  //! \param num_items The number of items to add
  //!        to the current size of the block
  //! \returns true iff the block could be grown
  //! \note The address of the block may have changed
  //!       meaning previous pointers retrieved from data()
  //!       may now be invalid
  bool grow(const size_t num_items) {
    T* block{nullptr};

    try {
      block = new T [num_t_allocated_ + num_items];
    }
    catch (const std::bad_alloc& e) {
      error_present_ = true;
      observer_.alloc_failure_ind(
          e.what(),
          size(),
          num_items * sizeof(T));
      return false;
    }

    if (data_) {
      // Move data to new data location
      std::memcpy(block, data_, num_t_allocated_);

      // Remove old copy of data
      delete [] data_;
    }

    num_t_allocated_ += num_items;


      // Point at new blockk
      data_ = block;

    return true;
  }

private:
  observer_if &observer_;
  size_t num_t_allocated_{0};
  bool error_present_{false};

  T* data_{nullptr};
};

} // namespace memory
