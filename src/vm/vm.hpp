#pragma once

#include <atomic>
#include <vector>

#include "source.hpp"
#include "aligned_block.hpp"
#include "instruction_data.hpp"
#include "bytecode/bytecode.hpp"
#include "interfaces/process_if.hpp"

namespace nibi {

class vm_c final : public process_if {
public:
  vm_c(locator_table_c &locator_table);
 
  // From process_if
  bool is_running() override;
  bool start() override;
  bool stop() override;

  //! \brief Indicate to the vm that there are new instructions
  //!        to execute
  void bytecode_ind(std::vector<bytecode::instruction_s> &instructions);

private:
  // Observer for aligned_block class that calls back on OOM
  class instruction_memory_observer_c : public memory::observer_if {
  public:
    instruction_memory_observer_c(vm_c &owner)
      : owner_(owner){}
    void alloc_failure_ind(
        const char* what,
        size_t total_allocated,
        size_t attempted_allocation) override;
  private:
    vm_c &owner_;
  };
  instruction_memory_observer_c ins_mem_observer_;

  instruction_data_s ins_data_;
  std::atomic<bool> running_{false};

  size_t instruction_pointer_{0};

  void run();
};

} // namespace nibi
