#include "vm.hpp"
#include "nibi.hpp"
#include <algorithm>

namespace nibi {

  vm_c::vm_c(locator_table_c &locator_table)
    : ins_mem_observer_(*this),
      ins_data_(locator_table, ins_mem_observer_) {}

  bool vm_c::is_running() {
    return running_.load();
  }

  bool vm_c::start() {
#ifdef DEBUG_BUILD
    std::cout << "vm asked to start" << std::endl;
#endif
    if (is_running()) return true;

    // Expand the instruction by one block to start
    if (!ins_data_.expand_instruction_memory(1)) {
      return false;
    }

    running_.store(true);

    // Start threads or whatever here

    return true;
  }

  bool vm_c::stop() {

#ifdef DEBUG_BUILD
    std::cout << "vm asked to stop" << std::endl;
#endif
    if (!is_running()) return true;

    ins_data_.ins_block.clear();
    ins_data_.instructions = nullptr;

    return false;
  }

  void vm_c::instruction_memory_observer_c::alloc_failure_ind(
      const char* msg,
      size_t total_allocated,
      size_t attempted_alloc) {

    std::string error = "VM instruction memory OOM\nError: ";
    error += msg;
    error += "\n";
    error += std::to_string(total_allocated);
    error += " : bytes allocated before OOM\n";
    error += std::to_string(attempted_alloc);
    error += " : bytes attempted to allocate\n";

    fatal_error(error_origin_e::VM, nullptr, error);
  }

  void vm_c::bytecode_ind(std::vector<bytecode::instruction_s> & instructions) {
    ins_data_.insert_instructions(instructions);
    run();  
  }

  void vm_c::run() {
    while (running_.load() && instruction_pointer_ < ins_data_.instruction_count) {

      //std::cout << "Instruction pointer: " << instruction_pointer_ << std::endl;

      bytecode::instruction_s ins = ins_data_.instructions[instruction_pointer_];

      switch(ins.op) {
        case bytecode::op_e::NOP: { break; }
        case bytecode::op_e::BUILTIN_SYMBOL: { break; }
        case bytecode::op_e::USER_SYMBOL: { break; }
        case bytecode::op_e::TAG: { break; }
        case bytecode::op_e::INTEGER: { break; }
        case bytecode::op_e::REAL: { break; }
        case bytecode::op_e::CHAR: { break; }
        case bytecode::op_e::STRING: { break; }
        case bytecode::op_e::NIL: { break; }
        case bytecode::op_e::LIST_INS_IND: { break; }
        case bytecode::op_e::LIST_DATA_IND: { break; }
        case bytecode::op_e::LIST_ACCESS_IND: { break; }
        case bytecode::op_e::LIST_END: { break; }
        case bytecode::op_e::LOCATOR: { break; }

      } // end switch

      ++instruction_pointer_;
    } // end while
  } // end run
} // namespace nibi
