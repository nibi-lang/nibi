#include "libnibi/cell.hpp"
#include "libnibi/environment.hpp"
#include "libnibi/keywords.hpp"
#include "libnibi/platform.hpp"
#include "macros.hpp"

#include <dlfcn.h>
#include <ffi.h>

#include <filesystem>
#include <unordered_map>

namespace {

constexpr std::size_t NIBI_FFI_ARG_MAX = 20;

// Command tag to type information so we can strictly enforce
// c types that users can use while mapping c types to nibi types
std::unordered_map<nibi::cell_type_e, ffi_type *> cell_type_to_ffi = {
    {nibi::cell_type_e::CHAR, &ffi_type_uchar},
    {nibi::cell_type_e::I8, &ffi_type_sint8},
    {nibi::cell_type_e::I16, &ffi_type_sint16},
    {nibi::cell_type_e::I32, &ffi_type_sint32},
    {nibi::cell_type_e::I64, &ffi_type_sint64},
    {nibi::cell_type_e::U8, &ffi_type_uint8},
    {nibi::cell_type_e::U16, &ffi_type_uint16},
    {nibi::cell_type_e::U32, &ffi_type_uint32},
    {nibi::cell_type_e::U64, &ffi_type_uint64},
    {nibi::cell_type_e::F32, &ffi_type_float},
    {nibi::cell_type_e::F64, &ffi_type_double},
    {nibi::cell_type_e::NIL, &ffi_type_void},
    {nibi::cell_type_e::PTR, &ffi_type_pointer},
    {nibi::cell_type_e::STRING, &ffi_type_pointer}};
} // namespace

namespace nibi {
namespace builtins {

cell_ptr builtin_fn_extern_call(cell_processor_if &ci, cell_list_t &list,
                                env_c &env) {

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::EXTERN_CALL, >=, 5);

  // Break apart the list and get all the information to make the call

  std::optional<std::string> lib_name = std::nullopt;
  if (list[1]->type != cell_type_e::NIL) {
    lib_name = ci.process_cell(list[1], env)->as_string();
  }

  const std::string fn_name = list[2]->as_string();
  auto &arg_types = list[3]->as_list();

  // Figure out the decalred return type
  // and make sure it is supported

  cell_type_e return_type = cell_type_e::NIL;

  {
    auto return_type_tag = list[4]->as_symbol();
    if (cell_trivial_type_tag_map.find(return_type_tag) ==
        cell_trivial_type_tag_map.end()) {
      std::string err =
          "extern-call: unsupported return type: " + return_type_tag;
      throw interpreter_c::exception_c(err, list[4]->locator);
    }

    return_type = cell_trivial_type_tag_map[return_type_tag];

    if (cell_type_to_ffi.find(return_type) == cell_type_to_ffi.end()) {
      std::string err =
          "extern-call: unsupported return type: " + return_type_tag;
      throw interpreter_c::exception_c(err, list[4]->locator);
    }
  }

  // Make sure the number of arguments given matches the number of
  // arguments declared

  if (list.size() != 5 + arg_types.size()) {
    std::string err = "extern-call: " + std::to_string(arg_types.size()) +
                      " arguments declared, but supplied" +
                      std::to_string(list.size() - 5);
    throw interpreter_c::exception_c(err, list[0]->locator);
  }

  // Ensure all arg types are supported, and populate a list
  // of their nibi type equivalents

  std::vector<cell_type_e> arg_cell_types;
  for (auto &arg_type : arg_types) {
    auto it = cell_trivial_type_tag_map.find(arg_type->as_symbol());
    if (it == cell_trivial_type_tag_map.end()) {
      std::string err =
          "extern-call: unsupported type: " + arg_type->as_symbol();
      throw interpreter_c::exception_c(err, arg_type->locator);
    }

    auto it2 = cell_type_to_ffi.find(it->second);
    if (it2 == cell_type_to_ffi.end()) {
      std::string err =
          "extern-call: unsupported type: " + arg_type->as_symbol();
      throw interpreter_c::exception_c(err, arg_type->locator);
    }
    arg_cell_types.push_back(it->second);
  }

