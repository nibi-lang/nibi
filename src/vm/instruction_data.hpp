#pragma once

#include "source.hpp"
#include "aligned_block.hpp"
#include "bytecode/bytecode.hpp"
#include <iostream>
namespace nibi {

  struct instruction_data_s {
    static constexpr uint8_t INSTRUCTION_BLOCK_COUNT = 128;

    instruction_data_s(locator_table_c& locator_table, memory::observer_if &observer) : 
      table(locator_table),
      ins_block(observer){}

    locator_table_c &table;
    memory::aligned_block_c<bytecode::instruction_s> ins_block;
    bytecode::instruction_s *instructions{nullptr};
    size_t instruction_count{0};

    bool expand_instruction_memory(const size_t blocks) {
      if (!ins_block.grow(INSTRUCTION_BLOCK_COUNT * blocks)) {
        return false;
      }
      instructions = ins_block.data();
      return true;
    }

    locator_ptr get_locator(const int64_t id) {
      return table.get_locator(id);
    }

    size_t capacity() const {
      return ins_block.count();
    }

    bool insert_instructions(std::vector<bytecode::instruction_s> & ins) {

      // Check if we need to expand instruction memory to accomodate new instructions
      {
        auto total_required_cap = instruction_count + ins.size();
        if (capacity() <= total_required_cap) {
          auto min_required_space = total_required_cap - capacity();
          if (min_required_space >= INSTRUCTION_BLOCK_COUNT) {
            auto required_blocks = std::ceil(min_required_space / INSTRUCTION_BLOCK_COUNT);
            if (!expand_instruction_memory(required_blocks)) {
              return false;
            }
          } else {
            expand_instruction_memory(1);
          }
        }
      }

      // Add the instructions
      std::copy(ins.begin(), ins.end(), instructions + instruction_count);

      // Increase the instruction count
      instruction_count += ins.size();
      return true;
    }
  };
} // namespace nibi
