#include "app/repl/repl.hpp"
#include "app/linenoise/linenoise.hpp"
#include "app/repl/completions.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <libnibi/nibi.hpp>

namespace app {

namespace {

constexpr std::size_t MAX_HISTORY = 1024;
constexpr char PROMPT[] = "nibi> ";

class input_buffer_c {
public:
  std::optional<std::string> submit(std::string &data);

private:
  std::string _buffer;
  uint64_t _tracker{0};
};

std::optional<std::string> input_buffer_c::submit(std::string &line) {

  if (auto pos = line.find_first_of('#'); pos != std::string::npos) {
    line = line.substr(0, pos);
  }

  if (line.empty()) {
    return {};
  }

  // Walk the line and see if the given brackets
  // indicate that we have a full
  for (auto &c : line) {
    if (c == '(') {
      _tracker++;
    } else if (c == ')') {
      _tracker--;
    }
    _buffer += c;
  }

  // If we have a statement and all brackets are closed then
  // we can submit the statement
  if (_tracker == 0 && !_buffer.empty()) {
    std::string ret_buffer = _buffer;
    _buffer.clear();
    return {ret_buffer};
  }

  return {};
}

} // namespace

void start_repl(repl_config_s config) {

  auto completion_map = completions::get_completion_map();

  linenoise::SetCompletionCallback(
      [&](const char *editBuffer, std::vector<std::string> &completions) {
        std::string first = editBuffer;
        if (completion_map.find(first) != completion_map.end()) {
          std::vector<std::string> v = completion_map[first];
          completions.insert(completions.end(), v.begin(), v.end());
        }
      });

  std::filesystem::path history_path(".nibi_repl_history.txt");
  {
    auto home = nibi::global_platform->get_nibi_path();
    if (home.has_value()) {
      history_path = std::filesystem::path(*home);
      history_path /= "repl_history.txt";
    }
  }

  if (std::filesystem::exists(history_path)) {
    std::filesystem::remove(history_path);
  }

  linenoise::SetHistoryMaxLen(MAX_HISTORY);
  linenoise::LoadHistory(history_path.c_str());
  linenoise::SetMultiLine(true);

  auto interpreter = nibi::interpreter_factory_c::line_interpreter(
      [](nibi::error_c e) { e.draw(); });

  if (config.prelude.has_value()) {
    interpreter->interpret_line(*config.prelude);
  }

  uint64_t line_number{0};
  bool show_prompt{true};
  std::string buffer;

  bool _do{true};
  input_buffer_c _buffer;
  std::string prompt = PROMPT;
  while (_do) {

    line_number++;

    if (show_prompt) {
      prompt = PROMPT;
    } else {
      prompt = "";
    }

    std::string line;
    if (linenoise::Readline(prompt.c_str(), line)) {
      _do = false;
      continue;
    }
    if (line.empty()) {
      continue;
    }

    auto buffer = _buffer.submit(line);
    if (buffer.has_value()) {

      // Add line to history
      linenoise::AddHistory((*buffer).c_str());

      // Save history
      linenoise::SaveHistory(history_path.c_str());

      interpreter->interpret_line(*buffer);

      std::cout << interpreter->get_result() << std::endl;

      show_prompt = true;

    } else {
      show_prompt = false;
    }
  }
}

} // namespace app
