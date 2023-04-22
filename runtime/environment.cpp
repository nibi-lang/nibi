#include "environment.hpp"

env_c::~env_c() {
  // If there is a parent environment, remove this environment
  // from the parent environment
  if (parent_env_) {
    parent_env_->remove_child();
  }

  // Mark all cells in the environment as not being in use
  for(auto& [name, cell] : cell_map_) {
    cell->mark_as_in_use(false);
  }
}

env_c::env_c(cell_list_t& parameter_list,
        env_c* parent_env)
  : parent_env_(parent_env) {

  // If a parent environment is provided, register this environment
  if (parent_env_) {
    parent_env_->register_child(*this);
  }
}

env_c::env_c(env_c* parent_env)
  : parent_env_(parent_env) {

  // If a parent environment is provided, register this environment
  if (parent_env_) {
    parent_env_->register_child(*this);
  }
}

cell_c* env_c::get_cell(std::string_view name) {

  // Check current environment first
  if (cell_map_.find(name) != cell_map_.end()) {
    return cell_map_[name];
  }

  // If not found, check parent environment
  if (parent_env_) {
    return parent_env_->get_cell(name);
  }

  // If not found, return nullptr
  return nullptr;
}

void env_c::set_local_cell(std::string_view name, cell_c& cell) {
  cell_map_[name] = &cell;
}

bool env_c::drop_cell(std::string_view name) {

  // Check current environment first
  if (cell_map_.find(name) != cell_map_.end()) {
    
    // Immediately mark the cell as not in use
    cell_map_[name]->mark_as_in_use(false);

    // Remove the cell from the environment
    cell_map_.erase(name);

    return true;
  }

  // If not found, check parent environment
  if (parent_env_) {
    return parent_env_->drop_cell(name);
  }

  // If not found, return false
  return false;
}

void env_c::mark_items_in_use() {

  // Mark all cells in the environment as being in use
  for(auto& [name, cell] : cell_map_) {
    cell->mark_as_in_use(true);
  }

  // If there is a child environment, mark it as being in use
  if (child_env_) {
    child_env_->mark_items_in_use();
  }
}

void env_c::register_child(env_c& child_env) {
  child_env_ = &child_env;
}