#include "cell_pool.hpp"
#include "cell.hpp"
#include <new>

namespace nibi {

cell_pool_c::cell_pool_c() {
  allocate_batch(NIBI_CELL_POOL_INITIAL_BATCH_SIZE);
  initialized_ = true;
}

cell_pool_c::~cell_pool_c() { shutdown(); }

cell_pool_c &cell_pool_c::instance() {
  static cell_pool_c instance;
  return instance;
}

void cell_pool_c::allocate_batch(std::size_t size) {
  std::size_t cell_size = sizeof(cell_c);
  std::size_t alignment = alignof(cell_c);

  std::size_t total_size = cell_size * size;

  void *memory = ::operator new(total_size, std::align_val_t{alignment});
  if (!memory) {
    throw std::bad_alloc();
  }

  batch_s batch;
  batch.memory = memory;
  batch.capacity = size;
  batches_.push_back(batch);

  for (std::size_t i = 0; i < size; ++i) {
    void *cell_mem = static_cast<char *>(memory) + (i * cell_size);
    free_list_.push(cell_mem);
  }

  next_batch_size_ = size * NIBI_CELL_POOL_GROWTH_FACTOR;
}

void *cell_pool_c::allocate() {
  if (free_list_.empty()) {
    allocate_batch(next_batch_size_);
  }

  void *memory = free_list_.front();
  free_list_.pop();
  return memory;
}

void cell_pool_c::deallocate(cell_c *cell) {
  if (!cell) {
    return;
  }

  cell->~cell_c();
  
  free_list_.push(static_cast<void *>(cell));
}

void cell_pool_c::shutdown() {
  if (shutdown_) {
    return;
  }
  shutdown_ = true;

  if (!initialized_) {
    return;
  }

  for (auto &batch : batches_) {
    if (batch.memory) {
      ::operator delete(batch.memory, std::align_val_t{alignof(cell_c)});
      batch.memory = nullptr;
    }
  }
  batches_.clear();

  while (!free_list_.empty()) {
    free_list_.pop();
  }
}

} // namespace nibi
