#include "cell.hpp"


cell_list_t& cell_c::as_list() {
    try
    {
      return std::any_cast<cell_list_t&>(this->data);
    }
    catch (const std::bad_any_cast& e)
    {
        throw cell_access_exception_c(
          "Cell is not a list", this->locator);
    }
}

