#pragma once

#include "atoms.hpp"

namespace front {

class parser_c final : public atom_receiver_if {
public:
    void on_error(error_s) override;
    void on_list(atom_list_t list) override;
    void on_top_list_complete() override;
private:
};

} // namespace
