#include "builtins.hpp"
#include "machine/instructions.hpp"
#include "machine/object.hpp"

namespace front {
namespace builtins {

static builtin_map_t builtin_code;

namespace {
  inline static void add_instruction(
      builtin_s& bs,
      const machine::ins_id_e& id) {
    forge::load_instruction(bs.data, id);
    bs.num_instructions++;
  }
  inline static void add_instruction(
      builtin_s& bs,
      const machine::ins_id_e& id,
      const machine::bytes_t& data) {
    forge::load_instruction(bs.data, id, data);
    bs.num_instructions++;
  }
  inline static void expect_type(
      builtin_s& bs,
      const machine::data_type_e& type) {
      add_instruction(
        bs,
        machine::ins_id_e::EXPECT_OBJECT_TYPE,
        machine::tools::pack<uint8_t>(
          (uint8_t)type));
  }
  inline static void expect_none(builtin_s& bs) {
    return expect_type(bs, machine::data_type_e::NONE);
  }
  inline static void expect_boolean(builtin_s& bs) {
    return expect_type(bs, machine::data_type_e::BOOLEAN);
  }
  inline static void expect_integer(builtin_s& bs) {
    return expect_type(bs, machine::data_type_e::INTEGER);
  }
  inline static void expect_real(builtin_s& bs) {
    return expect_type(bs, machine::data_type_e::REAL);
  }
  inline static void expect_string(builtin_s& bs) {
    return expect_type(bs, machine::data_type_e::STRING);
  }
  inline static void expect_bytes(builtin_s& bs) {
    return expect_type(bs, machine::data_type_e::BYTES);
  }
  inline static void expect_ref(builtin_s& bs) {
    return expect_type(bs, machine::data_type_e::REF);
  }
  inline static void expect_error(builtin_s& bs) {
    return expect_type(bs, machine::data_type_e::ERROR);
  }
  inline static void expect_exactly_n_args(
    builtin_s& bs, const uint8_t& n) {
    add_instruction(
      bs,
      machine::ins_id_e::EXPECT_N_ARGS,
      machine::tools::pack<uint8_t>(n));
  }
  inline static void expect_at_least_n_args(
    builtin_s& bs, const uint8_t& n) {
    add_instruction(
      bs,
      machine::ins_id_e::EXPECT_GTE_N_ARGS,
      machine::tools::pack<uint8_t>(n));
  }
} // namespace
  

builtin_s builtin_let() {

  builtin_s code;

  expect_exactly_n_args(code, 2);

  expect_string(code);

  add_instruction(
    code,
    machine::ins_id_e::EXEC_ASSIGN);

  // we may want to place the value back on the queue as
  // a ref to the obejct
  return code;
}

builtin_map_t& get_builtins() {
  if (!builtin_code.empty()) { return builtin_code; }

  builtin_code["let"] = builtin_let();

  return builtin_code;
}

} // namespace
} // namespace
