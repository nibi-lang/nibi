#include "object.hpp"
#include "env.hpp"
#include <fmt/format.h>

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
  }
  return "unknown";
}

std::string object_meta_data_s::to_string() const {
  std::string result;
  result += "bytecode origin index: ";
  result += std::to_string(bytecode_origin_id);
  return result;
}

std::string object_byte_data_s::to_string() const {
 std::string result = "[ ";
 for(size_t i = 0; i < len; i++) {
   result += std::to_string(static_cast<int>(data[i]));
   result += " ";
 }
 result += "]";
 return result;
}

std::string object_error_data_s::to_string() const {
  std::string result = "ERROR\n";
  result += message;
  result += "\n";
  result += operation;
  result += "\n";
  return result;
}

std::string object_c::to_string() const {
  switch(type) {
    case data_type_e::NONE: return "none";
    case data_type_e::BOOLEAN: return std::to_string(data.boolean);
    case data_type_e::INTEGER: return std::to_string(data.integer);
    case data_type_e::REAL: return std::to_string(data.real);
    case data_type_e::REF: return std::to_string(data.memory_ref);
    case data_type_e::ERROR: {
      if (data.err == nullptr) return "";
      if (data.err == nullptr) return "";
      return data.err->to_string();
    }
    case data_type_e::IDENTIFIER:
      [[fallthrough]];
    case data_type_e::STRING: {
      if (data.str == nullptr) return "";
      if (data.str->data == nullptr) return "";
      return std::string((char*)data.str->data, data.str->len);
    }
    case data_type_e::BYTES: {
      if (data.bytes == nullptr) return "[ ]";
      if (data.bytes->data == nullptr) return "[ ]";
      return data.bytes->to_string();
    }
  }
  return "unknown";
}

std::string object_c::dump_to_string(bool simple) const {

  if (simple) {
    return fmt::format("object<{}>[{}]", data_type_to_string(type), to_string());
  }

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
