#include "lib.hpp"
#include <iostream>
#include <libnibi/macros.hpp>
#include <set>
#include <string>

namespace {
std::set<std::string> valid_modes = {"r",  "rb",  "w",  "wb",  "a",  "ab",
                                     "r+", "rb+", "w+", "wb+", "a+", "ab+"};
}

nibi::cell_ptr nibi_fopen(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                          nibi::env_c &env) {

  NIBI_LIST_ENFORCE_SIZE("{fileio fopen}", ==, 3)

  auto arg1 = ci.process_cell(list[1], env);
  auto arg2 = ci.process_cell(list[2], env);

  if (arg1->type != nibi::cell_type_e::STRING) {
    throw nibi::interpreter_c::exception_c("arg1 must be of type `string`",
                                           list[1]->locator);
  }
  if (arg2->type != nibi::cell_type_e::STRING) {
    throw nibi::interpreter_c::exception_c("arg2 must be of type `string`",
                                           list[2]->locator);
  }
  if (!valid_modes.contains(arg2->as_string())) {
    throw nibi::interpreter_c::exception_c("Invalid mode", list[2]->locator);
  }

  auto result = nibi::allocate_cell(nibi::cell_type_e::PTR);
  result->data.ptr = fopen(arg1->to_string().data(), arg2->to_string().data());
  return result;
}

nibi::cell_ptr nibi_fclose(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                           nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{fileio fclose}", ==, 2)
  auto arg1 = ci.process_cell(list[1], env);
  if (arg1->type != nibi::cell_type_e::PTR) {
    throw nibi::interpreter_c::exception_c("arg1 must be of type `ptr`",
                                           list[1]->locator);
  }
  if (arg1->data.ptr == nullptr) {
    return nibi::allocate_cell((int64_t)-1);
  }
  int64_t result = (fclose((FILE *)arg1->data.ptr));
  if (0 == result) {
    arg1->data.ptr = nullptr;
  }
  return nibi::allocate_cell(result);
}

nibi::cell_ptr nibi_fputs(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                          nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{fileio fputs}", ==, 3)
  auto arg1 = ci.process_cell(list[1], env);
  auto arg2 = ci.process_cell(list[2], env);
  if (arg1->type != nibi::cell_type_e::PTR) {
    throw nibi::interpreter_c::exception_c("arg1 must be of type `ptr`",
                                           list[1]->locator);
  }
  if (arg2->type != nibi::cell_type_e::STRING) {
    throw nibi::interpreter_c::exception_c("arg2 must be of type `string`",
                                           list[2]->locator);
  }
  return nibi::allocate_cell(
      (int64_t)(fputs(arg2->data.cstr, (FILE *)arg1->data.ptr)));
}

nibi::cell_ptr nibi_fprintf(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                            nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{fileio fprintf}", ==, 4)
  auto arg1 = ci.process_cell(list[1], env);
  auto arg2 = ci.process_cell(list[2], env);
  auto arg3 = ci.process_cell(list[3], env);
  if (arg1->type != nibi::cell_type_e::PTR) {
    throw nibi::interpreter_c::exception_c("arg1 must be of type `ptr`",
                                           list[1]->locator);
  }
  if (arg2->type != nibi::cell_type_e::STRING) {
    throw nibi::interpreter_c::exception_c("arg2 must be of type `string`",
                                           list[2]->locator);
  }
  if (arg3->type != nibi::cell_type_e::STRING) {
    throw nibi::interpreter_c::exception_c("arg23must be of type `string`",
                                           list[3]->locator);
  }
  return nibi::allocate_cell((int64_t)(fprintf(
      (FILE *)arg1->data.ptr, arg2->data.cstr, arg3->data.cstr)));
}

nibi::cell_ptr nibi_fwrite(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                           nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{fileio fwrite}", ==, 5)
  auto arg1 = ci.process_cell(list[1], env);
  auto arg2 = ci.process_cell(list[2], env);
  auto arg3 = ci.process_cell(list[3], env);
  auto arg4 = ci.process_cell(list[4], env);
  if (arg1->type != nibi::cell_type_e::PTR) {
    throw nibi::interpreter_c::exception_c("arg1 must be of type `ptr`",
                                           list[1]->locator);
  }
  if (arg2->type != nibi::cell_type_e::I64) {
    throw nibi::interpreter_c::exception_c("arg2 must be of type `i64`",
                                           list[2]->locator);
  }
  if (arg3->type != nibi::cell_type_e::I64) {
    throw nibi::interpreter_c::exception_c("arg3 must be of type `i64`",
                                           list[3]->locator);
  }
  if (arg4->type != nibi::cell_type_e::PTR) {
    throw nibi::interpreter_c::exception_c("arg4 must be of type `ptr`",
                                           list[4]->locator);
  }

  return nibi::allocate_cell(
      (int64_t)(fwrite(arg2->data.cstr, arg3->data.i64, arg4->data.i64,
                       (FILE *)arg1->data.ptr)));
}

