#include "atomiser.hpp"

#include <map>
#include <regex>
#include <set>
#include <stack>
#include <string>
#include <filesystem>
#include <fstream>

class atomiser_c {
public:
  atomiser_c() = delete;
  atomiser_c(
    const std::string& origin,
    list_receiver_if &receiver);

  void submit(const char* data, size_t line=0);
  void submit(std::string &data, size_t line=0);
  bool finish();

private:
  std::string _origin;
  list_receiver_if &_receiver;

  struct {
    size_t line{0};
    size_t col{0};
  } _trace;

  void emit_error(
    const std::string& message,
    const file_position_s& pos) {
    _receiver.on_error(
      file_error_s{ _origin, message, pos});
  }

  void emit_error(
    const std::string& message) {
    emit_error(
      message, 
      file_position_s{_trace.line, _trace.col});
  }

  void parse(std::string &string_data);
  void reset();

  bool collect_comments(std::string& data, atom_list_t* list);
  bool collect_number(std::string& data, atom_list_t* list);
  bool collect_string(std::string& data, atom_list_t* list);
  bool collect_identifier(std::string& data, atom_list_t* list);
  bool collect_symbol(std::string& data, atom_list_t* list);

  atom_symbol_c::classification_e
    classify_type_tag(const std::string& sym);

  std::stack<atom_list_t> _active_lists;
  std::set<uint8_t> _termChars;
};

namespace {

static std::regex is_identifier(R"([_a-zA-Z][\-_a-zA-Z0-9]{0,30})");
static std::regex is_number(R"([+-]?([0-9]*[.])?[0-9]+)");

static std::regex is_type_tag(R"([:][a-zA-Z0-9][\-a-zA-Z0-9]+)");
static std::regex is_type_tag_vec(R"([:][a-zA-Z0-9][\-a-zA-Z0-9]+[\+])");

static std::regex is_macro_type_tag(R"((\$)[a-zA-Z0-9][\-a-zA-Z0-9]+)");
static std::regex is_macro_type_tag_vec(R"((\$)[a-zA-Z0-9][\-a-zA-Z0-9]+[\+])");
static std::regex is_macro_type_tag_cont(R"((\$)[a-zA-Z0-9][\-a-zA-Z0-9]+(\.\.))");
static std::regex is_macro_type_tag_vec_cont(R"((\$)[a-zA-Z0-9][\-a-zA-Z0-9]+[\+](\.\.))");

static std::map<char, char> char_escape_map = {
    {'n', '\n'},  {'t', '\t'},  {'r', '\r'}, {'a', '\a'},
    {'b', '\b'},  {'v', '\v'},  {'?', '\?'}, {'"', '\"'},
    {'\'', '\''}, {'\\', '\\'}, {'0', '\0'}};

inline bool check_for_chars(std::string &buffer, char c) {
  auto it = char_escape_map.find(c);
  if (it != char_escape_map.end()) {
    buffer += it->second;
    return true;
  }
  return false;
}

} // namespace

atomiser_c::atomiser_c(
  const std::string& origin,
  list_receiver_if &receiver)
    : _origin(origin),
      _receiver(receiver) {
  _termChars = {'{', '}', '[', ']', '(', ')'};
}

void atomiser_c::submit(const char* data, size_t line) {
  if (!data) {
    return;
  }
  std::string val = data;
  return submit(val, line);
}

void atomiser_c::submit(std::string &data, size_t line) {

  if (data.empty())
    return;

  _trace.line = line;
  _trace.col = 0;
  
  parse(data);
}

bool atomiser_c::finish() {
  if (!_active_lists.empty()) {
    emit_error(
      "Incomplete list - finish indicated with data buffered");
    return false;
  }
  return true;
}

