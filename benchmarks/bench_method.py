import time
import math as _math
from dataclasses import dataclass

UNROLL = 100
N = 100000

# old = 779 ns

@dataclass
class Foo:
    value: int

    def meth(self):
        pass

global_f = Foo(3)

def foo():
    f = global_f
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()
    f.meth()

def cfunc_o():
    math = _math
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)
    math.isnan(0.0)

def cmethod_o():
    x = []
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)
    x.count(0)

def run():
    for _ in range(3):
        start = time.perf_counter()
        for _ in range(N):
            foo()
        end = time.perf_counter()
        print('pymethod', (end - start) * 1e9/(UNROLL * N), 'ns')

        start = time.perf_counter()
        for _ in range(N):
            cfunc_o()
        end = time.perf_counter()
        print('cfunc_o', (end - start) * 1e9/(UNROLL * N), 'ns')

        start = time.perf_counter()
        for _ in range(N):
            cmethod_o()
        end = time.perf_counter()
        print('cmethod_o', (end - start) * 1e9/(UNROLL * N), 'ns')

run()

# import dis
# dis.dis(cfunc_o)
# dis.dis(run)

