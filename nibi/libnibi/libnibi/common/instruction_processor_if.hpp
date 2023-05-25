#pragma once

namespace nibi {

//! \brief Something that can process a list of instructions
class instruction_processor_if {
public:
  virtual ~instruction_processor_if() = default;

  //! \brief Execute a list of instructions
  //! \param list The list of instructions to execute
  //! \note Callers are responsible for ensuring that the
  //!       list is a list of instructions, and not a data list
  //!       or accessor list. 
  virtual void instruction_ind(cell_ptr &cell) = 0;
};

} // namespace nibi
