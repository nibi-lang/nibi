#pragma once

#include "front/intake.hpp"

#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include <vector>
#include "front/imports.hpp"

namespace ndb {

/*
    TODO: 

    Make the debugger mock up the ins_receiver 
    so we can make a full pass over the code and only handle 
    imports. We intercept so we don't end up executing the
    instructions. 

    When receiving instructions we can build up a total list of bytecode
    to work against.

    When we hit imports, we should add them locally to memory so we
    can manually feed them to the local engine later


*/


class ndb_c : public front::import_c::intercepter_if {
public:
  ndb_c(
    front::import_c &importer,
    std::vector<std::string> target_args,
    std::vector<std::string> target_stdin);

  int execute(const std::string& target);

  bool intercept_import(
      std::filesystem::path& file_path,
      std::filesystem::path& imported_from) override;
private:
  using cmd_fn = std::function<void(std::vector<std::string>&)>;
  runtime::context_c _runtime;
  front::import_c &_importer;
  std::unique_ptr<front::intake::group_s> _ig{nullptr};

  struct command_s {
    std::string name;
    std::string help;
    cmd_fn fn;
  };

  std::vector<command_s> _cmds;

  struct line_s {
    uint64_t number{0};
    std::string data;
  };

  struct state_s {
    bool repl_active{true};
    bool running{false};
    std::unordered_map<size_t, uint64_t> breakpoints;
    std::vector<line_s> file_data;
  };

  state_s state;

  int repl();

  void parse_command(std::string& cmd);


  bool acknowledge(const std::string& message);
  void cmd_help(std::vector<std::string>& cmd);
  void cmd_run(std::vector<std::string>& cmd);
  void cmd_quit(std::vector<std::string>& cmd);
  void cmd_break(std::vector<std::string>& cmd);
};

} // namespace
