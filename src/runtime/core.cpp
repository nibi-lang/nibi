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

object_ptr core_c::resolve(atom_view::view_s* atom, env_c &env) {
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
          wrap_int_s{atom->data.int_encoded.value});
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
    case (int)atom_type_e::DATA_LIST: {
      atom_view::walker_c walker(
        (uint8_t*)atom->data.len_encoded.data,
        (std::size_t)atom->data.len_encoded.len);
      object_list_t list;
      list.reserve(20);
      while(walker.has_next()) {
        list.push_back(resolve(walker.next(), env));
      }
      return allocate_object(list, true);
    }
    case (int)atom_type_e::SET: {
      atom_view::walker_c walker(
        (uint8_t*)atom->data.len_encoded.data,
        (std::size_t)atom->data.len_encoded.len);
      std::set<object_c> s;
      while(walker.has_next()) {
        s.insert(*resolve(walker.next(), env).get());
      }
      return allocate_object(s);
    }
    case (int)atom_type_e::LIST:
      return execute(
        atom->data.len_encoded.data,
        atom->data.len_encoded.len,
        env);
  };

  // Should be unreachable
  return allocate_object(
    object_error_c(
      _origin,
      "Unhandled atom when attempting to resolve to object",
      file_position_s{ atom->line, atom->col }));
}

object_ptr core_c::evaluate(atom_view::walker_c &walker, env_c &env) {

  if (_yield_value) return _yield_value;

  auto* atom = walker.next();

  if (!atom) {
    _yield_value = allocate_object(
      object_error_c(
        _origin,
        "Invalid atom evaluation (atom was nullptr)",
        file_position_s{ atom->line, atom->col }));
    return _yield_value;
  }

  object_ptr target = resolve(atom, env);

  if (target->type == data_type_e::CPPFN) {
    object_ptr result = target->as_cpp_fn()->fn(walker, *this, env); 

    // Check if external function added tracer info
    if (result->type == data_type_e::ERROR) {
      auto* e = result->as_error();
      if (!e->has_file_info()) {
        e->op = _origin;

        // External function may have flagged the walker 
        // with the specific instruction that caused the err
        if (walker.is_flagged()) {
          walker.go_to_mark();

          auto* x = walker.next();
          if (x) { atom = x; }
        }

        e->pos = file_position_s{ atom->line, atom->col };
      }
      _yield_value = result;
    }
    return result;
  }
  return target;
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
    case (int)atom_type_e::SET:
    case (int)atom_type_e::DATA_LIST:
    case (int)atom_type_e::LIST: {
      atom_view::walker_c walker(
        (uint8_t*)view->data.len_encoded.data,
        (std::size_t)view->data.len_encoded.len);
      object_list_t list;
      list.reserve(20);
      while(walker.has_next()) {
        list.push_back(object_from_view(walker.next()));
      }
      return allocate_object(list, (!(view->id == (int)atom_type_e::LIST)));
    }
  };
  
  return allocate_object(
    object_error_c(
      _origin,
      "Unhandled atom when converting object",
      file_position_s{ view->line, view->col }));
}

} // namespace
