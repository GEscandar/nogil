#include <stdbool.h>

#include "Python.h"
#include "code2.h"
#include "opcode2.h"
#include "structmember.h"
#include "pycore_code.h"
#include "pycore_object.h"
#include "pycore_gc.h"
#include "pycore_pystate.h"
#include "pycore_tupleobject.h"

// An individual register can have an owning or non-owning reference
// Deferred and immortal objects always have non-owning references (immortal for correctness, deferred for perf, helps)
// A regular object *may* have a non-owning reference for aliases

// alias       unowned
// not-alias   unowned|owned

// Function arguments may or may not be aliases
// Function return values are *never* aliases (?)

// x = object()
// y = x       # must not be an alias (!) (because x might change)
// foo(x, x)   # can be aliases!


// The debugger needs to make all aliases into non-aliases (?)

// So temporaries can be aliases
// Arguments can be aliases
// Assignemnts to named variables should not be aliases
// Return values should not be aliases: this allows:

//  y = foo()

// CALL(...)
// MOV(y, acc)  # transfer ownership



// Error handling
// Walk the stack
// Free variables
// Find nearest exception handler
// Jump?

// WebKit returns PC from every op
// There's an error handler PC for every instruction size inline so no matter the advance still reading/jumping to error handler
// WebKit writes result directly to destination register


// In Python, any function that can call arbitrary code (most):
// - can resize stack (ts->regs)
// - can raise an exception

/*[clinic input]
module _code2
class code "PyCodeObject2 *" "&PyCode2_Type"
[clinic start generated code]*/
/*[clinic end generated code: output=da39a3ee5e6b4b0d input=96be18a1b345cf3c]*/

#include "clinic/codeobject2.c.h"


PyCodeObject2 *
PyCode2_New(PyObject *bytecode, PyObject *consts)
{
    Py_ssize_t ninstrs = PyBytes_Size(bytecode) / sizeof(uint32_t);
    Py_ssize_t nconsts = PyTuple_GET_SIZE(consts);
    Py_ssize_t size = (sizeof(PyCodeObject2) +
                       ninstrs * sizeof(uint32_t) +
                       nconsts * sizeof(PyObject *));

    PyCodeObject2 *co = (PyCodeObject2 *)_PyObject_GC_Malloc(size);
    if (co == NULL) {
        return NULL;
    }
    memset(co, 0, sizeof(PyCodeObject2));
    PyObject_INIT(co, &PyCode2_Type);
    co->co_size = ninstrs;
    co->co_nconsts = nconsts;

    uint32_t *instrs = PyCode2_Code((PyObject *)co);
    memcpy(instrs, PyBytes_AsString(bytecode), ninstrs * sizeof(uint32_t));

    PyObject **co_constants = (PyObject **)((char *)co +
            sizeof(PyCodeObject2) + ninstrs * sizeof(uint32_t));
    co->co_constants = co_constants;
    for (Py_ssize_t i = 0; i != nconsts; i++) {
        PyObject *c = PyTuple_GET_ITEM(consts, i);
        Py_INCREF(c);
        co_constants[i] = c;
    }

    return co;
}

PyDoc_STRVAR(code_doc,
"code(???, argcount, posonlyargcount, kwonlyargcount, nlocals, stacksize,\n\
      flags, codestring, constants, names, varnames, filename, name,\n\
      firstlineno, lnotab[, freevars[, cellvars]])\n\
\n\
Create a code object.  Not for the faint of heart.");

/*[clinic input]
@classmethod
code.__new__ as code_new
    bytecode: object(subclass_of="&PyBytes_Type")
    constants as consts: object(subclass_of="&PyTuple_Type")
    argcount: int = 0
    posonlyargcount: int = 0
    kwonlyargcount: int = 0
    nlocals: int = 0
    framesize: int = 0
    flags: int = 0
    names: object(subclass_of="&PyTuple_Type", c_default="NULL") = ()
    varnames: object(subclass_of="&PyTuple_Type", c_default="NULL") = ()
    filename: unicode = None
    name: unicode = None
    firstlineno: int = 0
    linetable: object(subclass_of="&PyBytes_Type") = None
    freevars: object(subclass_of="&PyTuple_Type", c_default="NULL") = ()
    cellvars: object(subclass_of="&PyTuple_Type", c_default="NULL") = ()

Create a code object.  Not for the faint of heart.
[clinic start generated code]*/

static PyObject *
code_new_impl(PyTypeObject *type, PyObject *bytecode, PyObject *consts,
              int argcount, int posonlyargcount, int kwonlyargcount,
              int nlocals, int framesize, int flags, PyObject *names,
              PyObject *varnames, PyObject *filename, PyObject *name,
              int firstlineno, PyObject *linetable, PyObject *freevars,
              PyObject *cellvars)
/*[clinic end generated code: output=dc5152b0ba2545cc input=7cb9e81b878b5b9d]*/
{
    PyCodeObject2 *co = PyCode2_New(bytecode, consts);
    if (co == NULL) {
        return NULL;
    }
    co->co_argcount = argcount;
    Py_XINCREF(varnames);
    co->co_varnames = varnames;
    Py_XINCREF(freevars);
    co->co_freevars = freevars;
    Py_XINCREF(cellvars);
    co->co_cellvars = cellvars;
    Py_INCREF(filename);
    co->co_filename = filename;
    Py_INCREF(name);
    co->co_name = name;
    co->co_firstlineno = firstlineno;
    Py_INCREF(linetable);
    co->co_lnotab = linetable;
    return (PyObject *)co;
}

