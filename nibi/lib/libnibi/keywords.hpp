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
static constexpr const char *DICT = "dict";
static constexpr const char *DROP = "drop";
static constexpr const char *TRY = "try";
static constexpr const char *THROW = "throw";
static constexpr const char *ASSERT = "assert";
static constexpr const char *PUSH_FRONT = ">|";
static constexpr const char *PUSH_BACK = "|<";
static constexpr const char *POP_FRONT = "<<|";
static constexpr const char *POP_BACK = "|>>";
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
static constexpr const char *BW_RSH = "bw_rsh";
static constexpr const char *BW_AND = "bw-and";
static constexpr const char *BW_OR = "bw-or";
static constexpr const char *BW_XOR = "bw-xor";
static constexpr const char *BW_NOT = "bw-not";
static constexpr const char *STR = "str";
static constexpr const char *INT = "int";
static constexpr const char *I8 = "i8";
static constexpr const char *I16 = "i16";
static constexpr const char *I32 = "i32";
static constexpr const char *I64 = "i64";
static constexpr const char *U8 = "u8";
static constexpr const char *U16 = "u16";
static constexpr const char *U32 = "u32";
static constexpr const char *U64 = "u64";
static constexpr const char *FLOAT = "float";
static constexpr const char *SPLIT = "split";
static constexpr const char *F32 = "f32";
static constexpr const char *F64 = "f64";
static constexpr const char *CHAR = "char";
static constexpr const char *TYPE = "type";
static constexpr const char *MACRO = "macro";
static constexpr const char *EXTERN_CALL = "extern-call";
static constexpr const char *MEM_NEW = "mem-new";
static constexpr const char *MEM_DEL = "mem-del";
static constexpr const char *MEM_CPY = "mem-cpy";
static constexpr const char *MEM_LOAD = "mem-load";
static constexpr const char *MEM_OWNED = "mem-owned";
static constexpr const char *MEM_ACQUIRE = "mem-acquire";
static constexpr const char *MEM_ABANDON = "mem-abandon";
static constexpr const char *MEM_IS_SET = "mem-is-set";
static constexpr const char *ALIAS = "alias";

} // namespace kw
} // namespace nibi
