#include "interpreter/builtins/builtins.hpp"
#include "libnibi/cell.hpp"
#include "libnibi/keywords.hpp"
#include "libnibi/type_names.hpp"
#include "macros.hpp"
#include <cstring>

#include <iostream>

namespace nibi {

namespace builtins {

cell_ptr builtin_fn_memory_new(cell_processor_if &ci, cell_list_t &list,
                               env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::MEM_NEW, ==, 2)

  auto ptr_cell = allocate_cell(cell_type_e::PTR);
  if (list[1]->type == cell_type_e::NIL) {
    ptr_cell->complex_data = pointer_info_s{true, 0};
    return ptr_cell;
  }

  auto size = ci.process_cell(list[1], env)->as_integer();

  ptr_cell->data.ptr = malloc(size);
  ptr_cell->complex_data = pointer_info_s{true, size};
  return ptr_cell;
}

cell_ptr builtin_fn_memory_del(cell_processor_if &ci, cell_list_t &list,
                               env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::MEM_DEL, >=, 2)

  auto ptr = allocate_cell(cell_type_e::PTR);
  for (size_t i = 1; i < list.size(); i++) {
    ptr = ci.process_cell(list[i], env);
    auto &ptr_info = ptr->as_pointer_info();

    if (ptr->data.ptr != nullptr) {
      free(ptr->data.ptr);
    }

    ptr->data.ptr = nullptr;
    ptr_info.size_bytes = std::nullopt;
  }

  // Return the last cell processed
  return ptr;
}

cell_ptr builtin_fn_memory_is_set(cell_processor_if &ci, cell_list_t &list,
                                  env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::MEM_IS_SET, ==, 2)
  auto ptr = ci.process_cell(list[1], env);
  if (ptr->type != cell_type_e::PTR) {
    throw interpreter_c::exception_c("Cell does not contain a pointer",
                                     list[1]->locator);
  }
  return allocate_cell((int64_t)(ptr->data.ptr != nullptr));
}

cell_ptr copy_cell_into_memory(cell_processor_if &ci, cell_ptr &source,
                               cell_ptr &dest) {

  if (static_cast<uint8_t>(source->type) > CELL_TYPE_MAX_TRIVIAL) {
    throw interpreter_c::exception_c(
        "Can not copy non-trivial cell into memory", source->locator);
  }

  if (source->type == cell_type_e::NIL) {
    throw interpreter_c::exception_c("Can not copy NIL cell into memory",
                                     source->locator);
  }

  // We know that the dest is owned and a pointer as a prerequisite to this
  // function.
  if (dest->data.ptr != nullptr) {
    free(dest->data.ptr);
  }

  switch (source->type) {
  case cell_type_e::U8:
    dest->data.ptr = malloc(sizeof(uint8_t));
    *reinterpret_cast<uint8_t *>(dest->data.ptr) =
        (uint8_t)source->as_integer();
    break;
  case cell_type_e::U16:
    dest->data.ptr = malloc(sizeof(uint16_t));
    *reinterpret_cast<uint16_t *>(dest->data.ptr) =
        (uint16_t)source->as_integer();
    break;
  case cell_type_e::U32:
    dest->data.ptr = malloc(sizeof(uint32_t));
    *reinterpret_cast<uint32_t *>(dest->data.ptr) =
        (uint32_t)source->as_integer();
    break;
  case cell_type_e::U64:
    dest->data.ptr = malloc(sizeof(uint64_t));
    *reinterpret_cast<uint64_t *>(dest->data.ptr) =
        (uint64_t)source->as_integer();
    break;
  case cell_type_e::I8:
    dest->data.ptr = malloc(sizeof(int8_t));
    *reinterpret_cast<int8_t *>(dest->data.ptr) = (int8_t)source->as_integer();
    break;
  case cell_type_e::I16:
    dest->data.ptr = malloc(sizeof(int16_t));
    *reinterpret_cast<int16_t *>(dest->data.ptr) =
        (int16_t)source->as_integer();
    break;
  case cell_type_e::I32:
    dest->data.ptr = malloc(sizeof(int32_t));
    *reinterpret_cast<int32_t *>(dest->data.ptr) =
        (int32_t)source->as_integer();
    break;
  case cell_type_e::I64:
    dest->data.ptr = malloc(sizeof(int64_t));
    *reinterpret_cast<int64_t *>(dest->data.ptr) =
        (int64_t)source->as_integer();
    break;
  case cell_type_e::F32:
    dest->data.ptr = malloc(sizeof(float));
    *reinterpret_cast<float *>(dest->data.ptr) = (float)source->as_double();
    break;
  case cell_type_e::F64:
    dest->data.ptr = malloc(sizeof(double));
    *reinterpret_cast<double *>(dest->data.ptr) = (double)source->as_double();
    break;
  case cell_type_e::STRING: {
    std::string data = source->as_string();
    dest->data.ptr = malloc(data.size() + 1);
    memcpy(dest->data.ptr, data.c_str(), data.size() + 1);
    break;
  }
  default:
    throw interpreter_c::exception_c(
        "Attempt to copy unhandled non-trivial cell into memory",
        source->locator);
    break;
  }
  return dest;
}