static void
code_dealloc(PyCodeObject2 *co)
{
    PyObject_GC_UnTrack(co);

    PyObject **consts = co->co_constants;
    Py_ssize_t nconsts = co->co_nconsts;
    for (Py_ssize_t i = 0; i != nconsts; i++) {
        Py_DECREF(consts[i]);
    }

    Py_XDECREF(co->co_varnames);
    Py_XDECREF(co->co_freevars);
    Py_XDECREF(co->co_cellvars);
}

static PyObject *
code_repr(PyCodeObject2 *co)
{
    int lineno;
    if (co->co_firstlineno != 0)
        lineno = co->co_firstlineno;
    else
        lineno = -1;
    if (co->co_filename && PyUnicode_Check(co->co_filename)) {
        return PyUnicode_FromFormat(
            "<code2 object %U at %p, file \"%U\", line %d>",
            co->co_name, co, co->co_filename, lineno);
    } else {
        return PyUnicode_FromFormat(
            "<code2 object %U at %p, file ???, line %d>",
            co->co_name, co, lineno);
    }
}

static Py_hash_t
code_hash(PyCodeObject2 *co)
{
    return 7;
}

static int
code_traverse(PyCodeObject2 *co, visitproc visit, void *arg)
{
    return 0;
}

static PyObject *
code_richcompare(PyObject *self, PyObject *other, int op)
{
    if ((op != Py_EQ && op != Py_NE) ||
        !PyCode_Check(self) ||
        !PyCode_Check(other)) {
        Py_RETURN_NOTIMPLEMENTED;
    }

    return (self == other) ^ (op == Py_NE) ? Py_True : Py_False;
}

static PyObject *
code_sizeof(PyCodeObject2 *co, PyObject *Py_UNUSED(args))
{
    Py_ssize_t size = sizeof(PyCodeObject);
    size += co->co_size * sizeof(uint32_t);
    size = co->co_nconsts * sizeof(PyObject *);
    return PyLong_FromSsize_t(size);
}

static PyObject *
code_getcode(PyCodeObject2 *co, PyObject *Py_UNUSED(args))
{
    uint32_t *bytecode = PyCode2_Code((PyObject *)co);
    return PyBytes_FromStringAndSize((char *)bytecode, co->co_size * sizeof(uint32_t));
}

static PyObject *
code_getconsts(PyCodeObject2 *co, PyObject *Py_UNUSED(args))
{
    PyObject *t = PyTuple_New(co->co_nconsts);
    if (t == NULL) {
        return NULL;
    }
    for (Py_ssize_t i = 0; i != co->co_nconsts; i++) {
        PyObject *c = co->co_constants[i];
        Py_INCREF(c);
        PyTuple_SET_ITEM(t, i, c);
    }
    return t;
}

static struct PyMethodDef code_methods[] = {
    {"__sizeof__", (PyCFunction)code_sizeof, METH_NOARGS},
    {NULL, NULL}                /* sentinel */
};

#define OFF(x) offsetof(PyCodeObject2, x)

static PyMemberDef code_memberlist[] = {
    {"co_varnames",     T_OBJECT,       OFF(co_varnames),        READONLY},
    {"co_freevars",     T_OBJECT,       OFF(co_freevars),        READONLY},
    {"co_cellvars",     T_OBJECT,       OFF(co_cellvars),        READONLY},
    {"co_filename",     T_OBJECT,       OFF(co_filename),        READONLY},
    {"co_name",         T_OBJECT,       OFF(co_name),            READONLY},
    {"co_firstlineno", T_INT,           OFF(co_firstlineno),     READONLY},
    {"co_lnotab",       T_OBJECT,       OFF(co_lnotab),          READONLY},
    {NULL}      /* Sentinel */
};

static PyGetSetDef code_getset[] = {
    {"co_code", (getter)code_getcode, (setter)NULL, "code bytes", NULL},
    {"co_consts", (getter)code_getconsts, (setter)NULL, "constants", NULL},
    {NULL} /* sentinel */
};

PyTypeObject PyCode2_Type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "code",
    sizeof(PyCodeObject2),
    0,
    (destructor)code_dealloc,           /* tp_dealloc */
    0,                                  /* tp_vectorcall_offset */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_as_async */
    (reprfunc)code_repr,                /* tp_repr */
    0,                                  /* tp_as_number */
    0,                                  /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    (hashfunc)code_hash,                /* tp_hash */
    0,                                  /* tp_call */
    0,                                  /* tp_str */
    PyObject_GenericGetAttr,            /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC, /* tp_flags */
    code_doc,                           /* tp_doc */
    (traverseproc)code_traverse,        /* tp_traverse */
    0,                                  /* tp_clear */
    code_richcompare,                   /* tp_richcompare */
    offsetof(PyCodeObject2, co_weakreflist),     /* tp_weaklistoffset */
    0,                                  /* tp_iter */
    0,                                  /* tp_iternext */
    code_methods,                       /* tp_methods */
    code_memberlist,                    /* tp_members */
    code_getset,                        /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    code_new,                           /* tp_new */
};
