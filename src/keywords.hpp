/*
  All nibi keywords and their corresponding ids defined
  and mapped together for parsing / output
*/

#pragma once

#include <string>
#include <map>

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
static constexpr const char *STR_LIT = "str-lit";
static constexpr const char *STR_SET_AT = "str-set-at";
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
static constexpr const char *MEM_IS_SET = "mem-is-set";
static constexpr const char *ALIAS = "alias";
static constexpr const char *EXCHANGE = "exchange";
static constexpr const char *DEFER = "defer";

namespace id {

static constexpr uint8_t TERR = 0;
static constexpr uint8_t EQ = 1;
static constexpr uint8_t NEQ = 2;
static constexpr uint8_t LT = 3;
static constexpr uint8_t GT = 4;
static constexpr uint8_t LTE = 5;
static constexpr uint8_t GTE = 6;
static constexpr uint8_t AND = 7;
static constexpr uint8_t OR = 8;
static constexpr uint8_t NOT = 9;
static constexpr uint8_t ADD = 10;
static constexpr uint8_t SUB = 11;
static constexpr uint8_t DIV = 12;
static constexpr uint8_t MUL = 13;
static constexpr uint8_t MOD = 14;
static constexpr uint8_t POW = 15;
static constexpr uint8_t ASSIGN = 16;
static constexpr uint8_t SET = 17;
static constexpr uint8_t FN = 18;
static constexpr uint8_t DICT = 19;
static constexpr uint8_t DROP = 20;
static constexpr uint8_t TRY = 21;
static constexpr uint8_t THROW = 22;
static constexpr uint8_t ASSERT = 23;
static constexpr uint8_t PUSH_FRONT = 24;
static constexpr uint8_t PUSH_BACK = 25;
static constexpr uint8_t POP_FRONT = 26;
static constexpr uint8_t POP_BACK = 27;
static constexpr uint8_t SPAWN = 28;
static constexpr uint8_t ITER = 29;
static constexpr uint8_t AT = 30;
static constexpr uint8_t LEN = 31;
static constexpr uint8_t YIELD = 32;
static constexpr uint8_t LOOP = 33;
static constexpr uint8_t IF = 34;
static constexpr uint8_t CLONE = 35;
static constexpr uint8_t IMPORT = 36;
static constexpr uint8_t USE = 37;
static constexpr uint8_t EXIT = 38;
static constexpr uint8_t EVAL = 39;
static constexpr uint8_t QUOTE = 40;
static constexpr uint8_t NOP = 41;
static constexpr uint8_t BW_LSH = 42;
static constexpr uint8_t BW_RSH = 43;
static constexpr uint8_t BW_AND = 44;
static constexpr uint8_t BW_OR = 45;
static constexpr uint8_t BW_XOR = 46;
static constexpr uint8_t BW_NOT = 47;
static constexpr uint8_t STR = 48;
static constexpr uint8_t STR_LIT = 49;
static constexpr uint8_t STR_SET_AT = 50;
static constexpr uint8_t INT = 51;
static constexpr uint8_t I8 = 52;
static constexpr uint8_t I16 = 53;
static constexpr uint8_t I32 = 54;
static constexpr uint8_t I64 = 55;
static constexpr uint8_t U8 = 56;
static constexpr uint8_t U16 = 57;
static constexpr uint8_t U32 = 58;
static constexpr uint8_t U64 = 59;
static constexpr uint8_t FLOAT = 60;
static constexpr uint8_t SPLIT = 61;
static constexpr uint8_t F32 = 62;
static constexpr uint8_t F64 = 63;
static constexpr uint8_t CHAR = 64;
static constexpr uint8_t TYPE = 65;
static constexpr uint8_t MACRO = 66;
static constexpr uint8_t EXTERN_CALL = 67;
static constexpr uint8_t MEM_NEW = 68;
static constexpr uint8_t MEM_DEL = 69;
static constexpr uint8_t MEM_CPY = 70;
static constexpr uint8_t MEM_LOAD = 71;
static constexpr uint8_t MEM_IS_SET = 72;
static constexpr uint8_t ALIAS = 73;
static constexpr uint8_t EXCHANGE = 74;
static constexpr uint8_t DEFER = 75;

} // namespace id

} // namespace kw

