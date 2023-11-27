#include "object.hpp"
#include "env.hpp"

namespace runtime {

const char* data_type_to_string(const data_type_e& type) {
  switch(type) {
    case data_type_e::NONE: return "none";
    case data_type_e::BOOLEAN: return "boolean";
    case data_type_e::INTEGER: return "integer";
    case data_type_e::REAL: return "real";
    case data_type_e::REF: return "ref";
    case data_type_e::STRING: return "string";
    case data_type_e::BYTES: return "bytes";
    case data_type_e::ERROR: return "error";
    case data_type_e::IDENTIFIER: return "identifier";
    case data_type_e::CPPFN: return "cppfn";
    case data_type_e::LIST: return "list";
  }
  return "unknown";
}

std::string object_c::to_string(bool quotes) const {
  switch(type) {
    case data_type_e::NONE: return "none";
    case data_type_e::CPPFN: return "<cppfn>";
    case data_type_e::BOOLEAN: return std::to_string(data.boolean);
    case data_type_e::INTEGER: return std::to_string(data.integer);
    case data_type_e::REAL: return std::to_string(data.real);
    case data_type_e::REF: return std::to_string(data.memory_ref);
    case data_type_e::ERROR:
      return reinterpret_cast<object_error_c*>(data.co)->to_string();
    case data_type_e::LIST:
      return reinterpret_cast<object_list_c*>(data.co)->to_string();
    case data_type_e::IDENTIFIER:
      [[fallthrough]];
    case data_type_e::STRING: {
      auto* o = reinterpret_cast<object_bytes_c*>(data.co);
      std::string str((char*)o->data, o->len);
      if (quotes) {
        return fmt::format("\"{}\"", str);
      }
      return str;
    }
    case data_type_e::BYTES:
      return reinterpret_cast<object_bytes_c*>(data.co)->to_string();
  }
  return "unknown";
}

std::string object_c::dump_to_string(bool simple) const {

  if (simple) {
    return fmt::format("object<{}>[{}]", data_type_to_string(type), to_string());
  }

  return fmt::format(R"(
  object : {}
  data   : {}
  )", data_type_to_string(type), to_string());
}

[[nodiscard]] bool object_c::conditional_self_load(runtime::env_c *env, bool limit_scope) {
  if (!env) { return false; }
  if (type != data_type_e::IDENTIFIER) {
    return true;
  }

  auto target = env->get(this->to_string(), limit_scope);
  if (target == nullptr) {
    return false;
  }

  update_from(*target.get());

  return true;
}

complex_object_c* object_list_c::clone() {
  object_list_t nl;
  nl.reserve(list.size());


  // TODO: May want to copy this better 
  for(auto &o : list) {
    nl.push_back(allocate_object(o->clone()));
  }
  return new object_list_c(nl);
}

std::string object_list_c::to_string() {
  std::string result = "[";
  for(auto &o : list) {
    result += fmt::format(" {}", o->to_string());
  }
  result += "]";
  return result;
}

} // namespace
