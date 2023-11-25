#include "nibi.hpp"
#include "atoms.hpp"
#include <fmt/format.h>
#include <set>
#include <map>

namespace {
  struct list_verify_info_s {
    const std::string& origin;
    atom_list_t& list;
  };

  using verification_map_t = std::map<
    std::string,
    std::function<void(const list_verify_info_s&)>>;

  std::unique_ptr<verification_map_t> verification_map{nullptr};

  std::set<atom_type_e> potential_lvalues {
    atom_type_e::SYMBOL,
    atom_type_e::LIST
  };
}

void verify_list(const std::string& origin, atom_list_t& list);
void verify_builtin_fn(const list_verify_info_s& tbis);
void verify_builtin_if(const list_verify_info_s& tbis);
void verify_builtin_let(const list_verify_info_s& tbis);
void verify_builtin_set(const list_verify_info_s& tbis);
void verify_builtin_assert(const list_verify_info_s& tbis);

#define B_ASSERT(cond_, msg_, pos_) \
  if (!(cond_)) { \
  g_nibi->report_error( \
    "instruction builder",\
    { tbis.origin, msg_, pos_ }); \
  }
    
#define EXPECT_LEN(cond_, len_, msg_) \
  B_ASSERT(\
    tbis.list.size() cond_ len_,\
    msg_,\
    tbis.list[0]->pos)

#define EXPECT_TYPE(type_, idx_, list_) \
  B_ASSERT(\
    list_[idx_]->type == type_,\
    fmt::format("Expected type '{}', got '{}'", \
      atom_type_to_string(type_), \
      atom_type_to_string(list_[idx_]->type)),\
      list_[idx_]->pos)

#define EXPECT_SYM(idx_) \
  EXPECT_TYPE(atom_type_e::SYMBOL, idx_, tbis.list)

#define EXPECT_LIST(idx_) \
  EXPECT_TYPE(atom_type_e::LIST, idx_, tbis.list)

#define ADD_ENTRY(name_, fn_) \
  (*verification_map.get())[name_] = fn_;

#define VERIFY_INNER(idx_)\
  verify_list(\
    tbis.origin, \
    reinterpret_cast<atom_list_c*>( \
        tbis.list[idx_].get())->data);

#define CONDITIONAL_VERIFY_INNER(idx_) \
  if (tbis.list[idx_]->type == atom_type_e::LIST) {\
    VERIFY_INNER(idx_) \
  }

// ---------------------------------------------------------

verification_map_t& get_verification_map() {
  if (verification_map) return *verification_map.get();

  verification_map = std::make_unique<verification_map_t>();
  ADD_ENTRY("fn", verify_builtin_fn)
  ADD_ENTRY("if", verify_builtin_if)
  ADD_ENTRY("let", verify_builtin_let)
  ADD_ENTRY("set", verify_builtin_set)
  ADD_ENTRY("assert", verify_builtin_assert)

  return *verification_map.get();
}

void verify_builtin_fn(const list_verify_info_s& tbis) {
  EXPECT_LEN(>=, 4, "Missing required components for construction");
  EXPECT_SYM(1);
  EXPECT_LIST(2);

  {
    auto& params = reinterpret_cast<atom_list_c*>(
      tbis.list[2].get())->data;
    for(size_t i = 0; i < params.size(); i++) {
      EXPECT_TYPE(atom_type_e::SYMBOL, i, params);
    }
  }

  for (size_t i = 3; i < tbis.list.size(); i++) {
    EXPECT_LIST(i);
    VERIFY_INNER(i);
  }
}

void verify_builtin_if(const list_verify_info_s& tbis) {
  B_ASSERT(
    (tbis.list.size() == 3 || tbis.list.size() == 4),
    "'if' requires form '( if <cond> <true> (<false>)? )'",
    tbis.list[0]->pos);
  for (size_t i = 1; i < tbis.list.size(); i++) {
    EXPECT_LIST(i);
    VERIFY_INNER(i);
  }
}

void verify_builtin_let(const list_verify_info_s& tbis) {
  EXPECT_LEN(==, 3, "'let' requires form '(let <var> <value>)'");
  EXPECT_SYM(1);
  CONDITIONAL_VERIFY_INNER(2);
}

void verify_builtin_set(const list_verify_info_s& tbis) {
  EXPECT_LEN(==, 3, "'set' requires form '(set <target> <value>)'");
  B_ASSERT(
    (potential_lvalues.contains(tbis.list[1]->type)),
    "'set' requires <target> to be a list or a direct value reference",
    tbis.list[1]->pos);
  CONDITIONAL_VERIFY_INNER(1);
  CONDITIONAL_VERIFY_INNER(2);
}

void verify_builtin_assert(const list_verify_info_s& tbis) {
  B_ASSERT(
    (tbis.list.size() == 2 || tbis.list.size() == 3),
    "'assert' requires form '( assert <cond> (<action/message>)? )'",
    tbis.list[0]->pos);
  CONDITIONAL_VERIFY_INNER(1);

  if (tbis.list.size() == 3) {
    CONDITIONAL_VERIFY_INNER(2);
  }
}

void verify_list(const std::string& origin, atom_list_t& list) {
  list_verify_info_s tbis = {origin, list};
  if (tbis.list.empty()) {
    return;
  }
  EXPECT_SYM(0);
  auto* as_sym = reinterpret_cast<atom_symbol_c*>(tbis.list[0].get());
  auto& tm = get_verification_map();
  auto entry = tm.find(as_sym->data);
  if (entry != tm.end()) {
    entry->second(tbis);
  }
}
