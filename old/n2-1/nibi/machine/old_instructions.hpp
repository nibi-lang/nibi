#pragma once

#include <string>
#include <vector>
#include <array>
#include <limits>
#include <cstdint>
#include <iostream> // Temp for cerr

namespace machine {

namespace {
  constexpr uint8_t MAX_DATA_INS_ID = 6;
}

static constexpr uint16_t INS_BLOCK_BYTE_SIZE = 1024 * 32; // 32 kibibytes
static constexpr uint64_t MAX_REFERENCEABLE_INS_BLOCKS = 0xFFFFFFFFFFFF000A; // 6 bytes max

enum class ins_id_e {
  NOP = 0,
  LOAD_STRING,
  LOAD_INT,
  LOAD_REAL,
  LOAD_RESULT,
  LOAD_VAR,
  EXEC_SYMBOL = MAX_DATA_INS_ID, // User-defind

  EXEC_ADD,
  EXEC_SUB,
  EXEC_DIV,
  EXEC_MUL,
  
  // ..
  // ..
  // builtins
  // ..
  // ..
};

using bytes_t = std::vector<uint8_t>;

class instruction_c {
  public:
  instruction_c(const ins_id_e id)
    : id(id) {}
  ins_id_e id{ins_id_e::NOP};
  protected:
  ~instruction_c(){}
};

class instruction_data_c final : public instruction_c {
  public:
  instruction_data_c(const ins_id_e id, bytes_t data)
    : instruction_c(id),
      data(data){}
  bytes_t data;
  private:
  ~instruction_data_c(){}
};

//! \brief A contiguous block of bytes meant to store
//!        instructions in
struct instruction_block_s {
  std::array<uint8_t, INS_BLOCK_BYTE_SIZE> bytes{0};
  uint16_t bytes_allocated{0};
};

using ins_blocks_t = std::vector<instruction_block_s>;

//! \brief Constructs instruction blocks storing instruction_c
//!        and instruction_data_c in fixed contiguous memory blocks.
class instruction_block_builder_c {
public:

  instruction_block_builder_c() = delete;

  //! \brief Creates the builder
  //! \param blockss The blocks list that will be utilized to 
  //!        to store the program blocks
  instruction_block_builder_c(ins_blocks_t &blocks) : _blocks(blocks) {
    static_assert(sizeof(instruction_data_c) <= INS_BLOCK_BYTE_SIZE);
    _blocks.push_back(instruction_block_s());
  }

  //! \brief Allocate a regular instruction_c
  //! \param id Instruction to create
  //! \returns nullptr iff the item could not be allocated 
  //!          due to an incorrect id for the instruction type
  [[nodiscard]] instruction_c* alloc(const ins_id_e id) {
    if ((size_t)id <= MAX_DATA_INS_ID && id != ins_id_e::NOP) {
      return nullptr;
    }
    if (!check_vacancy(sizeof(instruction_c))) { return nullptr; }
    auto& active_block = _blocks[_idx];
    auto* ins = new (active_block.bytes.data() + active_block.bytes_allocated) instruction_c (id);
    active_block.bytes_allocated += sizeof(instruction_c);
    return ins;
  }

  //! \brief Allocate an instruction_data_c
  //! \param id Instruction to create
  //! \param data The data associated with the instruction
  //! \returns nullptr iff the item could not be allocated 
  //!          due to an incorrect id for the instruction type
  [[nodiscard]] instruction_data_c* alloc(const ins_id_e id, bytes_t data) {
    if ((size_t)id > MAX_DATA_INS_ID || id == ins_id_e::NOP) {
      return nullptr;
    }
    if (!check_vacancy(sizeof(instruction_data_c))) { return nullptr; }
    auto& active_block = _blocks[_idx];
    auto* ins = new (active_block.bytes.data() + active_block.bytes_allocated) instruction_data_c (id, data);
    active_block.bytes_allocated += sizeof(instruction_data_c);
    return ins;
  }

  //! \brief Retrieve the number of blocks the builder list has
  [[nodiscard]] size_t block_count() { return _blocks.size(); }

private:
  [[nodiscard]] inline bool check_vacancy(const size_t size) {
    if (block_can_accomodate(_blocks[_idx], size)) {
      return true;
    }
    _blocks.push_back(instruction_block_s{});
    _idx++;

    // Ensure we can reference the size using 6 bytes
    return (_blocks.size() < MAX_REFERENCEABLE_INS_BLOCKS);
  }