void atomiser_c::parse(std::string &line_data) {

  //  Here we utilize a stack rather than recursion so we 
  //  can pause processing of a list in the middle in the event
  //  we are being spoon-fed data rather than being given an entire
  //  statement at once as its possible we have partially complete
  //  lists as we read formatted text

  while(_trace.col < line_data.size()) {

    atom_list_t* current_list = nullptr;

    if (_active_lists.size()) {
      current_list = &_active_lists.top();
    }
    auto current_char = line_data[_trace.col];

    if (std::isspace(current_char)) {
      _trace.col++;
      continue;
    }
    
    if (collect_comments(line_data, current_list)) { continue; }

    if (current_char == '(') {
      _active_lists.push({});
      _trace.col++;
      continue;
    }

    if (!current_list) {
      emit_error(
        fmt::format("No active list to handle item : {}", current_char));
      _trace.col = line_data.size();
      return;
    }

    if (current_char == ')') {
      if (_active_lists.empty()) {
        _trace.col = line_data.size();
        return;
      }

      _trace.col++;
      atom_list_t top_list = std::move(_active_lists.top());
      _active_lists.pop();

      if (_active_lists.empty()) {
        _receiver.on_list(std::move(top_list));
        continue;
      }

      _active_lists.top().push_back(
        std::make_unique<atom_list_c>(
          std::move(top_list),
          _active_lists.top()[0]->pos));
      continue;
    }

    if (collect_number(line_data, current_list)) { continue; }
    if (collect_string(line_data, current_list)) { continue; }
    if (collect_identifier(line_data, current_list)) { continue; }
    if (collect_symbol(line_data, current_list)) { continue; }

    emit_error(
      fmt::format("Unhandled token identified while parsing list: {}", current_char));
    std::exit(1);
  }
}

#define HAS_NEXT \
  (_trace.col + 1 < data.size())

bool atomiser_c::collect_comments(std::string& data, atom_list_t* list) {
  auto& current_char = data[_trace.col];
  if (current_char == ';' ||
      current_char == '#' && _trace.col == 0) {
    _trace.col = data.size();
    return true;
  }
  return false;
}

bool atomiser_c::collect_number(std::string& data, atom_list_t* list) {

  bool pos{true};
  {
    auto& current_char = data[_trace.col];
    if ((current_char == '+' || current_char == '-') && 
        HAS_NEXT && 
        std::isdigit(data[_trace.col +1])) {
      if (current_char == '-') pos = false;
      _trace.col++;
    }
  }

  auto& current_char = data[_trace.col];
  if (!std::isdigit(current_char)) {
    return false;
  }
   
  std::string num;
  bool consume{true};
  bool has_dec{false};

  auto start_col = _trace.col;
  do {
    if ('.' == data[_trace.col]) {
      if (has_dec) {
        emit_error(
          fmt::format("Too many decimals detected in numerical value: {}", num));
        _trace.col = data.size();
        return false;
      }

      has_dec = true;
      num += '.';
      _trace.col++;
      continue;
    }

    if (!std::isdigit(data[_trace.col])) {
      consume = false;
      continue;
    }

    num += data[_trace.col];
    _trace.col++;

  } while (consume);

  if (!(std::regex_match(num, is_number))) {
    emit_error(
      fmt::format("Malformed numerical value: {}", num));
    _trace.col = data.size();
    return false;
  }
  
  if (has_dec) {
    list->push_back(
      std::make_unique<atom_real_c>(
        std::stod(num), 
        file_position_s{_trace.line, start_col}));
    return true;
  }

  list->push_back(
    std::make_unique<atom_int_c>(
      std::stoll(num), 
      file_position_s{_trace.line, start_col}));
  return true;
}

bool atomiser_c::collect_string(std::string& data, atom_list_t* list) {

  if (data[_trace.col] != '\"') { return false; }

  auto start_col = _trace.col;
  std::string str;

  _trace.col++;

  bool consume{true};
  do {
    
    if (data[_trace.col] == '"') {
      _trace.col++;
      consume = false;
      continue;
    }

    if (!HAS_NEXT) {
      emit_error(
        fmt::format("Unterminated string!"));
      return false;
    }

    if (data[_trace.col] == '\\') {
      if (check_for_chars(str, data[_trace.col + 1])) {
        _trace.col+= 2;
        continue;
      }
    }

    str += data[_trace.col];
    _trace.col++;

  } while (consume);

  list->push_back(
    std::make_unique<atom_string_c>(
      str, 
      file_position_s{_trace.line, start_col}));
  return true;
}

