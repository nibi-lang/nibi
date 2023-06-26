#pragma once

namespace nibi {

namespace kw {

static constexpr const char *TERR = "$e";
static constexpr const char *EQ = "eq";
static constexpr const char *NEQ = "neq";
static constexpr const char *LT = "<";
static constexpr const char *GT = ">";
static constexpr const char *LTE = "<=";
static constexpr const char *GTE = ">=";
static constexpr const char *AND = "and";
static constexpr const char *OR = "or";
static constexpr const char *NOT = "not";
static constexpr const char *ADD = "+";
static constexpr const char *SUB = "-";
static constexpr const char *DIV = "/";
static constexpr const char *MUL = "*";
static constexpr const char *MOD = "%";
static constexpr const char *POW = "**";
static constexpr const char *ASSIGN = ":=";
static constexpr const char *SET = "set";
static constexpr const char *FN = "fn";
static constexpr const char *DROP = "drop";
static constexpr const char *TRY = "try";
static constexpr const char *THROW = "throw";
static constexpr const char *ASSERT = "assert";
static constexpr const char *ASSERT_EQ = "assert_eq";
static constexpr const char *ASSERT_NEQ = "assert_neq";
static constexpr const char *PUSH_FRONT = ">|";
static constexpr const char *PUSH_BACK = "|<";
static constexpr const char *POP_FRONT = "<<|";
static constexpr const char *POP_BACK = "|>>";
static constexpr const char *EXECUTE = "execute_list";
static constexpr const char *SPAWN = "<|>";
static constexpr const char *ITER = "iter";
static constexpr const char *AT = "at";
static constexpr const char *LEN = "len";
static constexpr const char *YIELD = "<-";
static constexpr const char *LOOP = "loop";
static constexpr const char *IF = "if";
static constexpr const char *CLONE = "clone";
static constexpr const char *IMPORT = "import";
static constexpr const char *USE = "use";
static constexpr const char *EXIT = "exit";
static constexpr const char *EVAL = "eval";
static constexpr const char *QUOTE = "quote";
static constexpr const char *NOP = "nop";
static constexpr const char *BW_LSH = "bw-lsh";
static constexpr const char *BW_RSH = "bw-rsh";
static constexpr const char *BW_AND = "bw-and";
static constexpr const char *BW_OR = "bw-or";
static constexpr const char *BW_XOR = "bw-xor";
static constexpr const char *BW_NOT = "bw_not";
static constexpr const char *STR = "str";
static constexpr const char *INT = "int";
static constexpr const char *FLOAT = "float";
static constexpr const char *SPLIT = "split";
static constexpr const char *TYPE = "type";

} // namespace kw
} // namespace nibi