  // Ensure they don't have too many arguments

  if (arg_cell_types.size() > NIBI_FFI_ARG_MAX) {
    std::string err = "extern-call: max ffi arguments exceeded: " +
                      std::to_string(arg_cell_types.size()) + " > " +
                      std::to_string(NIBI_FFI_ARG_MAX);
    throw interpreter_c::exception_c(err, list[0]->locator);
  }

  // Take arguments and process them down to their values

  std::vector<cell_ptr> args_supplied(list.begin() + 5, list.end());
  for (auto &arg : args_supplied) {
    arg = ci.process_cell(arg, env);
  }

  // Check that the types of the arguments match the types declared
  // in the function signature

  for (std::size_t i = 0; i < arg_cell_types.size(); i++) {
    if (arg_cell_types[i] != args_supplied[i]->type) {
      std::string err =
          "extern-call: argument " + std::to_string(i) + " is of type " +
          cell_type_to_string(args_supplied[i]->type) + " but should be type " +
          cell_type_to_string(arg_cell_types[i]);
      throw interpreter_c::exception_c(err, list[0]->locator);
    }
  }

  // Open the library and get the function pointer

  void *lib_handle{nullptr};
  if (lib_name.has_value()) {
    lib_handle = dlopen((*lib_name).c_str(), RTLD_LAZY);

    if (!lib_handle) {
      // Attempt to locate the library
      auto current_path = std::filesystem::current_path();
      auto target = std::filesystem::path(*lib_name);
      auto result = global_platform->locate_file(target, current_path);
      if (result.has_value()) {
        lib_handle = dlopen((*result).c_str(), RTLD_LAZY);
      }
    }
  } else {
    lib_handle = dlopen(nullptr, RTLD_LAZY);
  }

  if (!lib_handle) {
    std::string err = "extern-call: could not open library " +
                      (lib_name.has_value() ? *lib_name : "") + " " +
                      dlerror() + ")";
    throw interpreter_c::exception_c(err, list[0]->locator);
  }

  void *fn_ptr = dlsym(lib_handle, fn_name.c_str());

  char *error;
  if ((error = dlerror()) != nullptr) {
    std::string err =
        "extern-call: could not get handle to function: " + fn_name + " " +
        error;
    throw interpreter_c::exception_c(err, list[0]->locator);
  }

  // Setup the ffi call

  ffi_cif cif;
  ffi_type *ffi_ret_type = cell_type_to_ffi[return_type];
  ffi_type *ffi_arg_types[NIBI_FFI_ARG_MAX];
  void *ffi_arg_vals[NIBI_FFI_ARG_MAX];
  ffi_status status;

  // Load the arguments

  for (std::size_t i = 0; i < arg_cell_types.size(); i++) {
    ffi_arg_types[i] = cell_type_to_ffi[arg_cell_types[i]];
    ffi_arg_vals[i] = &args_supplied[i]->data;
  }

  // Prep the cif

  status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, arg_cell_types.size(),
                        ffi_ret_type, ffi_arg_types);

  // If the call failed, close the lib and throw an exception
  // with the error code

  if (status != FFI_OK) {

    dlclose(lib_handle);

    std::string err = "'ffi' call to ffi_prep_cif failed with code: " +
                      std::to_string(status);
    throw interpreter_c::exception_c(err, list[0]->locator);
  }

  // Perform the call (data will be placed directly into cell->data union

  cell_ptr result_cell = allocate_cell(return_type);

  ffi_call(&cif, FFI_FN(fn_ptr), &result_cell->data, ffi_arg_vals);

  // Close the lib

  dlclose(lib_handle);

  return result_cell;
}

} // namespace builtins
} // namespace nibi
