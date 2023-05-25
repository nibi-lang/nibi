#include <iostream>

#include "libnibi/common/input.hpp"
#include "libnibi/common/instruction_processor_if.hpp"
#include "libnibi/common/list.hpp"
#include "libnibi/common/token.hpp"
#include "libnibi/source.hpp"
#include <vector>

#include <CppUTest/TestHarness.h>

namespace {
class receiver_c : public nibi::instruction_processor_if {
public:
  receiver_c(std::string expected) : expected_{expected} {}
  void instruction_ind(nibi::cell_ptr &list_cell) override {
    CHECK_EQUAL(expected_, list_cell->to_string(true));
  }

private:
  std::string expected_;
};

struct tc_s {
  std::string data;
  std::string expected;
};
} // namespace

#define TEST_GEN_BUILTIN_TC(__sym)                                             \
  "(" #__sym ")", "(<function:" #__sym ", type:BUILTIN_CPP_FUNCTION>)"

TEST_GROUP(nibi_input_tests){};

TEST(nibi_input_tests, builtins) {
  std::vector<tc_s> test_cases {
    tc_s{TEST_GEN_BUILTIN_TC(+)},
    tc_s{TEST_GEN_BUILTIN_TC(eq)},
    tc_s{TEST_GEN_BUILTIN_TC(neq)},
    tc_s{TEST_GEN_BUILTIN_TC(<)},
    tc_s{TEST_GEN_BUILTIN_TC(>)},
    tc_s{TEST_GEN_BUILTIN_TC(<=)},
    tc_s{TEST_GEN_BUILTIN_TC(>=)},
    tc_s{TEST_GEN_BUILTIN_TC(and)},
    tc_s{TEST_GEN_BUILTIN_TC(or)},
    tc_s{TEST_GEN_BUILTIN_TC(not)},
    tc_s{TEST_GEN_BUILTIN_TC(+)},
    tc_s{TEST_GEN_BUILTIN_TC(-)},
    tc_s{TEST_GEN_BUILTIN_TC(/)},
    tc_s{TEST_GEN_BUILTIN_TC(*)},
    tc_s{TEST_GEN_BUILTIN_TC(%)},
    tc_s{TEST_GEN_BUILTIN_TC(**)},
    tc_s{TEST_GEN_BUILTIN_TC(:=)},
    tc_s{TEST_GEN_BUILTIN_TC(set)},
    tc_s{TEST_GEN_BUILTIN_TC(fn)},
    tc_s{TEST_GEN_BUILTIN_TC(drop)},
    tc_s{TEST_GEN_BUILTIN_TC(try)},
    tc_s{TEST_GEN_BUILTIN_TC(throw)},
    tc_s{TEST_GEN_BUILTIN_TC(assert)},
    tc_s{TEST_GEN_BUILTIN_TC(>|)},
    tc_s{TEST_GEN_BUILTIN_TC(|<)},
    tc_s{TEST_GEN_BUILTIN_TC(<|>)},
    tc_s{TEST_GEN_BUILTIN_TC(iter)},
    tc_s{TEST_GEN_BUILTIN_TC(at)},
    tc_s{TEST_GEN_BUILTIN_TC(len)},
    tc_s{TEST_GEN_BUILTIN_TC(<-)},
    tc_s{TEST_GEN_BUILTIN_TC(loop)},
    tc_s{TEST_GEN_BUILTIN_TC(?)},
    tc_s{TEST_GEN_BUILTIN_TC(clone)},
    tc_s{TEST_GEN_BUILTIN_TC(put)},
    tc_s{TEST_GEN_BUILTIN_TC(putln)},
    tc_s{TEST_GEN_BUILTIN_TC(import)},
    tc_s{TEST_GEN_BUILTIN_TC(use)},
    tc_s{TEST_GEN_BUILTIN_TC(bw-lsh)},
    tc_s{TEST_GEN_BUILTIN_TC(bw-rsh)},
    tc_s{TEST_GEN_BUILTIN_TC(bw-and)},
    tc_s{TEST_GEN_BUILTIN_TC(bw-or)},
    tc_s{TEST_GEN_BUILTIN_TC(bw-xor)},
    tc_s{TEST_GEN_BUILTIN_TC(bw-not)},
    tc_s{TEST_GEN_BUILTIN_TC(env)}
  };

  for (auto &tc : test_cases) {

    nibi::source_manager_c source_manager;

    receiver_c receiver{tc.expected};
    nibi::list_builder_c builder{receiver};
    nibi::scanner_c scanner{builder};

    auto origin = source_manager.get_source("test");

    scanner.scan_line(origin, tc.data);
    scanner.indicate_complete();
  }
}

TEST(nibi_input_tests, data) {
  std::vector<tc_s> test_cases{
      tc_s{"(x [1 2 3])", "(x [1 2 3])"},
      tc_s{"(x {a b c})", "(x {a b c})"},
      tc_s{"(x 3.14)", "(x 3.140000)"},
      tc_s{"(x \"WHAT\")", "(x \"WHAT\")"},
  };

  for (auto &tc : test_cases) {

    nibi::source_manager_c source_manager;

    receiver_c receiver{tc.expected};
    nibi::list_builder_c builder{receiver};
    nibi::scanner_c scanner{builder};

    auto origin = source_manager.get_source("test");

    scanner.scan_line(origin, tc.data);
    scanner.indicate_complete();
  }
}
