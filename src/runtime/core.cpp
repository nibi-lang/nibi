#include "core.hpp"

namespace runtime {

object_ptr core_c::execute(const data_s& data, env_c& env) {
  if (!data.data || data.size == 0) {
    return allocate_object(
      object_error_c(
        "core_c::execute",
        "Invalid data given for execution"));
  }

  atom_view::walker_c walker(
    data.data, data.size);

  object_ptr result = allocate_object(wrap_int_s{0});
  while (walker.has_next()) {
    result = evaluate(data, walker.next(), env);
    if (result->is_err()) {
      
      return result;
    }
  }
  return result;
}

object_ptr core_c::evaluate(
    const data_s& list, atom_view::view_s *atom, env_c &env) {
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
      return o;
    }
    case (int)atom_type_e::LIST:
      return execute(
        {atom->data.len_encoded.data,
         atom->data.len_encoded.len},
        env);
  };

  /*

        Note: on `fn` we need to clone the section 
        of bytes and store them in an object for
        later execution when called.. 

  */
  return nullptr;
}

} // namespace
