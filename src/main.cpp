#include <fmt/format.h>

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

#include "atomiser.hpp"

class receiver_c : public list_receiver_if {
public:
  void on_list(atom_list_t list) override {
    fmt::print("Got list :: ");
    print_atom_list(list);
    fmt::print("\n\n");

  }

  void on_error(file_error_s err) override {
    fmt::print("Got error! : {}\n", err.to_string());
  }
};

int main(int argc, char** argv) {

  std::vector<std::string> input = {
   // "(+ 1",
   // "2.3) (- 10 2)(",
   // "* 4 (+ 10 11))"
 //  "(+ 1 (* 10 2) (/ x ",
 //    "3.33))"
 
//    "(+ a (- b (* d e))",
//      ")(println yay)"

  //  R"((println "I has a \"NESTED \"string\"\"\t\"thing\" astring"))"




    "(+ ; Some commentary",
    "1 2 3) ; Ending commenttary",
    "; One more for fun"
  };
  
  receiver_c recv;

  atomiser_c atomiser("test", recv);
  
  for(size_t i = 0; i < input.size(); i++) {
    atomiser.submit(input[i], i+1);
  }

  return 0;
}
