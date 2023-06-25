#include "lib.hpp"

#include <iostream>
#include <memory>
#include <string>

#include <libnibi/nibi.hpp>
#include <libnibi/macros.hpp>

inline void check_buffer(std::string &buffer, char c) {
  switch (c) {
  case 'n':
    buffer += '\n';
    break;
  case 't':
    buffer += '\t';
    break;
  case 'r':
    buffer += '\r';
    break;
  case 'a':
    buffer += '\a';
    break;
  case 'b':
    buffer += '\b';
    break;
  case 'v':
    buffer += '\v';
    break;
  case '?':
    buffer += '\?';
    break;
  case '"':
    buffer += '\"';
    break;
  case '\'':
    buffer += '\'';
    break;
  case '%':
    buffer += '%';
    break;
  case '\\':
    buffer += '\\';
    break;
  default:
    break;
  }
}

inline void print_cell(nibi::cell_ptr &cell) {

  if (cell->type == nibi::cell_type_e::STRING) {

    std::string buffer;
    auto target = cell->as_string();
    for (auto i = 0; i < target.size(); i++) {
      auto c = target[i];
      if (c == '\\') {
        if (i == target.size() - 1) {
          throw nibi::interpreter_c::exception_c(
              "formated string ends with escape character", cell->locator);
        }
        check_buffer(buffer, target[i + 1]);
        i++;
        continue;
      }
      buffer += c;
    }

    std::cout << buffer;
    return;
  }


  std::cout << cell->to_string();
}


nibi::cell_ptr print(nibi::cell_processor_if &ci, nibi::cell_list_t &list,
                       nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{io print}", >, 1)
  for (auto it = list.begin() + 1; it != list.end(); ++it) {
    if ((*it)->type == nibi::cell_type_e::SYMBOL){
      auto processed = ci.process_cell(*it, env);
      print_cell(processed);
    } else {
      print_cell(*it);
    }
  }
  return nibi::allocate_cell(nibi::cell_type_e::NIL);
}

nibi::cell_ptr get_str(nibi::cell_processor_if &ci, nibi::cell_list_t &list,
                       nibi::env_c &env) {
  std::string line;
  std::getline(std::cin, line);
  return nibi::allocate_cell(line);
}

nibi::cell_ptr get_int(nibi::cell_processor_if &ci, nibi::cell_list_t &list,
                       nibi::env_c &env) {
  std::string line;
  std::getline(std::cin, line);
  int64_t val{0};
  try {
    val = std::stoll(line);
  } catch (std::out_of_range) {
    return nibi::allocate_cell(nibi::cell_type_e::NIL);
  } catch (std::invalid_argument) {
    return nibi::allocate_cell(nibi::cell_type_e::NIL);
  }
  auto r = nibi::allocate_cell((int64_t)val);
  r->locator = list[0]->locator;
  return r;
}

nibi::cell_ptr get_double(nibi::cell_processor_if &ci, nibi::cell_list_t &list,
                          nibi::env_c &env) {
  std::string line;
  std::getline(std::cin, line);
  double val{0};
  try {
    val = std::stod(line);
  } catch (std::out_of_range) {
    return nibi::allocate_cell(nibi::cell_type_e::NIL);
  } catch (std::invalid_argument) {
    return nibi::allocate_cell(nibi::cell_type_e::NIL);
  }
  auto r = nibi::allocate_cell((double)val);
  r->locator = list[0]->locator;
  return r;
}
