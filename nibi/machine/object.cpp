#include "object.hpp"
#include "env.hpp"

namespace machine {

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
  }
  return "unknown";
}

std::string object_meta_data_s::to_string() const {
  return fmt::format("Origin index: {}", bytecode_origin_id);
}

std::string object_c::to_string() const {
  switch(type) {
    case data_type_e::NONE: return "none";
    case data_type_e::CPPFN: return "<cppfn>";
    case data_type_e::BOOLEAN: return std::to_string(data.boolean);
    case data_type_e::INTEGER: return std::to_string(data.integer);
    case data_type_e::REAL: return std::to_string(data.real);
    case data_type_e::REF: return std::to_string(data.memory_ref);
    case data_type_e::ERROR:
      return reinterpret_cast<object_error_c*>(data.co)->to_string();
    case data_type_e::IDENTIFIER:
      [[fallthrough]];
    case data_type_e::STRING: {
      auto* o = reinterpret_cast<object_bytes_c*>(data.co);
      return std::string((char*)o->data, o->len);
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

  // TODO: update this to use fmt
  std::string result = "object:";
  result += data_type_to_string(type);
  result += "\nmeta:";
  if (!meta) { result += "<none>"; }
  else { result += meta->to_string(); }
  result += "\ndata:\n\t";
  result += to_string();
  return result;
}

[[nodiscard]] bool object_c::conditional_self_load(machine::env_c *env, bool limit_scope) {
  if (!env) { return false; }
  if (type != data_type_e::IDENTIFIER) {
    return true;
  }

  object_c* target = env->get(this->to_string(), limit_scope);
  if (target == nullptr) {
    return false;
  }

  update_from(*target);

  return true;
}



} // namespace
