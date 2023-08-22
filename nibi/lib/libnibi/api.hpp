#pragma once
#include <cstddef>
#include <cstdint>

namespace nibi {
namespace api {

//! \brief Returns the version of the API
extern size_t get_api_version();

//! \brief Returns a unique id for an aberrant cell
//!        that an external library can use to confirm
//!        that the cell is one they generated.
//! \note  This function is thread-safe.
extern uint64_t get_aberrant_id();

} // namespace api
} // namespace nibi
