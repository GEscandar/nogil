#include "Python.h"
#include "pycore_call.h"
#include "pycore_ceval.h"
#include "pycore_code.h"
#include "pycore_object.h"
#include "pycore_refcnt.h"
#include "pycore_pyerrors.h"
#include "pycore_pylifecycle.h"
#include "pycore_pystate.h"
#include "pycore_tupleobject.h"
#include "pycore_qsbr.h"
#include "pycore_dict.h"

#include "code.h"
#include "dictobject.h"
#include "frameobject.h"
#include "pydtrace.h"
#include "setobject.h"
#include "structmember.h"
#include "opcode2.h"
#include "ceval2_meta.h"

#include <ctype.h>

#define UNLIKELY _PY_UNLIKELY
#define LIKELY _PY_LIKELY

// tt  = type
//   r = refcounted
//     .. [ttr]
// 000 ..  000 NULL
// aaa ..  000 non-RC OBJ
// aaa ..  001 RC obj
//     ..  010 int
//     ..  100 pri (True, False, None)


#define TARGET(name) \
    TARGET_##name: \
   __asm__ volatile(".TARGET_" #name ":");


#define COLD_TARGET(name) \
    TARGET_##name: __attribute__((cold)) \
   __asm__ volatile(".TARGET_" #name ":");

#define CALL_VM(call) \
    ts->regs = regs; \
    call; \
    regs = ts->regs;

#define DECREF(reg) do { \
    if (IS_RC(reg)) { \
        PyObject *obj = reg.obj; \
        if (LIKELY(_Py_ThreadLocal(obj))) { \
            uint32_t refcount = obj->ob_ref_local; \
            refcount -= 4; \
            obj->ob_ref_local = refcount; \
            if (UNLIKELY(refcount == 0)) { \
                CALL_VM(vm_zero_refcount(obj)); \
            } \
        } \
        else { \
            CALL_VM(vm_decref_shared(obj)); \
        } \
    } \
} while (0)

struct metadata {

};

Code *func_code(PyFunc *func)
{
    return (Code *)((char *)func + sizeof(PyFunc));
}

__attribute__((noinline))
void foo() {
    __asm__ volatile ("");
}

PyObject* _PyEval_Fast(struct ThreadState *ts);

static const int64_t INT32_TAG = 0x2;
static const int64_t REFCOUNT_TAG = 0x1;
static const int64_t PRI_TAG = 0x4;

// static const int64_t PRI_NONE = 0x0;
// static const int64_t PRI_FALSE = 0x1;
static const int64_t PRI_TRUE = 0x2;

static PyObject *primitives[3] = {
    Py_None,
    Py_False,
    Py_True
};

static inline Register
PACK_INT32(int32_t value)
{
    Register r;
    r.as_int64 = INT32_TAG | ((int64_t)value << 32);
    return r;
}

static inline Register
PACK_BOOL(bool value)
{
    Register r;
    r.as_int64 = PRI_TAG | (((int64_t)value + 1) << 32);
    return r;
}

static inline bool
IS_PRI(Register r)
{
    return (r.as_int64 & PRI_TAG) != 0;
}

static inline int32_t
AS_PRI(Register r)
{
    return (r.as_int64 >> 32);
}

static inline bool
IS_OBJ(Register r)
{
    return (r.as_int64 & INT32_TAG) == 0;
}

static inline bool
IS_RC(Register r)
{
    return (r.as_int64 & REFCOUNT_TAG) != 0;
}

static inline bool
IS_INT32(Register r)
{
    return (r.as_int64 & INT32_TAG) != 0;
}

static inline bool
BOTH_INT32(Register a, Register b)
{
    return ((a.as_int64 & b.as_int64) & INT32_TAG) != 0;
}

static inline int32_t
AS_INT32(Register r)
{
    return (r.as_int64 >> 32);
}

static inline PyObject *
AS_OBJ(Register r)
{
    r.as_int64 &= ~1;
    return r.obj;
}

PyObject *callfib(PyObject *const *args, Py_ssize_t nargs)
{
    static uint32_t code[] = {
        FUNC_HEADER(5),
        LOAD_INT(2),
        TEST_LESS_THAN(0),
        JUMP_IF_FALSE(2),
        LOAD_INT(1),
        RET(),
        LOAD_GLOBAL(0, 3),
        MOVE(1),
        LOAD_INT(2),
        SUB(0),
        MOVE(3),
        CALL_FUNCTION(1, 1),
        MOVE(1),
        LOAD_GLOBAL(0, 3),
        MOVE(2),
        LOAD_INT(1),
        SUB(0),
        MOVE(4),
        CALL_FUNCTION(2, 1),
        ADD(1),
        CLEAR(1),
        RET()
    };

    static uint32_t retc[] = {
        RETURN_TO_C()
    };

    Register stack[256];
    memset(stack, 0, sizeof(stack));

    _Py_IDENTIFIER(fib);

    Register constants[2];
    memset(constants, 0, sizeof(constants));
    constants[0].obj = _PyUnicode_FromId(&PyId_fib);

    struct ThreadState *ts = malloc(sizeof(struct ThreadState));
    memset(ts, 0, sizeof(struct ThreadState));

    ts->pc = &code[0];
    ts->stack = stack;
    ts->maxstack = &stack[256];
    ts->regs = &stack[2];
    ts->constants = constants;
    ts->opcode_targets = malloc(sizeof(void*) * 256);
    memset(ts->opcode_targets, 0, sizeof(void*) * 256);
    ts->metadata = malloc(sizeof(uint16_t) * 32);
    memset(ts->metadata, 0, sizeof(uint16_t) * 32);

    PyObject *globals = PyDict_New();

    PyFunc *func = (PyFunc *)vm_new_func();
    func->code = code;
    func->constants = constants;
    func->nargs = 1;
    func->framesize = 5;
    func->globals = globals;

    _PyDict_SetItemId(globals, &PyId_fib, (PyObject *)func);
    PyDictKeysObject *keys = ((PyDictObject *)globals)->ma_keys;
    PyDictKeyEntry *entry = find_unicode(((PyDictObject *)globals)->ma_keys, _PyUnicode_FromId(&PyId_fib));
    Py_ssize_t offset = entry - keys->dk_entries;
    ts->metadata[3] = offset;

    stack[0].obj = (PyObject *)func; // this_func
    stack[1].as_int64 = (intptr_t)&retc; // retc
    stack[2] = PACK_INT32(PyLong_AsLong(args[0])); // arg

    ts->nargs = 1;
    return _PyEval_Fast(ts);
}

PyObject* bar(int a, int b, int c, int d);

#define USE_REGISTER(value, reg) do { \
        register __typeof__(value) reg asm(#reg) = value; \
        __asm__ volatile("" :: "r"(reg)); \
    } while (0)

__attribute__((noinline))
void baz() {
    __asm__ volatile ("");
}

#define NEXTOPARG() do { \
        opD = *next_instr; \
        opcode = opD & 0xFF; \
        opA = (opD >> 8) & 0xFF; \
        opD >>= 16; \
        next_instr++; \
    } while (0)

