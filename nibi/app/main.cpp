#include <parser/parser.hpp>
#include <parser/file_parser.hpp>

#include <iostream>


class parser_receiver_c final : public parser::receiver_if {
public:
  virtual void on_error(parser::error_s err) override {
    std::cout << "Error (" << err.line << ":" << err.col << ") " << err.message << std::endl;
    std::exit(1);
  }

  virtual void on_list(parser::list_t list) override {
    std::cout << "Received list of length: " << list.size() << std::endl;
    parser::print_list(list);
  }

  virtual void on_top_list_complete() override {
    std::cout << "[ A list was completed! ]" << std::endl;
  }
};

void check_parser();

int main(int argc, char **argv) {

  //check_parser();

  

  parser_receiver_c prc;
  if (!parser::from_file(prc, "example.nibi")) {
    std::cerr << "Failed to parse file\n";
    return 1;
  }


  return 0;
}

void check_parser() {

  parser_receiver_c prc;

  parser::parser_c parser(prc);

  parser.submit("(+ (- - - - (* * *)  - - ) + + + )");
  parser.finish();
  std::cout << "\n\n";

  parser.submit("(<= + ;Now this should be okay", 1);
  parser.submit(R"("This is a \"string\"")", 2);
  parser.submit(")", 3);
  parser.finish();
  std::cout << "\n\n";

  parser.submit("(do-something p1 p2 -10 4 3.12)");
  parser.finish();
  std::cout << "\n\n";

  parser.submit("(<= 1 2)");
  parser.finish();
  std::cout << "\n\n";

  parser.submit("(<= 1 (- ");
  parser.submit("1 2))");
  parser.finish();
  std::cout << "\n\n";
  // TODO: Add tests. CPPUTests? Google tests? home made tests?



}
