#pragma once

#include "error.hpp"
#include "token.hpp"
#include "libnibi/source.hpp"
#include "libnibi/cell.hpp"
#include "libnibi/parallel_hashmap/phmap.h"

#include <optional>
#include <vector>
#include <istream>
#include <functional>
#include <string_view>

namespace nibi {

class intake_c {
public:
  using error_cb_t = std::function<void(nibi::error_c)>;
  using function_router = phmap::parallel_node_hash_map<std::string, function_info_s> &;

  intake_c() = delete;
  intake_c(error_cb_t error_cb, source_manager_c &sm, function_router router);

  void read(std::string_view source, std::istream &is);

private:  
  struct tracker_s {
    std::size_t bracket_count{0};
    std::size_t paren_count{0};
    std::size_t brace_count{0};
    std::size_t line_count{0};
  };

  tracker_s tracker_;
  error_cb_t error_cb_;
  source_manager_c &sm_;
  function_router router_;
  std::vector<token_c> tokens_;

  bool process_line(std::string_view line, 
    std::shared_ptr<source_origin_c> origin
    locator_ptr loc_override = nullptr);

  void process_token(token_c token);
  
};



} // nibi
