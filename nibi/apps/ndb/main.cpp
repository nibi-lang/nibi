
#include "apps/app.hpp"
#include "ndb.hpp"

#define VERSION "0.0.0"

static std::string help_message = fmt::format(R"(

  Ndb Help Page [ndb v{}]

  Command                   Description
  --------------------------------------------

  -h  --help                Show this message
  -v  --version             Show version
  -t  --tui                 Start with TUI (todo)
  -l  --launch              Immediatly launch target

  --------------------------------------------

)", VERSION);

static std::string version_message = 
  fmt::format("Ndb version: {}\n", VERSION);

void arg_with_tui(std::vector<std::string>& args, const size_t& i) {
  fmt::print("User wants to use a tui!\n");
  return;
}

void arg_immediate_launch(std::vector<std::string>& args, const size_t& i) {
  fmt::print("User wants to launch right away!\n");
  return;
}

int main(int argc, char **argv) {

  app::data_ptr app_data {nullptr};
  {
    app::arg_map_t arg_map {
      APP_ARG("-t", "--tui", arg_with_tui)
      APP_ARG("-l", "--launch", arg_immediate_launch)
    };

    app_data = app::make_data_ptr(
      argc, argv,
      help_message,
      version_message,
      arg_map);
  }

  ndb::ndb_c ndb(
    app_data->importer,
    app_data->target_args,
    app_data->target_stdin);

  if (!app_data->target.has_value()) {
    fmt::print("No target given\n");
    return 1;
  }

  if (std::filesystem::is_regular_file(*app_data->target)) {
    return ndb.execute(*app_data->target);
  }

  if (std::filesystem::is_directory(*app_data->target)) {
    fmt::print("DIRECTORY RUNNING NOT YET COMPLETED\n");
    return 0;
  }

  fmt::print(
    "Given target is not a file or directory: {}\n",
    *app_data->target);
  return 1;
}

