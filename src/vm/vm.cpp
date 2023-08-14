#include "vm.hpp"
#include <algorithm>

namespace nibi {

  vm_c::vm_c(locator_table_c &locator_table)
    : locator_table_(locator_table) {}

  bool vm_c::is_running() {
    return running_.load();
  }

  bool vm_c::start() {
#ifdef DEBUG_BUILD
    std::cout << "vm asked to start" << std::endl;
#endif
    if (is_running()) return true;

    return true;
  }

  bool vm_c::stop() {

#ifdef DEBUG_BUILD
    std::cout << "vm asked to stop" << std::endl;
#endif
    if (!is_running()) return true;


    return false;
  }


  void vm_c::bytecode_ind(std::vector<bytecode::instruction_s> & instructions) {
  
    /*
        New instructions coming may not be aligned in memory
        and we want to keep alignment for vm execution speed.
        Since apps tend to run longer than the time it takes 
        to parse them in, we first realloc the instruction space
        and then copy in new instructions to ensure all 
        instructions are aligned
    */
    auto *new_memory_block = new bytecode::instruction_s[
      num_ins_ + instructions.size()
    ];

    if (instructions_) {
      std::memcpy(new_memory_block, instructions_, num_ins_);
    }

    std::copy(instructions.begin(), instructions.end(), new_memory_block + num_ins_);

    num_ins_ += instructions.size();

    if (instructions_) {
      delete [] instructions_;
    }

    instructions_ = new_memory_block;

    // Now that the instructions are copied in we can exute them
  }
}
