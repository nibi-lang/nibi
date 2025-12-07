#pragma once

#include "config.hpp"
#include <cstddef>
#include <queue>
#include <vector>

namespace nibi {

class cell_c;

class cell_pool_c {
public:
  static cell_pool_c &instance();

  cell_pool_c(const cell_pool_c &) = delete;
  cell_pool_c &operator=(const cell_pool_c &) = delete;
  cell_pool_c(cell_pool_c &&) = delete;
  cell_pool_c &operator=(cell_pool_c &&) = delete;

  void *allocate();

  void deallocate(cell_c *cell);

  void shutdown();

  ~cell_pool_c();

private:
  cell_pool_c();

  struct batch_s {
    void *memory{nullptr};
    std::size_t capacity{0};
  };

  void allocate_batch(std::size_t size);

  std::vector<batch_s> batches_;
  std::queue<void *> free_list_;
  std::size_t next_batch_size_{NIBI_CELL_POOL_INITIAL_BATCH_SIZE};
  bool shutdown_{false};
  bool initialized_{false};
};

} // namespace nibi
