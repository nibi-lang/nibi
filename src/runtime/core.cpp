#include "core.hpp"

namespace runtime {

object_ptr core_c::execute(
    uint8_t* data, const std::size_t& size, env_c& env) {
  if (!data || size == 0) {
    return allocate_object(
      object_error_c(
        "core_c::execute",
        "Invalid data given for execution"));
  }

  atom_view::walker_c walker(
    data, size);

  object_ptr result = allocate_object(wrap_int_s{0});
  while (walker.has_next()) {
    result = evaluate(walker, env);
    if (result->is_err()) {
      return result;
    }
  }
  return result;
}

object_ptr core_c::evaluate_list(object_list_t& list, env_c &env) {
  

}

object_ptr core_c::evaluate(atom_view::walker_c &walker, env_c &env) {

  auto* atom = walker.next();

  if (!atom) return nullptr;

  switch (atom->id) {
    case (int)atom_type_e::STRING:
      return allocate_object(
          (char*)(atom->data.len_encoded.data), 
                  atom->data.len_encoded.len);
    case (int)atom_type_e::REAL:
      return allocate_object(
          wrap_real_s{atom->data.real_encoded.value});
    case (int)atom_type_e::INTEGER:
      return allocate_object(
          wrap_real_s{atom->data.int_encoded.value});
    case (int)atom_type_e::SYMBOL: {
      auto o = env.get(std::string(
          (char*)(atom->data.len_encoded.data), 
                  atom->data.len_encoded.len));
      if (!o.get()) {
        return allocate_object(
          object_error_c(
            _origin,
            fmt::format("Unable to locate symbol '{}'",
              std::string(
                (char*)(atom->data.len_encoded.data),
                atom->data.len_encoded.len)),
            file_position_s{ atom->line, atom->col }));
      }

      if (o->type == data_type_e::CPPFN) {
        return prep_and_call_cpp_fn(
          o->as_cpp_fn()->fn, walker, env);
      }
      return o;
    }
    case (int)atom_type_e::LIST:
      return execute(
        atom->data.len_encoded.data,
        atom->data.len_encoded.len,
        env);
  };

  /*

        Note: on `fn` we need to clone the section 
        of bytes and store them in an object for
        later execution when called.. 

  */
  return nullptr;
}

object_ptr core_c::object_from_view(atom_view::view_s* view) {
  switch (view->id) {
    case (int)atom_type_e::SYMBOL:
      return allocate_object(
        wrap_identifier_s{(char*)(view->data.len_encoded.data), view->data.len_encoded.len});
    case (int)atom_type_e::STRING:
      return allocate_object(
        (char*)(view->data.len_encoded.data), view->data.len_encoded.len);
    case (int)atom_type_e::REAL:
      return allocate_object(
        wrap_real_s{view->data.real_encoded.value});
    case (int)atom_type_e::INTEGER:
      return allocate_object(
        wrap_int_s{view->data.int_encoded.value});
    case (int)atom_type_e::LIST: {
      atom_view::walker_c walker(
        (uint8_t*)view->data.len_encoded.data,
        (std::size_t)view->data.len_encoded.len);
      object_list_t list;
      list.reserve(20);
      while(walker.has_next()) {
        list.push_back(object_from_view(walker.next()));
      }
      return allocate_object(list);
    }
  };
  
  return allocate_object(
    object_error_c(
      _origin,
      "Unhandled atom when converting object",
      file_position_s{ view->line, view->col }));
}

object_ptr core_c::prep_and_call_cpp_fn(
    cpp_fn &fn, atom_view::walker_c& walker, env_c &env) {

  object_list_t params;
  params.reserve(20);

  while(walker.has_next()) {
    params.push_back(object_from_view(walker.next()));
  }

 return fn(params, env); 
}

} // namespace