nibi::cell_ptr nibi_fgets(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                          nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{fileio fgets}", ==, 3)
  auto arg1 = ci.process_cell(list[1], env);
  auto arg2 = ci.process_cell(list[2], env);
  if (arg1->type != nibi::cell_type_e::PTR) {
    throw nibi::interpreter_c::exception_c("arg1 must be of type `ptr`",
                                           list[1]->locator);
  }
  if (arg2->type != nibi::cell_type_e::I64) {
    throw nibi::interpreter_c::exception_c("arg2 must be of type `i64`",
                                           list[2]->locator);
  }
  auto result_cell = nibi::allocate_cell(nibi::cell_type_e::NIL);
  result_cell->type = nibi::cell_type_e::STRING;

  // Freeing of string will be done by the cell
  result_cell->data.cstr = new char[arg2->data.i64 + 1];
  fgets(result_cell->data.cstr, arg2->data.i64 + 1, (FILE *)arg1->data.ptr);
  return result_cell;
}

nibi::cell_ptr nibi_fgetc(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                          nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{fileio fgetc}", ==, 2)
  auto arg1 = ci.process_cell(list[1], env);
  if (arg1->type != nibi::cell_type_e::PTR) {
    throw nibi::interpreter_c::exception_c("arg1 must be of type `ptr`",
                                           list[1]->locator);
  }
  if (arg1->data.ptr == nullptr) {
    return nibi::allocate_cell((int64_t)-1);
  }

  auto result_cell = nibi::allocate_cell(nibi::cell_type_e::NIL);
  result_cell->type = nibi::cell_type_e::STRING;

  // Freeing of string will be done by the cell
  result_cell->data.cstr = new char[2];
  fgets(result_cell->data.cstr, 2, (FILE *)arg1->data.ptr);
  return result_cell;
}

nibi::cell_ptr nibi_feof(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                         nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{fileio feof}", ==, 2)
  auto arg1 = ci.process_cell(list[1], env);
  if (arg1->type != nibi::cell_type_e::PTR) {
    throw nibi::interpreter_c::exception_c("arg1 must be of type `ptr`",
                                           list[1]->locator);
  }
  if (arg1->data.ptr == nullptr) {
    return nibi::allocate_cell((int64_t)-1);
  }
  return nibi::allocate_cell((int64_t)(feof((FILE *)arg1->data.ptr)));
}

nibi::cell_ptr nibi_ftell(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                          nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{fileio ftell}", ==, 2)
  auto arg1 = ci.process_cell(list[1], env);
  if (arg1->type != nibi::cell_type_e::PTR) {
    throw nibi::interpreter_c::exception_c("arg1 must be of type `ptr`",
                                           list[1]->locator);
  }
  if (arg1->data.ptr == nullptr) {
    return nibi::allocate_cell((int64_t)-1);
  }
  return nibi::allocate_cell((int64_t)(ftell((FILE *)arg1->data.ptr)));
}

nibi::cell_ptr nibi_rewind(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                           nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{fileio rewind}", ==, 2)
  auto arg1 = ci.process_cell(list[1], env);
  if (arg1->type != nibi::cell_type_e::PTR) {
    throw nibi::interpreter_c::exception_c("arg1 must be of type `ptr`",
                                           list[1]->locator);
  }
  if (arg1->data.ptr == nullptr) {
    return nibi::allocate_cell((int64_t)-1);
  }
  rewind((FILE *)arg1->data.ptr);
  return nibi::allocate_cell((int64_t)0);
}

nibi::cell_ptr nibi_clearerr(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                             nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{fileio rewind}", ==, 2)
  auto arg1 = ci.process_cell(list[1], env);
  if (arg1->type != nibi::cell_type_e::PTR) {
    throw nibi::interpreter_c::exception_c("arg1 must be of type `ptr`",
                                           list[1]->locator);
  }
  if (arg1->data.ptr == nullptr) {
    return nibi::allocate_cell((int64_t)-1);
  }
  clearerr((FILE *)arg1->data.ptr);
  return nibi::allocate_cell((int64_t)0);
}

nibi::cell_ptr nibi_ferror(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                           nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{fileio ferror}", ==, 2)
  auto arg1 = ci.process_cell(list[1], env);
  if (arg1->type != nibi::cell_type_e::PTR) {
    throw nibi::interpreter_c::exception_c("arg1 must be of type `ptr`",
                                           list[1]->locator);
  }
  if (arg1->data.ptr == nullptr) {
    return nibi::allocate_cell((int64_t)-1);
  }
  return nibi::allocate_cell((int64_t)(ferror((FILE *)arg1->data.ptr)));
}

nibi::cell_ptr nibi_fseek(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                          nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{fileio fseek}", ==, 4)
  auto arg1 = ci.process_cell(list[1], env);
  auto arg2 = ci.process_cell(list[2], env);
  auto arg3 = ci.process_cell(list[3], env);
  if (arg1->type != nibi::cell_type_e::PTR) {
    throw nibi::interpreter_c::exception_c("arg1 must be of type `ptr`",
                                           list[1]->locator);
  }
  if (arg2->type != nibi::cell_type_e::I64) {
    throw nibi::interpreter_c::exception_c("arg2 must be of type `i64`",
                                           list[2]->locator);
  }
  if (arg3->type != nibi::cell_type_e::I64) {
    throw nibi::interpreter_c::exception_c("arg3 must be of type `i64`",
                                           list[3]->locator);
  }
  return nibi::allocate_cell(
      (int64_t)(fseek((FILE *)arg1->data.ptr, arg2->data.i64, arg3->data.i64)));
}
