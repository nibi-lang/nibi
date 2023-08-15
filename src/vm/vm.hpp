#pragma once

#include <atomic>
#include <deque>
#include <stack>
#include <vector>

#include "source.hpp"
#include "aligned_block.hpp"
#include "instruction_data.hpp"
#include "bytecode/bytecode.hpp"
#include "interfaces/process_if.hpp"
#include "object.hpp"

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
  size_t instruction_pointer_{1};

  struct execution_ctx_s {
    int64_t handler_id{0};
    std::vector<base_object_c*> list;

    execution_ctx_s(const int64_t id) : handler_id(id) {}
    // TODO: Investigate Opt:
    // If we could determine the actual number of items
    // that would be resolved per ctx we could reserve the list
  };
  std::stack<execution_ctx_s> stack_;

  void execute_current_instruction();
  size_t execute_instruction(const size_t idx);

  void builtin_add();

  locator_ptr retrieve_locator(const size_t idx);
  void execute_ctx(const size_t idx);
};

} // namespace nibi
