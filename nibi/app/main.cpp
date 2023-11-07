#include "front/lexer.hpp"
#include "front/intake.hpp"

#include "machine/defines.hpp"
#include "machine/instructions.hpp"
#include "machine/byte_tools.hpp"

#include <iostream>

namespace {
  front::intake::settings_s settings;
}

int main(int argc, char **argv) {

  return front::intake::repl(settings);
}


/*
void check_front() {
  atom_list_receiver_c prc;

  front::lexer_c lexer(prc);

  lexer.submit("(let x :u8 0)");
  lexer.finish();
  std::cout << "\n\n";

  lexer.submit("(+ (- - - - (* * *)  - - ) + + + )");
  lexer.finish();
  std::cout << "\n\n";

  lexer.submit("(<- + ;Now this should be okay", 1);
  lexer.submit(R"("This is a \"string\"")", 2);
  lexer.submit(")", 3);
  lexer.finish();
  std::cout << "\n\n";

  lexer.submit("(do-something p1 p2 -10 4 3.12)");
  lexer.finish();
  std::cout << "\n\n";

  lexer.submit("(<- 1 2)");
  lexer.finish();
  std::cout << "\n\n";

  lexer.submit("(<- 1 (- ");
  lexer.submit("1 2))");
  lexer.finish();
  std::cout << "\n\n";
  // TODO: Add tests. CPPUTests? Google tests? home made tests?
}

void check_ins_builder() {

  std::cout << "MAIN: Checking machine::instruction*"
            << std::endl;

  std::cout << "Instruction size: "
            << sizeof(machine::instruction_s)
            << "\n";

  struct test_case_s {
    machine::ins_id_e id{machine::ins_id_e::ENUM_BOUNDARY};
    machine::bytes_t data{};
  };

  std::vector<test_case_s> tcs {

    { machine::ins_id_e::LOAD_INT,
      machine::tools::pack<uint32_t>(420) },

    { machine::ins_id_e::LOAD_REAL,
      machine::tools::pack<uint32_t>(
        machine::tools::real_to_uint64_t(8.88)) },

    { machine::ins_id_e::LOAD_INT,
      machine::tools::pack<uint32_t>(69) },

    { machine::ins_id_e::LOAD_REAL,
      machine::tools::pack<uint32_t>(
        machine::tools::real_to_uint64_t(3.14159)) },
    
    { machine::ins_id_e::EXEC_ADD, {} },

    { machine::ins_id_e::EXEC_DIV, {} },
    
    { machine::ins_id_e::EXEC_ADD, {} },
    
    { machine::ins_id_e::LOAD_INT,
      machine::tools::pack<uint32_t>(24) },

    { machine::ins_id_e::LOAD_REAL,
      machine::tools::pack<uint32_t>(
        machine::tools::real_to_uint64_t(2.71828)) },

    { machine::ins_id_e::EXEC_MUL, {} }
  };

  machine::instruction_set_builder_c isbc;

  for (auto&& ins : tcs) {

    std::cout << "MAIN: Encoding id [" << (int)ins.id << "]\n";

    if ((uint8_t)ins.id < machine::INS_DATA_BOUNDARY) { 
      if (!isbc.encode_instruction(ins.id)) {
        std::cerr << "Failed to encode non-data ins : "
                  << (int)ins.id
                  << std::endl;
        std::exit(1);
      }
      continue;
    }

    if (!isbc.encode_instruction(ins.id, ins.data)) {
      std::cerr << "Failed to encode data ins : "
                << (int)ins.id
                << std::endl;
      std::exit(1);
    }
  }

  std::cout << "MAIN: Finalizing and checking..\n";

  std::unique_ptr<machine::instruction_if> insif =
    isbc.get_instruction_interface();

  if (insif == nullptr) {
    std::cerr << "Failed to finalize bytecode" << std::endl;
    std::exit(1);
  }

  if (tcs.size() != insif->size()) {
    std::cout << "Different number of instructions came out..."
              << "Expected "
              << tcs.size()
              << ", got "
              << insif->size()
              << std::endl;
    std::exit(1);
  }

  for(size_t i = 0; i < tcs.size(); i++) {
    auto &tc = tcs[i];
    bool okay{false};
    auto &actual = insif->get(i, okay);
    if (!okay) {
      std::cout << "Failed to get instruction from if - okay=false\n";
      std::exit(1);
    }

    if (tc.id != actual.id) {
      std::cout << "Incorrect id "
                << (int)tc.id 
                << " != "
                << (int)actual.id
                << std::endl;
      std::exit(1);
    }

    if ((!actual.data) && tc.data.size()) {
      std::cout << "Expected data, got nullopt" << std::endl;
      std::exit(1);
    }

    if (actual.data &&
        tc.data.size() != (*actual.data).size()) {
      std::cout << "Incorrect instruction length. "
                << tc.data.size()
                << " != "
                << (*actual.data).size()
                << std::endl;
      std::exit(1);
    }

    std::cout << "Checking "
              << tc.data.size()
              << " bytes for ["
              << (int)tc.id
              << "]" 
              << std::endl;

    for (auto j = 0; j < tc.data.size(); j++) {
      if ((*actual.data)[j] != tc.data[j]) {
        std::cout << "Incorrect byte at index " << j << ": " 
                  << (int)tc.data[j] << " != "
                  << (int)((*actual.data)[j])
                  << std::endl;
        std::exit(1);
      }
    }
  }

  std::cout << "PASS" << std::endl;
}

*/