static std::map<std::string, uint8_t> symbol_id_map {
{ kw::TERR       ,  kw::id::TERR},
{ kw::EQ         ,  kw::id::EQ},
{ kw::NEQ        ,  kw::id::NEQ},
{ kw::LT         ,  kw::id::LT},
{ kw::GT         ,  kw::id::GT},
{ kw::LTE        ,  kw::id::LTE},
{ kw::GTE        ,  kw::id::GTE},
{ kw::AND        ,  kw::id::AND},
{ kw::OR         ,  kw::id::OR},
{ kw::NOT        ,  kw::id::NOT},
{ kw::ADD        ,  kw::id::ADD},
{ kw::SUB        ,  kw::id::SUB},
{ kw::DIV        ,  kw::id::DIV},
{ kw::MUL        ,  kw::id::MUL},
{ kw::MOD        ,  kw::id::MOD},
{ kw::POW        ,  kw::id::POW},
{ kw::ASSIGN     ,  kw::id::ASSIGN},
{ kw::SET        ,  kw::id::SET},
{ kw::FN         ,  kw::id::FN},
{ kw::DICT       ,  kw::id::DICT},
{ kw::DROP       ,  kw::id::DROP},
{ kw::TRY        ,  kw::id::TRY},
{ kw::THROW      ,  kw::id::THROW},
{ kw::ASSERT     ,  kw::id::ASSERT},
{ kw::PUSH_FRONT ,  kw::id::PUSH_FRONT},
{ kw::PUSH_BACK  ,  kw::id::PUSH_BACK},
{ kw::POP_FRONT  ,  kw::id::POP_FRONT},
{ kw::POP_BACK   ,  kw::id::POP_BACK},
{ kw::SPAWN      ,  kw::id::SPAWN},
{ kw::ITER       ,  kw::id::ITER},
{ kw::AT         ,  kw::id::AT},
{ kw::LEN        ,  kw::id::LEN},
{ kw::YIELD      ,  kw::id::YIELD},
{ kw::LOOP       ,  kw::id::LOOP},
{ kw::IF         ,  kw::id::IF},
{ kw::CLONE      ,  kw::id::CLONE},
{ kw::IMPORT     ,  kw::id::IMPORT},
{ kw::USE        ,  kw::id::USE},
{ kw::EXIT       ,  kw::id::EXIT},
{ kw::EVAL       ,  kw::id::EVAL},
{ kw::QUOTE      ,  kw::id::QUOTE},
{ kw::NOP        ,  kw::id::NOP},
{ kw::BW_LSH     ,  kw::id::BW_LSH},
{ kw::BW_RSH     ,  kw::id::BW_RSH},
{ kw::BW_AND     ,  kw::id::BW_AND},
{ kw::BW_OR      ,  kw::id::BW_OR},
{ kw::BW_XOR     ,  kw::id::BW_XOR},
{ kw::BW_NOT     ,  kw::id::BW_NOT},
{ kw::STR        ,  kw::id::STR},
{ kw::STR_LIT    ,  kw::id::STR_LIT},
{ kw::STR_SET_AT ,  kw::id::STR_SET_AT},
{ kw::INT        ,  kw::id::INT},
{ kw::I8         ,  kw::id::I8},
{ kw::I16        ,  kw::id::I16},
{ kw::I32        ,  kw::id::I32},
{ kw::I64        ,  kw::id::I64},
{ kw::U8         ,  kw::id::U8},
{ kw::U16        ,  kw::id::U16},
{ kw::U32        ,  kw::id::U32},
{ kw::U64        ,  kw::id::U64},
{ kw::FLOAT      ,  kw::id::FLOAT},
{ kw::SPLIT      ,  kw::id::SPLIT},
{ kw::F32        ,  kw::id::F32},
{ kw::F64        ,  kw::id::F64},
{ kw::CHAR       ,  kw::id::CHAR},
{ kw::TYPE       ,  kw::id::TYPE},
{ kw::MACRO      ,  kw::id::MACRO},
{ kw::EXTERN_CALL,  kw::id::EXTERN_CALL},
{ kw::MEM_NEW    ,  kw::id::MEM_NEW},
{ kw::MEM_DEL    ,  kw::id::MEM_DEL},
{ kw::MEM_CPY    ,  kw::id::MEM_CPY},
{ kw::MEM_LOAD   ,  kw::id::MEM_LOAD},
{ kw::MEM_IS_SET ,  kw::id::MEM_IS_SET},
{ kw::ALIAS      ,  kw::id::ALIAS},
{ kw::EXCHANGE   ,  kw::id::EXCHANGE},
{ kw::DEFER      ,  kw::id::DEFER},
};

} // namespace nibi
