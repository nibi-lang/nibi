#include "apps/app.hpp"

#define VERSION "0.0.0"

static std::string help_message = fmt::format(R"(

  Nibi Help Page [nibi v{}]

  Command                   Description
  --------------------------------------------

  -h  --help                Show this message
  -v  --version             Show version
  -t  --test                Run tests on target

  --------------------------------------------

  The first argument given that is not a known Nibi
  argument will be assumed to be a file or directory
  to launch.
  Following unknown arguments will be assumed to be
  arguments for the specified target file/dir.

)", VERSION);

static std::string version_message = 
  fmt::format("Nibi version: {}\n", VERSION);

void arg_do_test(std::vector<std::string>& args, const size_t& i) {
  fmt::print("User wants to test !\n");
  return;
}

int main(int argc, char **argv) {

  app::data_ptr app_data {nullptr};
  {
    app::arg_map_t arg_map {
      APP_ARG("-t", "--test", arg_do_test)
    };

    app_data = app::make_data_ptr(
      argc, argv,
      help_message,
      version_message,
      arg_map);
  }

  if (!app_data->target.has_value()) {
    return front::intake::repl(
        app_data->intake_settings);
  }

  if (std::filesystem::is_regular_file(*app_data->target)) {
    return front::intake::file(
        app_data->intake_settings,
        *app_data->target);
  }

  if (std::filesystem::is_directory(*app_data->target)) {
    return front::intake::dir(
        app_data->intake_settings,
        *app_data->target);
  }

  fmt::print(
    "Given target is not a file or directory: {}\n",
    *app_data->target);
  return 1;
}

