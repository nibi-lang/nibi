/*



    Right now this is a very basic ffi implementation that
    only supports calling functions that utilize basic
    c types [ int, double, float, char* ].

    The below is a list of ideas for expanding this to
    support more complicated c types and structs.


    Ideas for expanding these externals:

        Need a means to create, or at least, represent more complicated
        C stuff (struct, memory mgmt, etc).

        Option A:
        Consider using a dictionary to represent a c-struct, adding
        some stuff to command_tag_to_type to support this. May need
        to add changes to setting up cfi to support this.

        We could use abberant types to represent pointers to c stuff.
        Maybe embedding an object that wraps a uniond set of data
        and values that the ffi return can point to, along with a
        death callback in the inner object to free the c-allocated
        memory.

        We may need to embed that abberant into a dict so we can
        "bind a method" to the abberant so we can do a "from c type"
        and other methods.

        -----------------------------

        Obtion B:
        Another option would be to add a "c cell" type that could
        be interoperable with all other cell types. We could
        make a c_api.h to export cell processor and then
        make a c-list and c-cell to represent the cell_c in
        a c-friendly way. We then could have the c-list and
        whatever available for external libs to use.

        This means we could replace the whole module system
        with an ffi-only system.. we could use an abberant type
        to make an area for all created c cells to live in.

        This level of c-interoperability would be a lot of work
        but would be very worth it, especially if we ever
        want to actually target c as a backend.

        Plan:
        These c-cells could be used to store all c-type data.
        We could do this in multiple stages, where we start off
        getting basic types working. Then we could upgrade
        to structs-from-dicts, then we could do that c-list
        and c-api stuff.



*/

#include "libnibi/cell.hpp"
#include "libnibi/environment.hpp"
#include "libnibi/keywords.hpp"
#include "macros.hpp"

#include <dlfcn.h>
#include <ffi.h>
#include <iostream>
#include <unordered_map>

namespace {

// Map c types to nibi and ffi types
struct c_types_s {
  const char *type_name;
  nibi::cell_type_e cell_type;
  ffi_type *actual_ffi_type;
};

// This is a wrapper around the data that is passed to the ffi call
// so we can keep track of the original data and the converted data
// - This is also leveraged to store return data in the union
struct c_data {
  union {
    int64_t i;
    double d;
    float f;
    char *s;
  } data;
  nibi::cell_ptr original_data;
};

// Command tag to type information so we can strictly enforce
// c types that users can use while mapping c types to nibi types
std::unordered_map<std::string, c_types_s> command_tag_to_type = {
    {":int", {"int", nibi::cell_type_e::I64, &ffi_type_sint64}},
    {":void", {"void", nibi::cell_type_e::NIL, &ffi_type_void}},
    {":double", {"double", nibi::cell_type_e::F64, &ffi_type_double}},
    {":float", {"float", nibi::cell_type_e::F32, &ffi_type_float}},
    {":str", {"char*", nibi::cell_type_e::STRING, &ffi_type_pointer}}};

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
  const std::string return_type = list[4]->as_symbol();

  if (list.size() != 5 + arg_types.size()) {
    std::string err = "extern-call: " + std::to_string(arg_types.size()) +
                      " arguments declared, but supplied" +
                      std::to_string(list.size() - 5);
    throw interpreter_c::exception_c(err, list[0]->locator);
  }

  // Convert the tag type to the command type information
  // so we can map types between c and nibi

  std::vector<c_types_s> arg_info;
  for (auto &arg_type : arg_types) {

    auto it = command_tag_to_type.find(arg_type->as_symbol());

    if (it == command_tag_to_type.end()) {
      std::string err =
          "extern-call: unsupported type: " + arg_type->as_symbol();
      throw interpreter_c::exception_c(err, arg_type->locator);
    }
    arg_info.push_back(it->second);
  }

  // Build a list of arguments to pass to the function
  // with the given c types from nibi types