bool atomiser_c::collect_identifier(std::string& data, atom_list_t* list) {

  auto start_col = _trace.col;
  std::string identifier;
  while(std::isalnum(data[_trace.col]) || 
        data[_trace.col] == '_' ||
        (data[_trace.col] == '-' && identifier.size())) {
    identifier += data[_trace.col];
    _trace.col++;
  }
  if (identifier.empty()) {
    return false;
  }

  if (!(std::regex_match(identifier, is_identifier))) {
    emit_error(
      fmt::format("Malformed potential identifier: '{}'",
        identifier));
    _trace.col = data.size();
    return false;
  }
  list->push_back(
    std::make_unique<atom_symbol_c>(
      identifier, 
      file_position_s{_trace.line, start_col}));
  return true;
}

bool atomiser_c::collect_symbol(std::string& data, atom_list_t* list) {
  auto start_col = _trace.col;
  std::string sym;
  while(!_termChars.contains(data[_trace.col]) &&
        !std::isspace(data[_trace.col])) {
    sym += data[_trace.col];
    _trace.col++;
  }
  if (sym.empty()) {
    return false;
  }

  // Attempt to classify an identified tag
  atom_symbol_c::classification_e tag{
    atom_symbol_c::classification_e::STANDARD};
  if (sym[0] == ':') {
    tag = classify_type_tag(sym);
    if (tag == atom_symbol_c::classification_e::STANDARD) {
      emit_error(
        fmt::format("Malformed suspected type tag '{}'",
          sym));
    }
  }

  list->push_back(
    std::make_unique<atom_symbol_c>(
      sym, 
      file_position_s{_trace.line, start_col},
      tag));
  return true;
}

atom_symbol_c::classification_e
atomiser_c::classify_type_tag(const std::string &sym) {
  if (std::regex_match(sym, is_type_tag)) {
    return atom_symbol_c::classification_e::TYPE_TAG; }
  if (std::regex_match(sym, is_type_tag_vec)) {
    return atom_symbol_c::classification_e::TYPE_TAG_VEC; }
  if (std::regex_match(sym, is_macro_type_tag)) {
    return atom_symbol_c::classification_e::MACRO_TAG; }
  if (std::regex_match(sym, is_macro_type_tag_vec)) {
    return atom_symbol_c::classification_e::MACRO_TAG_VEC; }
  if (std::regex_match(sym, is_macro_type_tag_cont)) {
    return atom_symbol_c::classification_e::MACRO_TAG_CONT; }
  if (std::regex_match(sym, is_macro_type_tag_vec_cont)) {
    return atom_symbol_c::classification_e::MACRO_TAG_VEC_CONT; }
  return atom_symbol_c::classification_e::STANDARD;
}

class receiver_c : public list_receiver_if {
public:
  void on_list(atom_list_t list) override {
    // print_atom_list(list);

    // TODO: Check here to see if the list is an import.
    //       If it is, we can import it here. 

    lists.push_back(
      std::move(list));
  }

  void on_error(file_error_s err) override {
    fmt::print("Got error! : {}\n", err.to_string());
    okay = false;
  }

  bool okay{true};
  std::vector<atom_list_t> lists;
};

void atomiser_c::reset() {
  _trace = {0,0};
  _active_lists = {};
}

std::optional<parse_group_s> atomise_file(const std::string& file) {

  {
    std::filesystem::path p(file);
    if (!std::filesystem::is_regular_file(p)) {
      fmt::print("Target '{}' is not a file\n", file);
      return std::nullopt;
    }
  }

  std::ifstream in(file);
  if (!in.is_open()) {
    fmt::print("Unable to open target '{}'\n", file);
    return std::nullopt;
  }

  receiver_c recv;
  atomiser_c atomiser(file, recv);

  std::string line;
  std::size_t line_number{1};
  while (recv.okay && std::getline(in, line)) {
    atomiser.submit(line, line_number++);
  }

  in.close();

  return { parse_group_s { file, std::move(recv.lists) }};
}

int repl() {

  fmt::print("REPL NOT YET COMPLETED");
  return 0;
}
