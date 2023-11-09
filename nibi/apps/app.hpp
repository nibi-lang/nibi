/*
    This file offers up a data struct that compiles information
    at the start of a program through argument parsing/ etc that
    is needed by the nibi applications
*/

#pragma once

#include "platform/system.hpp"
#include "front/intake.hpp"
#include "runtime/context.hpp"

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <functional>
#include <memory>

#define APP_ARG(small, large, fn) \
  { small, fn }, { large, fn },

namespace app {

using arg_fn = std::function<void(
    std::vector<std::string>&, const size_t&)>;

using arg_map_t = std::map<std::string, arg_fn>;

struct data_s {

  std::vector<std::string> target_args;
  std::vector<std::string> target_stdin;
  runtime::context_c runtime;
  front::intake::settings_s intake_settings;
  std::optional<std::string> target;

  data_s(
      int argc, char** argv,
      std::string help_msg,
      std::string version,
      arg_map_t& arg_map)
    : runtime(target_args, target_stdin),
      intake_settings(runtime){ 

      std::vector<std::string> args(argv, argv + argc);
    
      for(size_t i = 1; i < args.size(); i++) {
    
        if (args[i] == "-h" || args[i] == "--help") {
          fmt::print("{}\n", help_msg);
          std::exit(0);
        }
    
        if (args[i] == "-v" || args[i] == "--version") {
          fmt::print("{}\n", version);
          std::exit(0);
        }

        auto arg_handler = arg_map.find(args[i]);
        if (arg_handler != arg_map.end()) {
          arg_handler->second(args, i);
          continue;
        }
    
        if (!target.has_value()) {
          target = {args[i]};
          continue;
        }
    
        target_args.push_back(args[i]);
      }
    
      if (!platform::stdin_is_tty()) {
        std::string x;
        while(std::cin >> x) {
          target_stdin.push_back(x);
        }
      }
  }
};

using data_ptr = std::unique_ptr<app::data_s>;

constexpr auto make_data_ptr = [](auto... args)
  -> data_ptr{
    return std::make_unique<app::data_s>(args...);
  };

} // namespace
