#include <fmt/format.h>

#include "atomiser.hpp"
#include "encoder.hpp"

int main(int argc, char** argv) {

  std::optional<parse_group_s>file_group = atomise_file("nib.test_file");

  if (!file_group.has_value()) {
    fmt::print("No parse group returned\n");
    return 1;
  }

  fmt::print(
    "File: '{}' returned '{}' lists\n",
    (*file_group).origin,
    (*file_group).lists.size());

  {
    size_t n = 0;
    for(auto&& list : (*file_group).lists) {
      fmt::print("List[{}]: ", n++);
      print_atom_list(list);
      fmt::print("\n");
    }
  }

  auto err_cb = [](const std::string& e, const file_position_s& pos) {
    fmt::print("Encoding error{}: {}", pos.to_string(), e);
    std::exit(1);
  };

  {
    size_t n = 0;
    for(auto &&list : (*file_group).lists) {
      bytes_t bytes = generate_instructions(
        list,
        err_cb);
      fmt::print("Generated {} bytes for list {}\n", bytes.size(), n++);
    }
  }

  return 0;
}
