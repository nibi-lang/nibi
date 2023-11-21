#pragma once

#include <vector>

using bytes_t = std::vector<uint8_t>;


/*

      ( + 1 2 3 (* 5 5))

      (if (eq 1 1) (putln 1) (putln 0))



      - I want a way to propagate down if the item is in a func/loop/etc
          - Something that can be called more than once. If it is, we can
            optimize it potentially 


      INSTRUCTION_TOP
        in-outer - false
      BLOCK
        LOAD SYM<if>
          BLOCK
            ID <eq>
            INT<1>
            INT<1>

          BLOCK
            LOAD SYM<putln>
            LOAD INT<1>

          BLOCK
            LOAD SYM<putln>
            LOAD INT<0>


    (fn hi (name)
      (putln "hi " name)
      (putln "bye " name))


    INSTRUCTION_TOP
      in-outer - false
      is-fn-def - true
      

    DEFMACRO

      Consider a macro table in the global scope
      of nibi. This table will be accessable from 
      machine and generator. 

      Machine can add/remove scopes of macros as 
      evaluations happen, while generator will
      simply store the relevant data for the macro.

      When a symbol is hit that is a macro, we can call
      the macro table with the 


      (macro x (lhs rhs) (
        (putln lhs rhs)))

        
      (x 3 a)

*/

//! \brief Pack some type T to a vector of bytes
template<
  typename T,
  typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
static inline std::vector<uint8_t> pack(const T value) {
  return std::vector<uint8_t>(
    (uint8_t*)&value,
    (uint8_t*)&(value) + sizeof(T));
}

//! \brief Unpack some vector of bytes to type T
template<
  typename T,
  typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
static inline std::optional<T> unpack(const std::vector<uint8_t>& data) {
  if (data.size() != sizeof(T)) {
    return std::nullopt;
  }
  T* val = (T*)(data.data());
  return {*val};
}

template<
  typename T,
  typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
static inline T quick_unpack(const std::vector<uint8_t>& data) {
  T val = (T*)(data.data());
  return val;
}

static inline std::vector<uint8_t> pack_string(const std::string& str) {
  return std::vector<uint8_t>(str.begin(), str.end());
}

static inline uint64_t real_to_uint64_t(const double& value) {
  return *reinterpret_cast<const uint64_t*>(&value);
}

static inline double real_from_uint64_t(const uint64_t& value) {
  return *reinterpret_cast<const double*>(&value);
}

static constexpr uint8_t INS_DATA_BOUNDARY = 50;

enum class ins_e : uint8_t {
  NOP = 0,
  EXECUTE_LIST, // Present at the end of an instruction list iff builtin wasn't generated
  INTEGER,
  REAL,


  // ----------------------------------------------------------------
  //        Enteries beyond this point require data encoded
  // ----------------------------------------------------------------

  LIST = INS_DATA_BOUNDARY,     // List to execute ()
  SYMBOL,   // Some symbol, may or may not call it
  STRING,
  DEFMACRO,


  // IF,
  // JMP_AHEAD, n-bytes?n-instructions
  // JMP_BACK,  n-bytes?n-instructions
  // DEF_FN
  // ASSIGN
  // SET
  // AT
  // ...
};

struct ins_view_e {
  uint8_t op;
  uint32_t len;
  uint8_t* data;
};

static inline size_t fetch_instruction_size_bytes(const ins_view_e& ins) {
  if (ins.op < INS_DATA_BOUNDARY) { return sizeof(uint8_t); }
  return sizeof(uint8_t) + sizeof(uint32_t) + ins.len;
}


