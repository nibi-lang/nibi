#pragma once

#include <atomic>
#include <vector>

#include "source.hpp"
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
  locator_table_c &locator_table_;
  std::atomic<bool> running_{false};

  size_t num_ins_{0};
  bytecode::instruction_s *instructions_{nullptr};

  // pc
  // etc
};

}