  std::vector<c_data> args_supplied;
  for (int i = 5; i < list.size(); i++) {

    c_data cd;
    cd.original_data = ci.process_cell(list[i], env);
    switch (cd.original_data->type) {
    case cell_type_e::I8:
    case cell_type_e::I16:
    case cell_type_e::I32:
    case cell_type_e::I64:
    case cell_type_e::U8:
    case cell_type_e::U16:
    case cell_type_e::U32:
    case cell_type_e::U64:
      cd.data.i = cd.original_data->as_integer();
      break;
    case cell_type_e::F32:
    case cell_type_e::F64:
      cd.data.d = cd.original_data->as_double();
      break;
    case cell_type_e::STRING:
      cd.data.s = cd.original_data->as_string().data();
      break;
    case cell_type_e::NIL:
      cd.data.i = 0;
      break;
    default:
      throw interpreter_c::exception_c("extern-call: unsupported type",
                                       list[i]->locator);
    }

    args_supplied.push_back(cd);
  }

  // Check that the types of the arguments match the types

  for (std::size_t i = 0; i < arg_info.size(); i++) {
    if (arg_info[i].cell_type != args_supplied[i].original_data->type) {
      std::string err =
          "extern-call: argument " + std::to_string(i) + " is of type " +
          cell_type_to_string(args_supplied[i].original_data->type) +
          " but should be type " + cell_type_to_string(arg_info[i].cell_type);
      throw interpreter_c::exception_c(err,
                                       args_supplied[i].original_data->locator);
    }
  }

  // Open the library and get the function pointer

  void *lib_handle{nullptr};
  if (lib_name.has_value()) {
    lib_handle = dlopen((*lib_name).c_str(), RTLD_LAZY);
  } else {
    lib_handle = dlopen(nullptr, RTLD_LAZY);
  }

  if (!lib_handle) {
    std::string err = "extern-call: could not open library";
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

  // Convert the return data type to the ffi type

  auto return_data_info = command_tag_to_type[return_type];

  // Setup the ffi call

  ffi_cif cif;
  ffi_type *ffi_ret_type = return_data_info.actual_ffi_type;
  ffi_type *ffi_arg_types[20];
  void *ffi_arg_vals[20];
  ffi_status status;

  // Load the arguments

  for (std::size_t i = 0; i < arg_info.size(); i++) {
    ffi_arg_types[i] = arg_info[i].actual_ffi_type;
    ffi_arg_vals[i] = &args_supplied[i].data;
  }

  // Prep the cif

  status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, arg_info.size(), ffi_ret_type,
                        ffi_arg_types);

  if (status != FFI_OK) {
    std::string err = "'ffi' call to ffi_prep_cif failed with code: " +
                      std::to_string(status);
    throw interpreter_c::exception_c(err, list[0]->locator);
  }

  // Make the actual call, and dump data into a c_data struct

  c_data return_data;

  ffi_call(&cif, FFI_FN(fn_ptr), &return_data.data, ffi_arg_vals);

  // Close the lib

  dlclose(lib_handle);

  // Using the return_data_info figure out how we should interpret
  // the data to allocate and return a cell
  switch (return_data_info.cell_type) {
  case cell_type_e::NIL:
    return allocate_cell(cell_type_e::NIL);
  case cell_type_e::I8:
  case cell_type_e::I16:
  case cell_type_e::I32:
  case cell_type_e::I64:
  case cell_type_e::U8:
  case cell_type_e::U16:
  case cell_type_e::U32:
  case cell_type_e::U64:
    return allocate_cell((int64_t)return_data.data.i);
  case cell_type_e::F32:
  case cell_type_e::F64:
    return allocate_cell((double)return_data.data.d);
  case cell_type_e::STRING:
    return allocate_cell(std::string(return_data.data.s));
  default:
    throw interpreter_c::exception_c("extern-call: unsupported return type",
                                     list[0]->locator);
  }
}

} // namespace builtins
} // namespace nibi
