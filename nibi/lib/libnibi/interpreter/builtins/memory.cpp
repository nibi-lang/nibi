#include "interpreter/builtins/builtins.hpp"
#include "libnibi/cell.hpp"
#include "libnibi/keywords.hpp"
#include "libnibi/type_names.hpp"
#include "macros.hpp"

#include <iostream>

namespace nibi {

namespace builtins {

cell_ptr builtin_fn_memory_new(cell_processor_if &ci,
                                        cell_list_t &list, env_c &env){
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::MEM_NEW, ==, 2)

  auto ptr_cell = allocate_cell(cell_type_e::PTR);
  if (list[1]->type == cell_type_e::NIL) {
    return ptr_cell;
  }

  auto size = ci.process_cell(list[1], env)->as_integer();

  ptr_cell->data.ptr = malloc(size);
  ptr_cell->complex_data = pointer_info_s{ true, size };
  return ptr_cell;
}

cell_ptr builtin_fn_memory_del(cell_processor_if &ci,
                                        cell_list_t &list, env_c &env){
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::MEM_DEL, ==, 2)

  auto ptr = ci.process_cell(list[1], env);
  auto &ptr_info = ptr->as_pointer_info();

  if (!ptr_info.is_owned) {
    throw interpreter_c::exception_c("Can not delete non-owned memory", list[1]->locator);
  }

  if (ptr->data.ptr != nullptr) {
    free(ptr->data.ptr);
  }

  ptr->data.ptr = nullptr;
  ptr_info.is_owned = false;
  ptr_info.size_bytes = std::nullopt;
  return ptr;
}

cell_ptr builtin_fn_memory_owned(cell_processor_if &ci,
                                        cell_list_t &list, env_c &env){ 
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::MEM_OWNED, ==, 2)
  auto ptr = ci.process_cell(list[1], env);
  auto &ptr_info = ptr->as_pointer_info();
  return allocate_cell((int64_t)ptr_info.is_owned);
}

cell_ptr builtin_fn_memory_abandon(cell_processor_if &ci,
                                        cell_list_t &list, env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::MEM_ABANDON, ==, 2)
  auto ptr = ci.process_cell(list[1], env);
  auto &ptr_info = ptr->as_pointer_info();
  ptr_info.is_owned = false;
  return ptr;
}

cell_ptr builtin_fn_memory_acquire(cell_processor_if &ci,
                                        cell_list_t &list, env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::MEM_ACQUIRE, ==, 2)
  auto ptr = ci.process_cell(list[1], env);
  auto &ptr_info = ptr->as_pointer_info();
  ptr_info.is_owned = true;
  return ptr;
}

cell_ptr builtin_fn_memory_is_set(cell_processor_if &ci,
                                        cell_list_t &list, env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::MEM_IS_SET, ==, 2)
  auto ptr = ci.process_cell(list[1], env);
  if (ptr->type != cell_type_e::PTR) {
    throw interpreter_c::exception_c("Cell does not contain a pointer", list[1]->locator);
  }
  return allocate_cell((int64_t)(ptr->data.ptr != nullptr)); 
}

cell_ptr builtin_fn_memory_cpy(cell_processor_if &ci,
                                        cell_list_t &list, env_c &env){
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::MEM_CPY, ==, 3)
  
  // If first param is not a ptr, we are storing cell data into memory
  //  all trivial types, and strings are allowed. no other complex data permitter

  // If the second param is a ptr, we are copying data in memory to
  // the ot the destination pointer

  // If the destination pointer is set & is NOT owned, fail out. its not ours 

  // If the second param is not a ptr we fail out

  return allocate_cell(cell_type_e::NIL);
}

cell_ptr builtin_fn_memory_load(cell_processor_if &ci,
                                        cell_list_t &list, env_c &env){
//  NIBI_LIST_ENFORCE_SIZE(nibi::kw::MEM_LOAD, ==, 3)

  // Pointer doesn't need to be owned.

  // Need tags like :i8 :i64 ... to indicate what the type of the cell should be

      // This would be a good time to move to a global map where we can
      // give the symbol of the tag and retrieve the cell_type_e as
      // we will need them elsewhere

  // If its a simple data type, move to the data union and set the type

  // If its a string, pull into a new string and create the cell

  // Other complex cell types should not be permitted yet

  return allocate_cell(cell_type_e::NIL);
}

}
}
