#include "libnibi/cell.hpp"
#include "libnibi/environment.hpp"

#include <iostream>

namespace nibi {
namespace builtins {

cell_ptr builtin_fn_extern_call(cell_processor_if &ci,
                                cell_list_t &list, env_c &env) {

  std::cout << "extern-call: >>> " << list[0]->to_string() << std::endl;

  return allocate_cell((int64_t)0);
}

} // namespace builtins
} // namespace nibi

  
