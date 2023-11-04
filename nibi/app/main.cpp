#include "front/lexer.hpp"
#include "front/file_intake.hpp"

#include "machine/instructions.hpp"

#include <iostream>


class atom_list_receiver_c final : public front::atom_receiver_if {
public:
  virtual void on_error(front::error_s err) override {
    std::cout << "Error (" << err.pos.line << ":" << err.pos.col << ") " << err.message << std::endl;
    std::exit(1);
  }

  virtual void on_list(front::atom_list_t list) override {
    std::cout << "Received list of length: " << list.size() << std::endl;
    front::print_list(list);
  }

  virtual void on_top_list_complete() override {
    std::cout << "[ A list was completed! ]" << std::endl;
  }
};

void check_front();
void check_ins_alloc();

int main(int argc, char **argv) {

 // check_front();

 // atom_list_receiver_c prc;
//  if (!front::from_file(prc, "example.nibi")) {
//    std::cerr << "Failed to parse file\n";
//    return 1;
//  }
//

  check_ins_alloc();

  return 0;
}

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

void check_ins_alloc() {

  std::cout << "sizeof instruction_c: " << sizeof(machine::instruction_c) << std::endl;
  std::cout << "sizeof instruction_data_c: " << sizeof(machine::instruction_data_c) << std::endl;
  
  // Block size must be > sizeof(instruction_data_c)
  {

    machine::ins_blocks_t instruction_blocks;
    machine::instruction_block_builder_c ibb(instruction_blocks);
    
    auto* new_ins = ibb.alloc(machine::ins_id_e::EXEC_SYMBOL, {});
    if (nullptr == new_ins) {
      std::cerr << "New ins was unable to be allocated" << std::endl;
      std::exit(1);
    }
    std::cout << (int)(static_cast<machine::instruction_c*>(new_ins))->id << std::endl;

    auto* another_ins = ibb.alloc(machine::ins_id_e::LOAD_INT, {});
    if (nullptr == another_ins) {
      std::cerr << "Another ins was unable to be allocated" << std::endl;
      std::exit(1);
    }

    std::cout << (int)(static_cast<machine::instruction_c*>(another_ins))->id << std::endl;

    std::cout << "There are a total of : " << ibb.block_count() << " blocks" << std::endl;
  }

}

