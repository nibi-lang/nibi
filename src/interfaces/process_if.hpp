#pragma once

namespace nibi {

class process_if {
public:
  virtual bool is_running() = 0;
  virtual bool start() = 0;
  virtual bool stop() = 0;
};


} // namespace nibi
