#include "nibi.hpp"

#include "macros.hpp"
#include "object.hpp"
#include "keywords.hpp"
#include "bytecode/bytecode.hpp"

#include <iostream>

#ifndef NIBI_BUILD_HASH
#define NIBI_BUILD_HASH "unknown"
#endif


void show_version() {
  std::cout << "nibi version: " << NIBI_VERSION << std::endl;
  std::cout << "application build hash: " << NIBI_BUILD_HASH << std::endl;
}


int main(int argc, char** argv) {
  show_version();



  return 0;
}
