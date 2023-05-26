#include <iostream>

#include "libnibi/nibi_factory.hpp"
#include <vector>

#include <CppUTest/TestHarness.h>

namespace {

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

  auto interpreter = nibi::nibi_factory_c::line_interpreter([](nibi::error_c e){
    e.draw();
    FAIL("Recieved Nibi Error");
    });
  for (auto &tc : test_cases) {
    interpreter->interpret_line(tc.data);
    CHECK_EQUAL(tc.expected, interpreter->get_result());
  }
}

TEST(nibi_input_tests, data) {
  std::vector<tc_s> test_cases{
      tc_s{"(x [1 2 3])", "(x [1 2 3])"},
      tc_s{"(x {a b c})", "(x {a b c})"},
      tc_s{"(x 3.14)", "(x 3.140000)"},
      tc_s{"(x \"WHAT\")", "(x \"WHAT\")"},
  };

  auto interpreter = nibi::nibi_factory_c::line_interpreter([](nibi::error_c e){
    e.draw();
    FAIL("Recieved Nibi Error");
    });
  for (auto &tc : test_cases) {
    interpreter->interpret_line(tc.data);
    CHECK_EQUAL(tc.expected, interpreter->get_result());
  }
}
