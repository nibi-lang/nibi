#include <fmt/format.h>

#include "lang/input/atomiser.hpp"


class receiver_c final : public atom_receiver_if {
  public:

  void on_error (
    const std::size_t line,
    const std::size_t& col,
    const std::string& message) override {
    fmt::print("Error: ({}, {}): {}\n", line, col, message);
  }

  void on_list(atom_list_t list) override {
    fmt::print("Got list: ");
    print_atom_list(list);
    fmt::print("\n");
  }
};


int main(int argc, char**argv) {

  receiver_c receiver;
  atomiser_c atomiser(receiver);

  //atomiser.submit(R"(
  //  ;(+ 1 2)
  //  ;(+ 1 5)
  //  ;(+ a 1 (- 5 6))
  //  # This is a thing
  //  ;(- (+ 1 2 3) (* (+ 5 (+ 6 7)))) ; This is a line
  //  

  //  (putln x (+ 1 2 3 4 9 0) (x 54))(+ 1 2)

  // ; (+ x)
  //  )");
 
  atomiser.submit("(+ 1 2");
  atomiser.submit("(- 10 2))");

  atomiser.indicate_complete();
  return 0;
}