#define FAST_DISPATCH() \
    NEXTOPARG(); \
    goto *opcode_targets[opcode]

#define THIS_FUNC() \
    ((PyFunc *)AS_OBJ(regs[-2]))

// LLINT (JSCORE)
// Call Frame points to regs
// regs[0] = caller
// regs[1] = returnPC
// regs[2] = code block
// regs[3] = nargs
// regs[4] = this
// regs[5] = arg0
// ...
// code block ->
//   ...
//   pointer to constants array
//
// so loading constant at index N
// tmp = load regs[2]
// tmp = load tmp[offsetof constnt]
// tmp = load tmp[N]

PyObject*
_PyEval_Fast(struct ThreadState *ts)
{
    const Register *constants = ts->constants;
    const uint32_t *next_instr = ts->pc;
    intptr_t opcode;
    intptr_t opA;
    intptr_t opD;
    Register acc;
    // callee saved: rbx,rbp,r12,r13,r14, r15 6 usable registers + rsp

    // WebKit Saved:
    // regs (cfr)
    // next_instr (PB, well half of it)
    // tagTypeNumber
    // tagMask
    // wasmInstance
    // metadataTable

    // LuaJIT saved:
    // constants (KBASE)
    // PC
    // opcode_targets

    // We want saved:
    // next_instr (pc) -- webkit splits this in two: PB/PC PB is saved, PC is returned on calls
    // constants -- webkit sticks this in frame?
    // opcode_targets
    // ts (?)
    // OBJ_MASK
    // INT32_TAG

    static void *opcode_targets_base[256] = {
        0,
        &&TARGET_LOAD_INT,
        &&TARGET_TEST_LESS_THAN,
        &&TARGET_FUNC_HEADER,
        &&TARGET_JUMP_IF_FALSE,
        &&TARGET_RET,
        &&TARGET_LOAD_GLOBAL,
        &&TARGET_MOVE,
        &&TARGET_ADD,
        &&TARGET_SUB,
        &&TARGET_CALL_FUNCTION,
        &&TARGET_RETURN_TO_C,
        &&TARGET_CLEAR,
        0,
        0,
        &&TARGET_debug_regs
    };
    if (!ts->opcode_targets[0]) {
        memcpy(ts->opcode_targets, opcode_targets_base, sizeof(opcode_targets_base));
    }

    void **opcode_targets = ts->opcode_targets;
    uint16_t *metadata = ts->metadata;

    // NOTE: after memcpy call!
    Register *regs = ts->regs;
    acc = PACK_INT32(ts->nargs);
    FAST_DISPATCH();

    TARGET(LOAD_INT) {
        acc = PACK_INT32(opD);
        FAST_DISPATCH();
    }

    TARGET(TEST_LESS_THAN) {
        // is register less than accumulator
        Register r = regs[opA];
        if (IS_INT32(acc) && IS_INT32(r)) {
            // FIXME: to boolean?
            acc = PACK_BOOL(r.as_int64 < acc.as_int64);
        }
        else {
            CALL_VM(acc = vm_compare(r, acc));
        }
        FAST_DISPATCH();
    }

    TARGET(JUMP_IF_FALSE) {
        if (_PY_LIKELY(IS_PRI(acc))) {
            if ((AS_PRI(acc) & PRI_TRUE) == 0) {
                next_instr += opD - 0x8000;
                FAST_DISPATCH();
            }
            else {
                FAST_DISPATCH();
            }
        }
        else {
            CALL_VM(acc = vm_to_bool(acc));
            if ((AS_PRI(acc) & PRI_TRUE) == 0) {
                opD = next_instr[-1] >> 16;
                next_instr += opD - 0x8000;
            }
            FAST_DISPATCH();
        }
    }

    TARGET(FUNC_HEADER) {
        // opA contains framesize
        // acc contains nargs from call
        // FIXME: this_func is this_code object
        PyFunc *this_func = (PyFunc *)AS_OBJ(regs[-2]);
        constants = this_func->constants;
        ts->regs = regs;
        if (UNLIKELY(regs + opA > ts->maxstack)) {
            // resize stack
            CALL_VM(vm_resize_stack(ts, opA));
            // todo: check for errors!
        }
        Py_ssize_t nargs = AS_INT32(acc);
        if (UNLIKELY(nargs != this_func->nargs)) {
            // error!
            // well... we might have set-up a try-catch, so we can't just return
            ts->regs = regs;
            return vm_args_error(ts);
        }
        FAST_DISPATCH();
    }

    TARGET(CALL_FUNCTION) {
        // opsD = nargs
        // opsA = func
        // opsA + 1 = <empty> (frame link)
        // opsA + 2 = arg0
        // opsA + 2 + opsD = argsN
        Register callable = regs[opA];
        if (UNLIKELY(!IS_OBJ(callable))) {
            ts->regs = regs;
            return vm_error_not_callable(ts);
        }
        PyFunc *func = (PyFunc *)AS_OBJ(callable);
        regs = &regs[opA + 2];
        regs[-1].as_int64 = (intptr_t)next_instr;
        acc = PACK_INT32(opD);
        next_instr = func->code;
        FAST_DISPATCH();
    }

    TARGET(RET) {
        // GET code object (?)
        PyFunc *this_func = (PyFunc *)AS_OBJ(regs[-2]);
        Register *top = &regs[this_func->nargs];
        int64_t pc = regs[-1].as_int64;
        while (top != regs - 2) {
            top--;
            Register r = *top;
            top->as_int64 = 0;
            DECREF(r);
        }
        next_instr = (const uint32_t *)pc;
        // this is the call that dispatched to us
        uint32_t call = next_instr[-1];
        intptr_t offset = (call >> 8) & 0xFF;
        regs -= offset + 2;
        ts->regs = regs;
        FAST_DISPATCH();
    }

    TARGET(LOAD_GLOBAL) {
        PyObject *name = constants[opD].obj;
        PyDictObject *globals = (PyDictObject *)THIS_FUNC()->globals;
        PyDictKeysObject *keys = globals->ma_keys;

        intptr_t guess = metadata[opA];
        guess &= keys->dk_size;
        PyDictKeyEntry *entry = &keys->dk_entries[guess];
        if (entry->me_key == name) {
            // printf("guess found\n");
            PyObject *value = entry->me_value;
            acc.obj = value;
            FAST_DISPATCH();
        }

        CALL_VM(acc = vm_load_name((PyObject *)globals, name));
        FAST_DISPATCH();
    }

    TARGET(RETURN_TO_C) {
        if (IS_OBJ(acc)) {
            acc.as_int64 &= ~REFCOUNT_TAG;
            return acc.obj;
        }
        else if (IS_INT32(acc)) {
            return PyLong_FromLong(AS_INT32(acc));
        }
        else if (IS_PRI(acc)) {
            return primitives[AS_PRI(acc)];
        }
        else {
            __builtin_unreachable();
        }
    }

    TARGET(MOVE) {
        regs[opA] = acc;
        acc.as_int64 = 0;
        FAST_DISPATCH();
    }

    TARGET(CLEAR) {
        Register r = regs[opA];
        regs[opA].as_int64 = 0;
        DECREF(r);
        FAST_DISPATCH();
    }

    // 0 1 2 3
    // 1 1 2 3

    TARGET(ADD) {
        Register a = regs[opA];
        // if (IS_INT32(acc) && IS_INT32(a)) {
        if (IS_INT32(acc) && IS_INT32(a)) {
            int32_t res;
            bool overflow = __builtin_add_overflow(AS_INT32(a), AS_INT32(acc), &res);
            if (LIKELY(!overflow)) {
                acc = PACK_INT32(res);
                FAST_DISPATCH();
            }
        }
        CALL_VM(acc = vm_add(regs[opA], acc));
        FAST_DISPATCH();
    }

    TARGET(SUB) {
        Register a = regs[opA];
        if (IS_INT32(acc) && IS_INT32(a)) {
            int32_t res;
            bool overflow = __builtin_sub_overflow(AS_INT32(a), AS_INT32(acc), &res);
            if (LIKELY(!overflow)) {
                acc = PACK_INT32(res);
                FAST_DISPATCH();
            }
        }
        CALL_VM(acc = vm_add(regs[opA], acc));
        FAST_DISPATCH();
    }

    COLD_TARGET(debug_regs) {
        __asm__ volatile (
            "# REGISTER ASSIGNMENT \n\t"
            "# opcode = %0 \n\t"
            "# opA = %1 \n\t"
            "# opD = %2 \n\t"
            "# regs = %5 \n\t"
            "# acc = %3 \n\t"
            "# next_instr = %4 \n\t"
            "# constants = %6 \n\t"
            "# ts = %7 \n\t"
            "# opcode_targets = %8 \n\t"
            "# metadata = %9 \n\t" ::
            "r" (opcode),
            "r" (opA),
            "r" (opD),
            "r" (acc),
            "r" (next_instr),
            "r" (regs),
            "r" (constants),
            "r" (ts),
            "r" (opcode_targets),
            "r" (metadata));
        FAST_DISPATCH();
    }

    __builtin_unreachable();
    // Py_RETURN_NONE;
}