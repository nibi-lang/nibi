#include "ndb.hpp"
#include "apps/linenoise.hpp"

#include <fmt/format.h>
#include <filesystem>
#include <fstream>
#include <algorithm>

#include <unordered_map>
namespace {
  constexpr std::size_t MAX_HISTORY = 4092;
  constexpr char PROMPT[] = "ndb> ";

inline std::vector<std::string> chunk(std::string line) 
{
    std::vector<std::string> chunks;
    bool inSubStr = false;
    std::string token = "";
    if (!line.empty() && line[line.size() - 1] == '\r'){
        line.erase(line.size() - 1);
    }
    for(unsigned i = 0; i < line.length(); i++) {
        if (line[i] == '"') {
            inSubStr ? inSubStr = false : inSubStr = true;
        }
        if ((line[i] == ' ' || line[i] == '\t' || line[i] == '\n') && !inSubStr){
            if (token.length() > 0) {
                chunks.push_back(token);
                token.clear();
            } 
        } else {
            token = token + line[i];
        }
    }
    if (token.length() > 0) {
        chunks.push_back(token);
    }
    return chunks;
}

inline std::string& rtrim(std::string &line) 
{
    line.erase(std::find_if(line.rbegin(), line.rend(), 
    [](int ch) 
    {
        return !std::isspace(ch);
    }).base(), line.end());
    return line;
}

inline std::string& ltrim (std::string &line)
{
    line.erase(line.begin(),find_if_not(line.begin(),line.end(),
    [](int c)
    {
        return isspace(c);
    }));
    return line;
}

} // namespace

namespace ndb {

ndb_c::ndb_c(
    std::vector<std::string> target_args,
    std::vector<std::string> target_stdin)
    : _runtime(target_args, target_stdin){

    _cmds = {
      { "help",   "Show help page",
                  std::bind(&ndb::ndb_c::cmd_help, this, std::placeholders::_1) },
      { "run",    "Run the target application",
                  std::bind(&ndb::ndb_c::cmd_run, this, std::placeholders::_1) },
      { "quit",   "Quit the debugger",
                  std::bind(&ndb::ndb_c::cmd_quit, this, std::placeholders::_1) }
      { "break",  "Set a break point at a line number",
                  std::bind(&ndb::ndb_c::cmd_break, this, std::placeholders::_1) }
    };
}

int ndb_c::execute(const std::string& target) {

  fmt::print("ndb execute : {}\n", target);

  {
    std::filesystem::path path(target); 
    if (!std::filesystem::is_regular_file(path)) {
      fmt::print("'{}' is not a regular file\n", target);
      return 1;
    }
  }

  front::traced_file_ptr traced_file = front::allocate_traced_file(target);

  _ig = std::make_unique<front::intake::group_s>(
    traced_file, _runtime);
  
  std::ifstream in(target);
  if (!in.is_open()) {
    fmt::print("Unable to open file: {}\n", target);
    return 1;
  }

  state.file_data.reserve(1024);

  size_t line_no{1};
  std::string line;
  while(std::getline(in, line)) {
    state.file_data.push_back(line_s{line_no++, line});
  }

  in.close();

  return repl();
}

int ndb_c::repl() {

  std::unordered_map<std::string, std::vector<std::string>> auto_completion_map = {};

  std::filesystem::path history_path(".ndb.cache");
  if (std::filesystem::exists(history_path)) {
    std::filesystem::remove(history_path);
  }

  linenoise::SetHistoryMaxLen(MAX_HISTORY);
  linenoise::LoadHistory(history_path.c_str());
  linenoise::SetMultiLine(true);
  
  std::string prompt = PROMPT;
  while (state.repl_active) {

    std::string line;
    linenoise::Readline(prompt.c_str(), line);
    if (line.empty()) {
      continue;
    }

    linenoise::AddHistory(line.c_str());
    linenoise::SaveHistory(history_path.c_str());
    parse_command(line);
  }
  return 0;
}

bool ndb_c::acknowledge(const std::string& msg) {
  fmt::print("\n{}\n", msg);
  std::string s;
  linenoise::Readline("[y/n] >> ", s);
  std::transform(s.begin(), s.end(), s.begin(),
      [](unsigned char c) { return std::toupper(c); });
  if (s == "Y" || s == "YES") {
    return true;
  }
  return false;
}

void ndb_c::parse_command(std::string& cmd) {
  std::vector<std::string> pieces = chunk(ltrim(rtrim(cmd)));
  for(auto && c : _cmds) {
    if (c.name == pieces[0]) {
      pieces = std::vector<std::string>(pieces.begin() + 1, pieces.end());
      c.fn(pieces);
      return;
    }
  }
  fmt::print("Unknown command\n");
}

#define EXPECT_N_PARAMS(cmd_, n_) \
  if (cmd.size() != n_) { \
    fmt::print( \
      "Command '{}' expected '{}' parameters, but received '{}'\n", \
      cmd_, n_, cmd.size()); \
    return; \
  }

void ndb_c::cmd_help(std::vector<std::string>& cmd) {
  fmt::print("\n\tCommand\t\tDescription\n\n");
  for(auto &&c : _cmds) { 
    fmt::print("\t{}\t\t{}\n", c.name, c.help);
  }
  fmt::print("\n");
}

void ndb_c::cmd_run(std::vector<std::string>& cmd) {
  EXPECT_N_PARAMS("run", 0);
  state.running = true;
}

void ndb_c::cmd_quit(std::vector<std::string>& cmd) {
  EXPECT_N_PARAMS("quit", 0);
  if (state.running) {
    if (!acknowledge("Program is running. Terminate running program?")) {
      return;
    }
  }
  state.running = false;
  state.repl_active = false;
}
void ndb_c::cmd_quit(std::vector<std::string>& cmd) {
  EXPECT_N_PARAMS("break", 1);


  // see if its filename:num or num

  // if filename:num then we search filedata for joined commend (;DBG_FILE_JOIN:filename.nibi)
  //    - not yet supported, but the .nibi_cache dir should append files on import ?
  //        idk how we will handle imports yet... but we need a way to load multiple files
  //        here to mirror it. joins might be a pain because then line nums need to be tracked
  //        differently

  // For now we just do num.
}

} // namespace
