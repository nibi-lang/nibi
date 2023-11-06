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
  ENUM_BOUNDARY,
};

#pragma pack(push, 1)
//! \brief A single instruction, along with
//!        its potential data
struct instruction_s {
  const ins_id_e id{ins_id_e::NOP};
  std::unique_ptr<bytes_t> data{nullptr};
};
#pragma pack(pop)

//! \brief Builder for encoding/ decoding
//!        an instruction set
class instruction_set_builder_c {
public:
  //! \brief Interface to interact with decoded
  //!        instructions 
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
  [[nodiscard]] std::unique_ptr<instruction_set_if> finalize();

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
