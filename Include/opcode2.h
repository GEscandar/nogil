/* Auto-generated by Tools/scripts/generate_opcode_h.py from Lib/opcode.py */
#ifndef Py_OPCODE2_H
#define Py_OPCODE2_H
#ifdef __cplusplus
extern "C" {
#endif


// Instruction opcodes for compiled code
//    name                   opcode   size   wide_size
#define OPCODE_LIST(_) \
    _(CLEAR_ACC,                  1,     1,      2) \
    _(CLEAR_FAST,                 2,     2,      6) \
    _(COPY,                       3,     3,     10) \
    _(MOVE,                       4,     3,     10) \
    _(FUNC_HEADER,                5,     2,      6) \
    _(METHOD_HEADER,              6,     1,      2) \
    _(COROGEN_HEADER,             7,     2,      6) \
    _(CFUNC_HEADER,               8,     1,      2) \
    _(FUNC_TPCALL_HEADER,         9,     1,      2) \
    _(UNARY_POSITIVE,            10,     1,      2) \
    _(UNARY_NEGATIVE,            11,     1,      2) \
    _(UNARY_NOT,                 12,     1,      2) \
    _(UNARY_NOT_FAST,            13,     1,      2) \
    _(UNARY_INVERT,              14,     1,      2) \
    _(BINARY_MATRIX_MULTIPLY,    15,     2,      6) \
    _(BINARY_POWER,              16,     2,      6) \
    _(BINARY_MULTIPLY,           17,     2,      6) \
    _(BINARY_MODULO,             18,     2,      6) \
    _(BINARY_ADD,                19,     2,      6) \
    _(BINARY_SUBTRACT,           20,     2,      6) \
    _(BINARY_SUBSCR,             21,     2,      6) \
    _(BINARY_FLOOR_DIVIDE,       22,     2,      6) \
    _(BINARY_TRUE_DIVIDE,        23,     2,      6) \
    _(BINARY_LSHIFT,             24,     2,      6) \
    _(BINARY_RSHIFT,             25,     2,      6) \
    _(BINARY_AND,                26,     2,      6) \
    _(BINARY_XOR,                27,     2,      6) \
    _(BINARY_OR,                 28,     2,      6) \
    _(IS_OP,                     29,     2,      6) \
    _(CONTAINS_OP,               30,     2,      6) \
    _(COMPARE_OP,                31,     3,     10) \
    _(INPLACE_FLOOR_DIVIDE,      32,     2,      6) \
    _(INPLACE_TRUE_DIVIDE,       33,     2,      6) \
    _(INPLACE_ADD,               34,     2,      6) \
    _(INPLACE_SUBTRACT,          35,     2,      6) \
    _(INPLACE_MULTIPLY,          36,     2,      6) \
    _(INPLACE_LSHIFT,            37,     2,      6) \
    _(INPLACE_RSHIFT,            38,     2,      6) \
    _(INPLACE_AND,               39,     2,      6) \
    _(INPLACE_XOR,               40,     2,      6) \
    _(INPLACE_OR,                41,     2,      6) \
    _(INPLACE_MODULO,            42,     2,      6) \
    _(INPLACE_MATRIX_MULTIPLY,   43,     2,      6) \
    _(INPLACE_POWER,             44,     2,      6) \
    _(LOAD_FAST,                 45,     2,      6) \
    _(LOAD_NAME,                 46,     2,      6) \
    _(LOAD_CONST,                47,     2,      6) \
    _(LOAD_ATTR,                 48,     3,     10) \
    _(LOAD_GLOBAL,               49,     3,     10) \
    _(LOAD_METHOD,               50,     3,     10) \
    _(LOAD_INTRINSIC,            51,     2,      6) \
    _(LOAD_DEREF,                52,     2,      6) \
    _(LOAD_CLASSDEREF,           53,     3,     10) \
    _(STORE_FAST,                54,     2,      6) \
    _(STORE_NAME,                55,     2,      6) \
    _(STORE_ATTR,                56,     3,     10) \
    _(STORE_GLOBAL,              57,     2,      6) \
    _(STORE_SUBSCR,              58,     3,     10) \
    _(STORE_DEREF,               59,     2,      6) \
    _(DELETE_FAST,               60,     2,      6) \
    _(DELETE_NAME,               61,     2,      6) \
    _(DELETE_ATTR,               62,     2,      6) \
    _(DELETE_GLOBAL,             63,     2,      6) \
    _(DELETE_SUBSCR,             64,     2,      6) \
    _(DELETE_DEREF,              65,     2,      6) \
    _(CALL_FUNCTION,             66,     4,      8) \
    _(CALL_FUNCTION_EX,          67,     2,      6) \
    _(CALL_METHOD,               68,     4,      8) \
    _(CALL_INTRINSIC_1,          69,     2,      6) \
    _(CALL_INTRINSIC_N,          70,     3,     10) \
    _(RETURN_VALUE,              71,     1,      2) \
    _(RAISE,                     72,     1,      2) \
    _(YIELD_VALUE,               73,     1,      2) \
    _(YIELD_FROM,                74,     2,      6) \
    _(JUMP,                      75,     3,      6) \
    _(JUMP_IF_FALSE,             76,     3,      6) \
    _(JUMP_IF_TRUE,              77,     3,      6) \
    _(JUMP_IF_NOT_EXC_MATCH,     78,     4,     10) \
    _(POP_JUMP_IF_FALSE,         79,     3,      6) \
    _(POP_JUMP_IF_TRUE,          80,     3,      6) \
    _(GET_ITER,                  81,     2,      6) \
    _(GET_YIELD_FROM_ITER,       82,     2,      6) \
    _(FOR_ITER,                  83,     4,     10) \
    _(IMPORT_NAME,               84,     2,      6) \
    _(IMPORT_FROM,               85,     3,     10) \
    _(IMPORT_STAR,               86,     2,      6) \
    _(BUILD_SLICE,               87,     2,      6) \
    _(BUILD_TUPLE,               88,     3,     10) \
    _(BUILD_LIST,                89,     3,     10) \
    _(BUILD_SET,                 90,     3,     10) \
    _(BUILD_MAP,                 91,     2,      6) \
    _(END_EXCEPT,                92,     2,      6) \
    _(CALL_FINALLY,              93,     4,     10) \
    _(END_FINALLY,               94,     2,      6) \
    _(LOAD_BUILD_CLASS,          95,     1,      2) \
    _(GET_AWAITABLE,             96,     2,      6) \
    _(GET_AITER,                 97,     2,      6) \
    _(GET_ANEXT,                 98,     2,      6) \
    _(END_ASYNC_WITH,            99,     2,      6) \
    _(END_ASYNC_FOR,            100,     2,      6) \
    _(UNPACK,                   101,     2,      6) \
    _(MAKE_FUNCTION,            102,     2,      6) \
    _(SETUP_WITH,               103,     2,      6) \
    _(END_WITH,                 104,     2,      6) \
    _(SETUP_ASYNC_WITH,         105,     2,      6) \
    _(LIST_EXTEND,              106,     2,      6) \
    _(LIST_APPEND,              107,     2,      6) \
    _(SET_ADD,                  108,     2,      6) \
    _(SET_UPDATE,               109,     2,      6) \
    _(DICT_MERGE,               110,     2,      6) \
    _(DICT_UPDATE,              111,     2,      6) \
    _(WIDE,                     112,     1,      2)


enum {
#define OPCODE_NAME(Name, Code, ...) Name = Code,
OPCODE_LIST(OPCODE_NAME)
#undef OPCODE_NAME
};

enum {
#define OPSIZE(Name, Code, Size, ...) OP_SIZE_##Name = Size,
OPCODE_LIST(OPSIZE)
#undef OPSIZE
};

enum {
#define OPSIZE(Name, Code, Size, WideSize) OP_SIZE_WIDE_##Name = WideSize,
OPCODE_LIST(OPSIZE)
#undef OPSIZE
};

#ifdef __cplusplus
}
#endif
#endif /* !Py_OPCODE2_H */
