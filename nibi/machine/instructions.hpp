#pragma once

#include "machine/defines.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace machine {

namespace {
  constexpr uint8_t INS_DATA_BOUNDARY = 128;
}

//! \brief Instruction operation id
enum class ins_id_e : uint8_t {
  NOP = 0,
  EXEC_ADD,
  EXEC_SUB,
  EXEC_DIV,
  EXEC_MUL,
  
  // ..
  // ..
  // builtins
  // ..
  // ..

  LOAD_STRING = INS_DATA_BOUNDARY,
  LOAD_INT,
  LOAD_REAL,
  LOAD_RESULT,
  LOAD_VAR,
  EXEC_SYMBOL,

  JUMP_TO,  // Byte index of instruction set to jump to

  ENUM_BOUNDARY,
};

struct execution_error_s {
  std::string message;
  // TODO: Add specific, helpful, debug data
};

class instruction_error_handler_if {
public:
  //! \brief Handle an error that arose from
  //!        attempting to execute a specific
  //!        machine instruction.
  //! \note It is the handler's responsibility to 
  //!       know where in the source document that
  //!       the bytecode at the given index was 
  //!       generated from to report the error to
  //!       the user
  virtual void on_error(
      const uint64_t instruction_index,
      const execution_error_s err) = 0;
};
using error_handler_ptr = std::unique_ptr<instruction_error_handler_if>;

class instruction_receiver_if {
public:
  //! \brief Handle a set of instructions
  //! \param instructions The encoded instruction(s)
  //! \param error_handler The handler that should be 
  //!        called upon if an error occurs within the 
  //!        set of instructions
  virtual void handle_instructions(
      const bytes_t& instructions,
      error_handler_ptr error_handler) = 0;
};

#pragma pack(push, 1)
struct instruction_view_s {
  uint8_t op{0};
  std::uint32_t data_len{0};
  uint8_t* data{nullptr};
};
#pragma pack(pop)

#pragma pack(push, 1)
//! \brief A single instruction, along with
//!        its potential data
struct instruction_s {
  const ins_id_e id{ins_id_e::NOP};
  std::unique_ptr<bytes_t> data{nullptr};
};
#pragma pack(pop)


/*
      Byte-encoded instruction layout

      [   id    |      len      |   data ... ]
        1 byte       4 bytes       variable
*/

//! \brief Builder for encoding/ decoding
//!        an instruction set
class instruction_set_builder_c {
public:
  //! \brief Interface to interact with decoded
  //!        instructions 
  //!
  //! \note The set interface should prob. not be used
  //!       for runtime execution of instructions. 
  //!       Each instruction in the list from this 
  //!       generated interface will have 8 bytes overhead
  //!       for EVERY instruction to handle the data pointer.
  //!       - For runtime execution it is recommended that the
  //!         instructions are hand-decoded at execution time.
  class instruction_set_if {
    public:
    virtual ~instruction_set_if() = default;
  
    //! \brief Retrieve instruction at given indec
    //! \param index The index to retrieve
    //! \param okay [out] Returns true iff index was valid
    //! \returns NOP if index is invalid
    [[nodiscard]] virtual const instruction_s& get(
        const size_t index, bool &okay) const = 0;
    [[nodiscard]] virtual const size_t size() const = 0;
  };

  //! \brief Create an empty builder
  instruction_set_builder_c();

  //! \brief Create a builder with encoded bytes
  //!        that need to be decoded to instructions
  instruction_set_builder_c(const bytes_t& data)
    : _data(data) {}

  //! \brief Attempt to build an instruction set from
  //!        the given, or built bytes
  //! \returns nullptr if there were invalid instruction(s)
  //!          detected
  //! \note After this call on success, the builder may be 
  //!       disregarded
  [[nodiscard]] std::unique_ptr<instruction_set_if> 
    get_instruction_interface();

  //! \brief Encode a data-less instruction
  [[nodiscard]] bool encode_instruction(const ins_id_e id);

  //! \brief Encode a data-filled instruction
  [[nodiscard]] bool encode_instruction(
      const ins_id_e id, const bytes_t& data);

  //! \brief Retrieve the raw encoded bytes that the builder
  //!        contains (useful for dumping encoded instructions
  //!        to file / testing)
  [[nodiscard]] inline bytes_t get_bytes() const {
    return _data;
  }

  //! \brief Load encoded bytes into the builder
  //!        so finalize() can be ran against them
  //!        and get them decoded into instructions
  inline void load_bytes(const bytes_t& data) {
    _data = data;
  }

  //! \brief Reset builder
  //! \note This won't invalidate existing, generated
  //!       instruction interfaces
  inline void reset() {
    _data.clear();
  }

private:
  // Helper subclass that does the parsing of bytes
  // and acts as the interface that the user will retrieve
  class instruction_set_c final : public instruction_set_if {
  public:
    [[nodiscard]] bool populate(const bytes_t& data);
    [[nodiscard]] const instruction_s& get(
      const size_t index, bool &okay) const override;
    [[nodiscard]] virtual const size_t size() const override;
  private:
    std::vector<instruction_s> _instructions;
    bytes_t _data;
    instruction_s _nop{};
  };

  // Builder's data
  bytes_t _data;
};

using instruction_if = 
  instruction_set_builder_c::instruction_set_if;

} // namespace 