  [[nodiscard]] bool block_can_accomodate(
    const instruction_block_s& block, const size_t size) {
    return block.bytes_allocated + size <= INS_BLOCK_BYTE_SIZE;
  }

  uint64_t _idx{0};
  ins_blocks_t& _blocks;
};

//! \brief Object that allows the sequential reading 
//!        variable-length instructions from a list of
//!        a list of instruction blocks
//! \note The addressing of instructions splits a uint64_t into
//!       two parts. The upper 6 bytes are used to index into
//!       the list of blocks, and the lower 2 bytes are used to 
//!       index into the block data itself.
class instruction_block_reader_c {
public:

  //! \brief Observer for block reader
  class observer_c {
  public:
    virtual void on_block_index_error(
      const uint64_t block_idx) = 0;
    virtual void on_instruction_index_error(
      const uint64_t block_idx, const uint64_t instruction_idx) = 0;
  };

  static constexpr uint64_t BLOCK_INS_IDX_MASK = 0xFFFF;
  static constexpr uint64_t BLOCK_IDX_MASK = 0xFFFFFFFFFFFF;
  
  instruction_block_reader_c() = delete;
  instruction_block_reader_c(ins_blocks_t &blocks) : _blocks(blocks){
    static_assert(INS_BLOCK_BYTE_SIZE <= std::numeric_limits<uint16_t>::max());
  }

  void set_observer(observer_c &o) {
    _observer = &o;
  }

  //! \brief Retrieve the value of the current address data
  uint64_t current_address() const { return _current_address; }

  //! \brief Retrieve current instruction without incrementing
  instruction_c* current_instruction() {
    ins_data_s ids = get_ins_data(_current_address, _blocks);
    return ids.ins;
  }

  //! \brief Increment the reader to the next instruction
  //! \returns true iff there are more instructions to read
  bool increment() {
    ins_data_s ids = get_ins_data(_current_address, _blocks);

    uint64_t next_instruction_idx = get_instruction_size(ids) + ids.ins_idx;

    auto& current_block = _blocks[ids.block_idx];

    // Check if we hit the end of the allocated bytes in the current block
    if (next_instruction_idx >= current_block.bytes_allocated) {
        uint64_t next_block_idx = ids.block_idx + 1;
        if (next_block_idx >= _blocks.size()) { return false; }

        set_block_index(next_block_idx);
        return true;
    }

    set_instruction_index(next_instruction_idx);
    return true;
  }

  //! \brief Decode the index of the current block within the block list
  //!        of a given address
  inline static uint64_t decode_block_index(const uint64_t& address) {
    return (uint64_t)(address >> 16);  
  }
  
  //! \brief Decode the index of the instruction within a block 
  //!        of a given address
  inline static uint16_t decode_block_instruction_index(const uint64_t& address) {
    return (uint16_t)(address & BLOCK_INS_IDX_MASK);
  }

private:
  struct ins_data_s {
    uint64_t block_idx{0};
    uint16_t ins_idx{0};
    instruction_c* ins{nullptr};
  };

  size_t get_instruction_size(ins_data_s& ids) {
    if (ids.ins->id == ins_id_e::NOP ||
        (uint8_t)ids.ins->id > MAX_DATA_INS_ID) {
        return sizeof(instruction_c);
      }
    return sizeof(instruction_data_c);
  }

  void set_instruction_index(const uint16_t& idx) {
      _current_address &= ~((uint64_t)BLOCK_INS_IDX_MASK);
      _current_address |= (uint64_t)idx;
  }
  
  void set_block_index(const uint64_t idx) {
      _current_address &= ~((uint64_t)BLOCK_IDX_MASK << 16);
      _current_address |= idx << 16;
  }

  inline ins_data_s get_ins_data
    (const uint64_t& addr, const ins_blocks_t &blocks) {
    uint64_t block_idx = decode_block_index(addr);
    uint16_t ins_idx = decode_block_instruction_index(addr);

    if (block_idx >= blocks.size()) {
      if (_observer) { _observer->on_block_index_error(block_idx); }
    }

    auto& current_bytes = blocks[block_idx].bytes;

    if (ins_idx >= current_bytes.size()) {
      if (_observer) { _observer->on_instruction_index_error(block_idx, ins_idx); }
    }

    auto* ins = (instruction_c*)current_bytes.data() + ins_idx;
    return ins_data_s { block_idx, ins_idx, ins };
  }

  // First 6 bytes is the block number,
  // second 2 bytes is the index into the block
  uint64_t _current_address{0};

  ins_blocks_t &_blocks;

  observer_c *_observer{nullptr};
};

} // namespace
