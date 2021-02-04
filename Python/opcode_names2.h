static const char *opcode_names[256] = {
    "unknown_opcode",
    "CLEAR_ACC",
    "FUNC_HEADER",
    "METHOD_HEADER",
    "GENERATOR_HEADER",
    "unknown_opcode",
    "CFUNC_HEADER",
    "unknown_opcode",
    "unknown_opcode",
    "NOP",
    "UNARY_POSITIVE",
    "UNARY_NEGATIVE",
    "UNARY_NOT",
    "UNARY_NOT_FAST",
    "unknown_opcode",
    "UNARY_INVERT",
    "BINARY_MATRIX_MULTIPLY",
    "INPLACE_MATRIX_MULTIPLY",
    "unknown_opcode",
    "BINARY_POWER",
    "BINARY_MULTIPLY",
    "unknown_opcode",
    "BINARY_MODULO",
    "BINARY_ADD",
    "BINARY_SUBTRACT",
    "BINARY_SUBSCR",
    "BINARY_FLOOR_DIVIDE",
    "BINARY_TRUE_DIVIDE",
    "INPLACE_FLOOR_DIVIDE",
    "INPLACE_TRUE_DIVIDE",
    "LOAD_EXC",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "LOAD_INTRINSIC",
    "CALL_INTRINSIC_1",
    "CALL_INTRINSIC_N",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "RERAISE",
    "WITH_EXCEPT_START",
    "GET_AITER",
    "GET_ANEXT",
    "BEFORE_ASYNC_WITH",
    "unknown_opcode",
    "END_ASYNC_FOR",
    "INPLACE_ADD",
    "INPLACE_SUBTRACT",
    "INPLACE_MULTIPLY",
    "unknown_opcode",
    "INPLACE_MODULO",
    "STORE_SUBSCR",
    "DELETE_SUBSCR",
    "BINARY_LSHIFT",
    "BINARY_RSHIFT",
    "BINARY_AND",
    "BINARY_XOR",
    "BINARY_OR",
    "INPLACE_POWER",
    "GET_ITER",
    "GET_YIELD_FROM_ITER",
    "PRINT_EXPR",
    "LOAD_BUILD_CLASS",
    "YIELD_FROM",
    "GET_AWAITABLE",
    "LOAD_ASSERTION_ERROR",
    "INPLACE_LSHIFT",
    "INPLACE_RSHIFT",
    "INPLACE_AND",
    "INPLACE_XOR",
    "INPLACE_OR",
    "unknown_opcode",
    "unknown_opcode",
    "LIST_TO_TUPLE",
    "RETURN_VALUE",
    "IMPORT_STAR",
    "SETUP_ANNOTATIONS",
    "YIELD_VALUE",
    "POP_BLOCK",
    "unknown_opcode",
    "END_EXCEPT",
    "STORE_NAME",
    "DELETE_NAME",
    "UNPACK_SEQUENCE",
    "FOR_ITER",
    "UNPACK_EX",
    "STORE_ATTR",
    "DELETE_ATTR",
    "STORE_GLOBAL",
    "DELETE_GLOBAL",
    "unknown_opcode",
    "LOAD_CONST",
    "LOAD_NAME",
    "BUILD_TUPLE",
    "BUILD_LIST",
    "BUILD_SET",
    "BUILD_MAP",
    "LOAD_ATTR",
    "COMPARE_OP",
    "IMPORT_NAME",
    "IMPORT_FROM",
    "unknown_opcode",
    "JUMP_IF_FALSE",
    "JUMP_IF_TRUE",
    "JUMP",
    "POP_JUMP_IF_FALSE",
    "POP_JUMP_IF_TRUE",
    "LOAD_GLOBAL",
    "IS_OP",
    "CONTAINS_OP",
    "unknown_opcode",
    "unknown_opcode",
    "JUMP_IF_NOT_EXC_MATCH",
    "END_FINALLY",
    "CALL_FINALLY",
    "LOAD_FAST",
    "STORE_FAST",
    "DELETE_FAST",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "RAISE",
    "CALL_FUNCTION",
    "MAKE_FUNCTION",
    "BUILD_SLICE",
    "unknown_opcode",
    "unknown_opcode",
    "LOAD_DEREF",
    "STORE_DEREF",
    "DELETE_DEREF",
    "unknown_opcode",
    "unknown_opcode",
    "CALL_FUNCTION_KW",
    "CALL_FUNCTION_EX",
    "SETUP_WITH",
    "EXTENDED_ARG",
    "LIST_APPEND",
    "SET_ADD",
    "MAP_ADD",
    "LOAD_CLASSDEREF",
    "unknown_opcode",
    "END_WITH",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "SETUP_ASYNC_WITH",
    "unknown_opcode",
    "BUILD_CONST_KEY_MAP",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "LOAD_METHOD",
    "CALL_METHOD",
    "LIST_EXTEND",
    "SET_UPDATE",
    "DICT_MERGE",
    "DICT_UPDATE",
    "unknown_opcode",
    "unknown_opcode",
    "CLEAR_FAST",
    "COPY",
    "MOVE",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "unknown_opcode",
    "debug_regs"
};
