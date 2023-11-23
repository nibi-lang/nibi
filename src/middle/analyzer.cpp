#include "analyzer.hpp"

#include <fmt/format.h>
#include <map>
#include <string>
#include <functional>
#include "nibi.hpp"

static constexpr const char* NIBI_INTERNAL_ORIGIN = "analyzer";
#define REP_FATAL(msg_, pos_) \
 g_nibi->report_error(\
    NIBI_INTERNAL_ORIGIN,\
    { origin,\
      msg_,\
      pos_});

#define WORD_BYTES 8
#define HALF_WORD_BYTES 4

uint32_t calculate_datum_size_bytes(const datum_info_s& di) {
  if (di.is_ref || 
      di.classification == data_classification_e::VEC) {
    return 4; // 32bit address
  }
  switch (di.type) {
    case data_type_e::U8 : return 1;
    case data_type_e::U16: return 2;
    case data_type_e::U32: return 4;
    case data_type_e::U64: return 8;
    case data_type_e::I8 : return 1;
    case data_type_e::I16: return 2;
    case data_type_e::I32: return 4;
    case data_type_e::I64: return 8;
    case data_type_e::F32: return 4;
    case data_type_e::F64: return 8;
    case data_type_e::USER_DEFINED:
      fmt::print(
        "{} ERROR: User defined objects not yet impld\n",
        __FILE__);
      std::exit(-1);
  }

  fmt::print(
    "{} Internal error: Type not yet impld\n",
    __FILE__);
  std::exit(-1);
};

using aclass_e = atom_symbol_c::classification_e;

struct atom_expect_s {
  atom_type_e type;
  std::optional<aclass_e> symclass{std::nullopt};
  std::optional<std::string> symdata{std::nullopt};
};

using permitted_atoms_t = std::vector<atom_expect_s>;

struct list_expect_s {
  atom_list_t& list;
  std::string target;
  std::optional<size_t> min_len{std::nullopt};
  std::optional<size_t> max_len{std::nullopt};
  std::vector<permitted_atoms_t> list_expectations;
};

void enforce_expectations(const list_expect_s& expect) {

  if (expect.min_len.has_value()) {
      // Walk through expectations and make sure that 
      // the list follows them.

  }
}





parse_node_ptr analyze_fn(const std::string& origin, atom_list_t& list) {

  fmt::print("Analyze FN in {} at {}\n", origin, list[0]->pos.to_string());

  enforce_expectations({
    list,
    "Function definition",
    {4},
    {std::nullopt}, 
    {
      {{atom_type_e::SYMBOL, {std::nullopt}, "fn" }},   // Only one possibility for first atom
      {{atom_type_e::SYMBOL, {aclass_e::STANDARD}, }},  // Function name
      {                                                 // Return type
        {atom_type_e::SYMBOL, {aclass_e::TYPE_TAG}},
        {atom_type_e::SYMBOL, {aclass_e::TYPE_TAG_VEC}},
        {atom_type_e::SYMBOL, {aclass_e::REF_TAG}},
        {atom_type_e::SYMBOL, {aclass_e::REF_TAG_VEC}},
      },
      {{atom_type_e::LIST}},                             // Parameters
    }
  });

  return nullptr;
}

using tl_handler_fn = std::function<parse_node_ptr(
    const std::string&, atom_list_t&)>;

static std::map<std::string, tl_handler_fn>
  top_level_list_handlers = {
  { "fn", analyze_fn }
};

std::optional<parse_list_t> analyze(parse_group_s group) {

  fmt::print("Analyze {} list(s)\n", group.lists.size());
  const std::string& origin = group.origin;

  // Ensure all top-lists are eitehr 'program' or 'fn' lists. 
  // Ensure only one app
  // Unique fn signatures (names can be duped)

  // Once form and type of function is determined, 
  // pass to semantic boyo to ensure it all jives.
  // We do this in a second stage so we can easily (lazily)
  // ensure that calls can be made to functions declared 
  // after the function definition

  // TODO: At this stage we would handle imports, however,
  //       I don't believe we should import raw code here.
  //       I think we should either bring in the bytecode
  //       to check the export tables, or spawn a new sub-process
  //       of nibi to compile first, and then retrieve the bytecode
  //       This would mean that nibi's imports would be dynamically
  //       linked.
  //
  //       ... Perhaps imports would just be populating top-level 
  //       symbols so we could do that easily here. An actual linker
  //       could do the rest. 
  //
  //       ... We could statically "link" the things by dumping them together
  //

  for(auto&& list : group.lists) {

    if (list.empty()) continue;
    
    if (list[0]->type != atom_type_e::SYMBOL) {
       REP_FATAL(
          "Expected symbol for first argument to top-level list", 
          list[0]->pos);
    }

    auto sym = reinterpret_cast<atom_symbol_c*>(list[0].get());

    auto handler = top_level_list_handlers.find(sym->data);
   
    if (handler == top_level_list_handlers.end()) {
       REP_FATAL(
          fmt::format("Invalid top-level expression '{}'\n", sym->data),
          list[0]->pos);
    }

    handler->second(origin, list);
  }

  return {std::nullopt};
};
