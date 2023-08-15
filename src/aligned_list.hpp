#pragma once

#include "aligned_block.hpp"

namespace nibi {

template<typename T, size_t BLOCK_SIZE>
class aligned_list_c {
public:
  aligned_list_c(
      memory::observer_if &observer): block_(observer) {
    block_.grow(BLOCK_SIZE);
  }


private:
  memory::aligned_block<T> block_;
};

} // namespace nibi
