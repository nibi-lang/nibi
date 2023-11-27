#include "object.hpp"
#include "env.hpp"
#include "util.hpp"

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
    case data_type_e::VEC: return "vector";
    case data_type_e::SET: return "set";
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
    case data_type_e::SET:
      return reinterpret_cast<object_map_c*>(data.co)->to_string();
    case data_type_e::LIST:
    case data_type_e::VEC:
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
  result += " ]";
  return result;
}

complex_object_c* object_map_c::clone() {
  std::map<std::size_t, object_ptr> ns;
  for(auto& o : data) {
    ns[o.first] = allocate_object(o.second->clone());
  }
  return new object_map_c(ns);
}

std::string object_map_c::to_string() {
  std::string result = "{";
  for(auto &o : data) {
    result += fmt::format(" {}", o.second->to_string());
  }
  result += " }";
  return result;
}

std::size_t object_bytes_c::hash() const {
  std::size_t seed = len;
  for(std::size_t i = 0; i < len; i++) {
    std::size_t x = data[i];
    UTIL_PERFORM_HASH(x,seed)
  }
  return seed;
}

std::size_t object_cpp_fn_c::hash() const {
  return std::hash<long>{}(*(long *)(char *)(&fn));
}

std::size_t object_list_c::hash() const {
  std::size_t seed = list.size();
  for(const auto &obj : list) {
    std::size_t x = obj->hash();
    UTIL_PERFORM_HASH(x,seed)
  }
  return seed;
}

std::size_t object_error_c::hash() const {
  std::size_t seed = 4;
  for(auto i : {op, message, std::to_string(pos.line), std::to_string(pos.col)}){
    std::size_t x = std::hash<std::string>{}(i);
    UTIL_PERFORM_HASH(x,seed)
  }
  return seed;
}

std::size_t object_map_c::hash() const {
  std::size_t seed = data.size();
  std::map<std::size_t, object_ptr>::const_iterator it;
  for(it = data.begin(); it != data.end(); ++it) {
    std::size_t x = (*it).second->hash();
    UTIL_PERFORM_HASH(x,seed)
  }
  return seed;
}




} // namespace