cell_ptr copy_memory(cell_processor_if &ci, cell_ptr &source, cell_ptr &dest) {

  auto source_ptr_info = source->as_pointer_info();
  auto dest_ptr_info = dest->as_pointer_info();

  if (!source_ptr_info.size_bytes.has_value()) {
    throw interpreter_c::exception_c(
        "Can not copy pointer, source cell does not have a known size",
        source->locator);
  }

  if (dest->data.ptr != nullptr) {
    free(dest->data.ptr);
  }

  dest->data.ptr = malloc(source_ptr_info.size_bytes.value());
  memcpy(dest->data.ptr, source->data.ptr, source_ptr_info.size_bytes.value());

  dest_ptr_info.size_bytes = source_ptr_info.size_bytes;
  return dest;
}

cell_ptr builtin_fn_memory_cpy(cell_processor_if &ci, cell_list_t &list,
                               env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::MEM_CPY, ==, 3)

  auto dest = ci.process_cell(list[2], env);

  if (dest->type != cell_type_e::PTR) {
    throw interpreter_c::exception_c("Destination cell must be a pointer",
                                     list[2]->locator);
  }

  auto source = ci.process_cell(list[1], env);

  if (source->type != cell_type_e::PTR) {
    return copy_cell_into_memory(ci, source, dest);
  }

  return copy_memory(ci, source, dest);
}

cell_ptr builtin_fn_memory_load(cell_processor_if &ci, cell_list_t &list,
                                env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::MEM_LOAD, ==, 3)

  auto suspected_tag = list[1]->as_symbol();

  auto tcm = cell_trivial_type_tag_map.find(suspected_tag);
  if (tcm == cell_trivial_type_tag_map.end()) {
    throw interpreter_c::exception_c(
        "Expected parameter to be a trivial type tag", list[1]->locator);
  }

  auto dest = ci.process_cell(list[2], env);

  auto &dest_ptr_info = dest->as_pointer_info();

  if (dest->data.ptr == nullptr) {
    throw interpreter_c::exception_c("Attempt to load from unallocated pointer",
                                     list[2]->locator);
  }

  auto new_cell = allocate_cell(tcm->second);

  switch (new_cell->type) {
  case cell_type_e::U8:
    new_cell->data.u8 = *reinterpret_cast<uint8_t *>(dest->data.ptr);
    break;
  case cell_type_e::U16:
    new_cell->data.u16 = *reinterpret_cast<uint16_t *>(dest->data.ptr);
    break;
  case cell_type_e::U32:
    new_cell->data.u32 = *reinterpret_cast<uint32_t *>(dest->data.ptr);
    break;
  case cell_type_e::U64:
    new_cell->data.u64 = *reinterpret_cast<uint64_t *>(dest->data.ptr);
    break;
  case cell_type_e::I8:
    new_cell->data.i8 = *reinterpret_cast<int8_t *>(dest->data.ptr);
    break;
  case cell_type_e::I16:
    new_cell->data.i16 = *reinterpret_cast<int16_t *>(dest->data.ptr);
    break;
  case cell_type_e::I32:
    new_cell->data.i32 = *reinterpret_cast<int32_t *>(dest->data.ptr);
    break;
  case cell_type_e::I64:
    new_cell->data.i64 = *reinterpret_cast<int64_t *>(dest->data.ptr);
    break;
  case cell_type_e::F32:
    new_cell->data.f32 = *reinterpret_cast<float *>(dest->data.ptr);
    break;
  case cell_type_e::F64:
    new_cell->data.f64 = *reinterpret_cast<double *>(dest->data.ptr);
    break;
  case cell_type_e::STRING: {
    auto str = reinterpret_cast<char *>(dest->data.ptr);
    new_cell->update_string(std::string(str, strlen(str)));
    break;
  }
  default:
    throw interpreter_c::exception_c(
        "Attempted to load unhandled type directly from memory",
        list[1]->locator);
    break;
  }

  return new_cell;
}

} // namespace builtins
} // namespace nibi
