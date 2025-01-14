"""functools.py - Tools for working with functions and callable objects
"""
# Python module wrapper for _functools C module
# to allow utilities written in Python to be added
# to the functools module.
# Written by Nick Coghlan <ncoghlan at gmail.com>,
# Raymond Hettinger <python at rcn.com>,
# and Łukasz Langa <lukasz at langa.pl>.
#   Copyright (C) 2006-2013 Python Software Foundation.
# See C source code for _functools credits/copyright

__all__ = ['update_wrapper', 'wraps', 'WRAPPER_ASSIGNMENTS', 'WRAPPER_UPDATES',
           'total_ordering', 'cmp_to_key', 'lru_cache', 'reduce',
           'TopologicalSorter', 'CycleError',
           'partial', 'partialmethod', 'singledispatch', 'singledispatchmethod']

from abc import get_cache_token
from collections import namedtuple
# import types, weakref  # Deferred to single_dispatch()
from reprlib import recursive_repr
from _thread import RLock

from _functools import cmp_to_key, reduce, partial, _lru_cache_wrapper


################################################################################
### update_wrapper() and wraps() decorator
################################################################################

# update_wrapper() and wraps() are tools to help write
# wrapper functions that can handle naive introspection

WRAPPER_ASSIGNMENTS = ('__module__', '__name__', '__qualname__', '__doc__',
                       '__annotations__')
WRAPPER_UPDATES = ('__dict__',)
def update_wrapper(wrapper,
                   wrapped,
                   assigned = WRAPPER_ASSIGNMENTS,
                   updated = WRAPPER_UPDATES):
    """Update a wrapper function to look like the wrapped function

       wrapper is the function to be updated
       wrapped is the original function
       assigned is a tuple naming the attributes assigned directly
       from the wrapped function to the wrapper function (defaults to
       functools.WRAPPER_ASSIGNMENTS)
       updated is a tuple naming the attributes of the wrapper that
       are updated with the corresponding attribute from the wrapped
       function (defaults to functools.WRAPPER_UPDATES)
    """
    for attr in assigned:
        try:
            value = getattr(wrapped, attr)
        except AttributeError:
            pass
        else:
            setattr(wrapper, attr, value)
    for attr in updated:
        getattr(wrapper, attr).update(getattr(wrapped, attr, {}))
    # Issue #17482: set __wrapped__ last so we don't inadvertently copy it
    # from the wrapped function when updating __dict__
    wrapper.__wrapped__ = wrapped
    # Return the wrapper so this can be used as a decorator via partial()
    return wrapper

def wraps(wrapped,
          assigned = WRAPPER_ASSIGNMENTS,
          updated = WRAPPER_UPDATES):
    """Decorator factory to apply update_wrapper() to a wrapper function

       Returns a decorator that invokes update_wrapper() with the decorated
       function as the wrapper argument and the arguments to wraps() as the
       remaining arguments. Default arguments are as for update_wrapper().
       This is a convenience function to simplify applying partial() to
       update_wrapper().
    """
    return partial(update_wrapper, wrapped=wrapped,
                   assigned=assigned, updated=updated)


################################################################################
### total_ordering class decorator
################################################################################

# The total ordering functions all invoke the root magic method directly
# rather than using the corresponding operator.  This avoids possible
# infinite recursion that could occur when the operator dispatch logic
# detects a NotImplemented result and then calls a reflected method.

def _gt_from_lt(self, other, NotImplemented=NotImplemented):
    'Return a > b.  Computed by @total_ordering from (not a < b) and (a != b).'
    op_result = self.__lt__(other)
    if op_result is NotImplemented:
        return op_result
    return not op_result and self != other

def _le_from_lt(self, other, NotImplemented=NotImplemented):
    'Return a <= b.  Computed by @total_ordering from (a < b) or (a == b).'
    op_result = self.__lt__(other)
    return op_result or self == other

def _ge_from_lt(self, other, NotImplemented=NotImplemented):
    'Return a >= b.  Computed by @total_ordering from (not a < b).'
    op_result = self.__lt__(other)
    if op_result is NotImplemented:
        return op_result
    return not op_result

def _ge_from_le(self, other, NotImplemented=NotImplemented):
    'Return a >= b.  Computed by @total_ordering from (not a <= b) or (a == b).'
    op_result = self.__le__(other)
    if op_result is NotImplemented:
        return op_result
    return not op_result or self == other

def _lt_from_le(self, other, NotImplemented=NotImplemented):
    'Return a < b.  Computed by @total_ordering from (a <= b) and (a != b).'
    op_result = self.__le__(other)
    if op_result is NotImplemented:
        return op_result
    return op_result and self != other

def _gt_from_le(self, other, NotImplemented=NotImplemented):
    'Return a > b.  Computed by @total_ordering from (not a <= b).'
    op_result = self.__le__(other)
    if op_result is NotImplemented:
        return op_result
    return not op_result

def _lt_from_gt(self, other, NotImplemented=NotImplemented):
    'Return a < b.  Computed by @total_ordering from (not a > b) and (a != b).'
    op_result = self.__gt__(other)
    if op_result is NotImplemented:
        return op_result
    return not op_result and self != other

def _ge_from_gt(self, other, NotImplemented=NotImplemented):
    'Return a >= b.  Computed by @total_ordering from (a > b) or (a == b).'
    op_result = self.__gt__(other)
    return op_result or self == other

def _le_from_gt(self, other, NotImplemented=NotImplemented):
    'Return a <= b.  Computed by @total_ordering from (not a > b).'
    op_result = self.__gt__(other)
    if op_result is NotImplemented:
        return op_result
    return not op_result

def _le_from_ge(self, other, NotImplemented=NotImplemented):
    'Return a <= b.  Computed by @total_ordering from (not a >= b) or (a == b).'
    op_result = self.__ge__(other)
    if op_result is NotImplemented:
        return op_result
    return not op_result or self == other

def _gt_from_ge(self, other, NotImplemented=NotImplemented):
    'Return a > b.  Computed by @total_ordering from (a >= b) and (a != b).'
    op_result = self.__ge__(other)
    if op_result is NotImplemented:
        return op_result
    return op_result and self != other

def _lt_from_ge(self, other, NotImplemented=NotImplemented):
    'Return a < b.  Computed by @total_ordering from (not a >= b).'
    op_result = self.__ge__(other)
    if op_result is NotImplemented:
        return op_result
    return not op_result

_convert = {
    '__lt__': [('__gt__', _gt_from_lt),
               ('__le__', _le_from_lt),
               ('__ge__', _ge_from_lt)],
    '__le__': [('__ge__', _ge_from_le),
               ('__lt__', _lt_from_le),
               ('__gt__', _gt_from_le)],
    '__gt__': [('__lt__', _lt_from_gt),
               ('__ge__', _ge_from_gt),
               ('__le__', _le_from_gt)],
    '__ge__': [('__le__', _le_from_ge),
               ('__gt__', _gt_from_ge),
               ('__lt__', _lt_from_ge)]
}

def total_ordering(cls):
    """Class decorator that fills in missing ordering methods"""
    # Find user-defined comparisons (not those inherited from object).
    roots = {op for op in _convert if getattr(cls, op, None) is not getattr(object, op, None)}
    if not roots:
        raise ValueError('must define at least one ordering operation: < > <= >=')
    root = max(roots)       # prefer __lt__ to __le__ to __gt__ to __ge__
    for opname, opfunc in _convert[root]:
        if opname not in roots:
            opfunc.__name__ = opname
            setattr(cls, opname, opfunc)
    return cls

################################################################################
### topological sort
################################################################################

_NODE_OUT = -1
_NODE_DONE = -2


class _NodeInfo:
    __slots__ = 'node', 'npredecessors', 'successors'

    def __init__(self, node):
        # The node this class is augmenting.
        self.node = node

        # Number of predecessors, generally >= 0. When this value falls to 0,
        # and is returned by get_ready(), this is set to _NODE_OUT and when the
        # node is marked done by a call to done(), set to _NODE_DONE.
        self.npredecessors = 0

        # List of successor nodes. The list can contain duplicated elements as
        # long as they're all reflected in the successor's npredecessors attribute).
        self.successors = []


class CycleError(ValueError):
    """Subclass of ValueError raised by TopologicalSorterif cycles exist in the graph

    If multiple cycles exist, only one undefined choice among them will be reported
    and included in the exception. The detected cycle can be accessed via the second
    element in the *args* attribute of the exception instance and consists in a list
    of nodes, such that each node is, in the graph, an immediate predecessor of the
    next node in the list. In the reported list, the first and the last node will be
    the same, to make it clear that it is cyclic.
    """
    pass


class TopologicalSorter:
    """Provides functionality to topologically sort a graph of hashable nodes"""

    def __init__(self, graph=None):
        self._node2info = {}
        self._ready_nodes = None
        self._npassedout = 0
        self._nfinished = 0

        if graph is not None:
            for node, predecessors in graph.items():
                self.add(node, *predecessors)

    def _get_nodeinfo(self, node):
        if (result := self._node2info.get(node)) is None:
            self._node2info[node] = result = _NodeInfo(node)
        return result

    def add(self, node, *predecessors):
        """Add a new node and its predecessors to the graph.

        Both the *node* and all elements in *predecessors* must be hashable.

        If called multiple times with the same node argument, the set of dependencies
        will be the union of all dependencies passed in.

        It is possible to add a node with no dependencies (*predecessors* is not provided)
        as well as provide a dependency twice. If a node that has not been provided before
        is included among *predecessors* it will be automatically added to the graph with
        no predecessors of its own.

        Raises ValueError if called after "prepare".
        """
        if self._ready_nodes is not None:
            raise ValueError("Nodes cannot be added after a call to prepare()")

        # Create the node -> predecessor edges
        nodeinfo = self._get_nodeinfo(node)
        nodeinfo.npredecessors += len(predecessors)

        # Create the predecessor -> node edges
        for pred in predecessors:
            pred_info = self._get_nodeinfo(pred)
            pred_info.successors.append(node)

    def prepare(self):
        """Mark the graph as finished and check for cycles in the graph.

        If any cycle is detected, "CycleError" will be raised, but "get_ready" can
        still be used to obtain as many nodes as possible until cycles block more
        progress. After a call to this function, the graph cannot be modified and
        therefore no more nodes can be added using "add".
        """
        if self._ready_nodes is not None:
            raise ValueError("cannot prepare() more than once")

        self._ready_nodes = [i.node for i in self._node2info.values()
                             if i.npredecessors == 0]
        # ready_nodes is set before we look for cycles on purpose:
        # if the user wants to catch the CycleError, that's fine,
        # they can continue using the instance to grab as many
        # nodes as possible before cycles block more progress
        cycle = self._find_cycle()
        if cycle:
            raise CycleError(f"nodes are in a cycle", cycle)

    def get_ready(self):
        """Return a tuple of all the nodes that are ready.

        Initially it returns all nodes with no predecessors; once those are marked
        as processed by calling "done", further calls will return all new nodes that
        have all their predecessors already processed. Once no more progress can be made,
        empty tuples are returned.

        Raises ValueError if called without calling "prepare" previously.
        """
        if self._ready_nodes is None:
            raise ValueError("prepare() must be called first")

        # Get the nodes that are ready and mark them
        result = tuple(self._ready_nodes)
        n2i = self._node2info
        for node in result:
            n2i[node].npredecessors = _NODE_OUT

        # Clean the list of nodes that are ready and update
        # the counter of nodes that we have returned.
        self._ready_nodes.clear()
        self._npassedout += len(result)

        return result

    def is_active(self):
        """Return True if more progress can be made and ``False`` otherwise.

        Progress can be made if cycles do not block the resolution and either there
        are still nodes ready that haven't yet been returned by "get_ready" or the
        number of nodes marked "done" is less than the number that have been returned
        by "get_ready".

        Raises ValueError if called without calling "prepare" previously.
        """
        if self._ready_nodes is None:
            raise ValueError("prepare() must be called first")
        return self._nfinished < self._npassedout or bool(self._ready_nodes)

    def __bool__(self):
        return self.is_active()

    def done(self, *nodes):
        """Marks a set of nodes returned by "get_ready" as processed.

        This method unblocks any successor of each node in *nodes* for being returned
        in the future by a a call to "get_ready"

        Raises :exec:`ValueError` if any node in *nodes* has already been marked as
        processed by a previous call to this method, if a node was not added to the
        graph by using "add" or if called without calling "prepare" previously or if
        node has not yet been returned by "get_ready".
        """

        if self._ready_nodes is None:
            raise ValueError("prepare() must be called first")

        n2i = self._node2info

        for node in nodes:

            # Check if we know about this node (it was added previously using add()
            if (nodeinfo := n2i.get(node)) is None:
                raise ValueError(f"node {node!r} was not added using add()")

            # If the node has not being returned (marked as ready) previously, inform the user.
            stat = nodeinfo.npredecessors
            if stat != _NODE_OUT:
                if stat >= 0:
                    raise ValueError(f"node {node!r} was not passed out (still not ready)")
                elif stat == _NODE_DONE:
                    raise ValueError(f"node {node!r} was already marked done")
                else:
                    assert False, f"node {node!r}: unknown status {stat}"

            # Mark the node as processed
            nodeinfo.npredecessors = _NODE_DONE

            # Go to all the successors and reduce the number of predecessors, collecting all the ones
            # that are ready to be returned in the next get_ready() call.
            for successor in nodeinfo.successors:
                successor_info = n2i[successor]
                successor_info.npredecessors -= 1
                if successor_info.npredecessors == 0:
                    self._ready_nodes.append(successor)
            self._nfinished += 1

    def _find_cycle(self):
        n2i = self._node2info
        stack = []
        itstack = []
        seen = set()
        node2stacki = {}

        for node in n2i:
            if node in seen:
                continue

            while True:
                if node in seen:
                    # If we have seen already the node and is in the
                    # current stack we have found a cycle.
                    if node in node2stacki:
                        return stack[node2stacki[node]:] + [node]
                    # else go on to get next successor
                else:
                    seen.add(node)
                    itstack.append(iter(n2i[node].successors).__next__)
                    node2stacki[node] = len(stack)
                    stack.append(node)

                # Backtrack to the topmost stack entry with
                # at least another successor.
                while stack:
                    try:
                        node = itstack[-1]()
                        break
                    except StopIteration:
                        del node2stacki[stack.pop()]
                        itstack.pop()
                else:
                    break
        return None

    def static_order(self):
        """Returns an iterable of nodes in a topological order.

        The particular order that is returned may depend on the specific
        order in which the items were inserted in the graph.

        Using this method does not require to call "prepare" or "done". If any
        cycle is detected, :exc:`CycleError` will be raised.
        """
        self.prepare()
        while self.is_active():
            node_group = self.get_ready()
            yield from node_group
            self.done(*node_group)


# Descriptor version
class partialmethod(object):
    """Method descriptor with partial application of the given arguments
    and keywords.

    Supports wrapping existing descriptors and handles non-descriptor
    callables as instance methods.
    """

    def __init__(self, func, /, *args, **keywords):
        if not callable(func) and not hasattr(func, "__get__"):
            raise TypeError("{!r} is not callable or a descriptor"
                                 .format(func))

        # func could be a descriptor like classmethod which isn't callable,
        # so we can't inherit from partial (it verifies func is callable)
        if isinstance(func, partialmethod):
            # flattening is mandatory in order to place cls/self before all
            # other arguments
            # it's also more efficient since only one function will be called
            self.func = func.func
            self.args = func.args + args
            self.keywords = {**func.keywords, **keywords}
        else:
            self.func = func
            self.args = args
            self.keywords = keywords

    def __repr__(self):
        args = ", ".join(map(repr, self.args))
        keywords = ", ".join("{}={!r}".format(k, v)
                                 for k, v in self.keywords.items())
        format_string = "{module}.{cls}({func}, {args}, {keywords})"
        return format_string.format(module=self.__class__.__module__,
                                    cls=self.__class__.__qualname__,
                                    func=self.func,
                                    args=args,
                                    keywords=keywords)

    def _make_unbound_method(self):
        def _method(cls_or_self, /, *args, **keywords):
            keywords = {**self.keywords, **keywords}
            return self.func(cls_or_self, *self.args, *args, **keywords)
        _method.__isabstractmethod__ = self.__isabstractmethod__
        _method._partialmethod = self
        return _method

    def __get__(self, obj, cls=None):
        get = getattr(self.func, "__get__", None)
        result = None
        if get is not None:
            new_func = get(obj, cls)
            if new_func is not self.func:
                # Assume __get__ returning something new indicates the
                # creation of an appropriate callable
                result = partial(new_func, *self.args, **self.keywords)
                try:
                    result.__self__ = new_func.__self__
                except AttributeError:
                    pass
        if result is None:
            # If the underlying descriptor didn't do anything, treat this
            # like an instance method
            result = self._make_unbound_method().__get__(obj, cls)
        return result

    @property
    def __isabstractmethod__(self):
        return getattr(self.func, "__isabstractmethod__", False)

# Helper functions

def _unwrap_partial(func):
    while isinstance(func, partial):
        func = func.func
    return func

################################################################################
### LRU Cache function decorator
################################################################################

_CacheInfo = namedtuple("CacheInfo", ["hits", "misses", "maxsize", "currsize"])

class _HashedSeq(list):
    """ This class guarantees that hash() will be called no more than once
        per element.  This is important because the lru_cache() will hash
        the key multiple times on a cache miss.

    """

    __slots__ = 'hashvalue'

    def __init__(self, tup, hash=hash):
        self[:] = tup
        self.hashvalue = hash(tup)

    def __hash__(self):
        return self.hashvalue

def _make_key(args, kwds, typed,
             kwd_mark = (object(),),
             fasttypes = {int, str},
             tuple=tuple, type=type, len=len):
    """Make a cache key from optionally typed positional and keyword arguments

    The key is constructed in a way that is flat as possible rather than
    as a nested structure that would take more memory.

    If there is only a single argument and its data type is known to cache
    its hash value, then that argument is returned without a wrapper.  This
    saves space and improves lookup speed.

    """
    # All of code below relies on kwds preserving the order input by the user.
    # Formerly, we sorted() the kwds before looping.  The new way is *much*
    # faster; however, it means that f(x=1, y=2) will now be treated as a
    # distinct call from f(y=2, x=1) which will be cached separately.
    key = args
    if kwds:
        key += kwd_mark
        for item in kwds.items():
            key += item
    if typed:
        key += tuple(type(v) for v in args)
        if kwds:
            key += tuple(type(v) for v in kwds.values())
    elif len(key) == 1 and type(key[0]) in fasttypes:
        return key[0]
    return _HashedSeq(key)

def lru_cache(maxsize=128, typed=False):
    """Least-recently-used cache decorator.

    If *maxsize* is set to None, the LRU features are disabled and the cache
    can grow without bound.

    If *typed* is True, arguments of different types will be cached separately.
    For example, f(3.0) and f(3) will be treated as distinct calls with
    distinct results.

    Arguments to the cached function must be hashable.

    View the cache statistics named tuple (hits, misses, maxsize, currsize)
    with f.cache_info().  Clear the cache and statistics with f.cache_clear().
    Access the underlying function with f.__wrapped__.

    See:  http://en.wikipedia.org/wiki/Cache_replacement_policies#Least_recently_used_(LRU)

    """

    # Users should only access the lru_cache through its public API:
    #       cache_info, cache_clear, and f.__wrapped__
    # The internals of the lru_cache are encapsulated for thread safety and
    # to allow the implementation to change (including a possible C version).

    if isinstance(maxsize, int):
        # Negative maxsize is treated as 0
        if maxsize < 0:
            maxsize = 0
    elif callable(maxsize) and isinstance(typed, bool):
        # The user_function was passed in directly via the maxsize argument
        user_function, maxsize = maxsize, 128
        wrapper = _lru_cache_wrapper(user_function, maxsize, typed, _CacheInfo)
        wrapper.cache_parameters = lambda : {'maxsize': maxsize, 'typed': typed}
        return update_wrapper(wrapper, user_function)
    elif maxsize is not None:
        raise TypeError(
            'Expected first argument to be an integer, a callable, or None')

    def decorating_function(user_function):
        wrapper = _lru_cache_wrapper(user_function, maxsize, typed, _CacheInfo)
        wrapper.cache_parameters = lambda : {'maxsize': maxsize, 'typed': typed}
        return update_wrapper(wrapper, user_function)

    return decorating_function


################################################################################
### singledispatch() - single-dispatch generic function decorator
################################################################################

def _c3_merge(sequences):
    """Merges MROs in *sequences* to a single MRO using the C3 algorithm.

    Adapted from http://www.python.org/download/releases/2.3/mro/.

    """
    result = []
    while True:
        sequences = [s for s in sequences if s]   # purge empty sequences
        if not sequences:
            return result
        for s1 in sequences:   # find merge candidates among seq heads
            candidate = s1[0]
            for s2 in sequences:
                if candidate in s2[1:]:
                    candidate = None
                    break      # reject the current head, it appears later
            else:
                break
        if candidate is None:
            raise RuntimeError("Inconsistent hierarchy")
        result.append(candidate)
        # remove the chosen candidate
        for seq in sequences:
            if seq[0] == candidate:
                del seq[0]

def _c3_mro(cls, abcs=None):
    """Computes the method resolution order using extended C3 linearization.

    If no *abcs* are given, the algorithm works exactly like the built-in C3
    linearization used for method resolution.

    If given, *abcs* is a list of abstract base classes that should be inserted
    into the resulting MRO. Unrelated ABCs are ignored and don't end up in the
    result. The algorithm inserts ABCs where their functionality is introduced,
    i.e. issubclass(cls, abc) returns True for the class itself but returns
    False for all its direct base classes. Implicit ABCs for a given class
    (either registered or inferred from the presence of a special method like
    __len__) are inserted directly after the last ABC explicitly listed in the
    MRO of said class. If two implicit ABCs end up next to each other in the
    resulting MRO, their ordering depends on the order of types in *abcs*.

    """
    for i, base in enumerate(reversed(cls.__bases__)):
        if hasattr(base, '__abstractmethods__'):
            boundary = len(cls.__bases__) - i
            break   # Bases up to the last explicit ABC are considered first.
    else:
        boundary = 0
    abcs = list(abcs) if abcs else []
    explicit_bases = list(cls.__bases__[:boundary])
    abstract_bases = []
    other_bases = list(cls.__bases__[boundary:])
    for base in abcs:
        if issubclass(cls, base) and not any(
                issubclass(b, base) for b in cls.__bases__
            ):
            # If *cls* is the class that introduces behaviour described by
            # an ABC *base*, insert said ABC to its MRO.
            abstract_bases.append(base)
    for base in abstract_bases:
        abcs.remove(base)
    explicit_c3_mros = [_c3_mro(base, abcs=abcs) for base in explicit_bases]
    abstract_c3_mros = [_c3_mro(base, abcs=abcs) for base in abstract_bases]
    other_c3_mros = [_c3_mro(base, abcs=abcs) for base in other_bases]
    return _c3_merge(
        [[cls]] +
        explicit_c3_mros + abstract_c3_mros + other_c3_mros +
        [explicit_bases] + [abstract_bases] + [other_bases]
    )

def _compose_mro(cls, types):
    """Calculates the method resolution order for a given class *cls*.

    Includes relevant abstract base classes (with their respective bases) from
    the *types* iterable. Uses a modified C3 linearization algorithm.

    """
    bases = set(cls.__mro__)
    # Remove entries which are already present in the __mro__ or unrelated.
    def is_related(typ):
        return (typ not in bases and hasattr(typ, '__mro__')
                                 and issubclass(cls, typ))
    types = [n for n in types if is_related(n)]
    # Remove entries which are strict bases of other entries (they will end up
    # in the MRO anyway.
    def is_strict_base(typ):
        for other in types:
            if typ != other and typ in other.__mro__:
                return True
        return False
    types = [n for n in types if not is_strict_base(n)]
    # Subclasses of the ABCs in *types* which are also implemented by
    # *cls* can be used to stabilize ABC ordering.
    type_set = set(types)
    mro = []
    for typ in types:
        found = []
        for sub in typ.__subclasses__():
            if sub not in bases and issubclass(cls, sub):
                found.append([s for s in sub.__mro__ if s in type_set])
        if not found:
            mro.append(typ)
            continue
        # Favor subclasses with the biggest number of useful bases
        found.sort(key=len, reverse=True)
        for sub in found:
            for subcls in sub:
                if subcls not in mro:
                    mro.append(subcls)
    return _c3_mro(cls, abcs=mro)

def _find_impl(cls, registry):
    """Returns the best matching implementation from *registry* for type *cls*.

    Where there is no registered implementation for a specific type, its method
    resolution order is used to find a more generic implementation.

    Note: if *registry* does not contain an implementation for the base
    *object* type, this function may return None.

    """
    mro = _compose_mro(cls, registry.keys())
    match = None
    for t in mro:
        if match is not None:
            # If *match* is an implicit ABC but there is another unrelated,
            # equally matching implicit ABC, refuse the temptation to guess.
            if (t in registry and t not in cls.__mro__
                              and match not in cls.__mro__
                              and not issubclass(match, t)):
                raise RuntimeError("Ambiguous dispatch: {} or {}".format(
                    match, t))
            break
        if t in registry:
            match = t
    return registry.get(match)

def singledispatch(func):
    """Single-dispatch generic function decorator.

    Transforms a function into a generic function, which can have different
    behaviours depending upon the type of its first argument. The decorated
    function acts as the default implementation, and additional
    implementations can be registered using the register() attribute of the
    generic function.
    """
    # There are many programs that use functools without singledispatch, so we
    # trade-off making singledispatch marginally slower for the benefit of
    # making start-up of such applications slightly faster.
    import types, weakref

    registry = {}
    dispatch_cache = weakref.WeakKeyDictionary()
    cache_token = None

    def dispatch(cls):
        """generic_func.dispatch(cls) -> <function implementation>

        Runs the dispatch algorithm to return the best available implementation
        for the given *cls* registered on *generic_func*.

        """
        nonlocal cache_token
        if cache_token is not None:
            current_token = get_cache_token()
            if cache_token != current_token:
                dispatch_cache.clear()
                cache_token = current_token
        try:
            impl = dispatch_cache[cls]
        except KeyError:
            try:
                impl = registry[cls]
            except KeyError:
                impl = _find_impl(cls, registry)
            dispatch_cache[cls] = impl
        return impl

    def register(cls, func=None):
        """generic_func.register(cls, func) -> func

        Registers a new implementation for the given *cls* on a *generic_func*.

        """
        nonlocal cache_token
        if func is None:
            if isinstance(cls, type):
                return lambda f: register(cls, f)
            ann = getattr(cls, '__annotations__', {})
            if not ann:
                raise TypeError(
                    f"Invalid first argument to `register()`: {cls!r}. "
                    f"Use either `@register(some_class)` or plain `@register` "
                    f"on an annotated function."
                )
            func = cls

            # only import typing if annotation parsing is necessary
            from typing import get_type_hints
            argname, cls = next(iter(get_type_hints(func).items()))
            if not isinstance(cls, type):
                raise TypeError(
                    f"Invalid annotation for {argname!r}. "
                    f"{cls!r} is not a class."
                )
        registry[cls] = func
        if cache_token is None and hasattr(cls, '__abstractmethods__'):
            cache_token = get_cache_token()
        dispatch_cache.clear()
        return func

    def wrapper(*args, **kw):
        if not args:
            raise TypeError(f'{funcname} requires at least '
                            '1 positional argument')

        return dispatch(args[0].__class__)(*args, **kw)

    funcname = getattr(func, '__name__', 'singledispatch function')
    registry[object] = func
    wrapper.register = register
    wrapper.dispatch = dispatch
    wrapper.registry = types.MappingProxyType(registry)
    wrapper._clear_cache = dispatch_cache.clear
    update_wrapper(wrapper, func)
    return wrapper


# Descriptor version
class singledispatchmethod:
    """Single-dispatch generic method descriptor.

    Supports wrapping existing descriptors and handles non-descriptor
    callables as instance methods.
    """

    def __init__(self, func):
        if not callable(func) and not hasattr(func, "__get__"):
            raise TypeError(f"{func!r} is not callable or a descriptor")

        self.dispatcher = singledispatch(func)
        self.func = func

    def register(self, cls, method=None):
        """generic_method.register(cls, func) -> func

        Registers a new implementation for the given *cls* on a *generic_method*.
        """
        return self.dispatcher.register(cls, func=method)

    def __get__(self, obj, cls=None):
        def _method(*args, **kwargs):
            method = self.dispatcher.dispatch(args[0].__class__)
            return method.__get__(obj, cls)(*args, **kwargs)

        _method.__isabstractmethod__ = self.__isabstractmethod__
        _method.register = self.register
        update_wrapper(_method, self.func)
        return _method

    @property
    def __isabstractmethod__(self):
        return getattr(self.func, '__isabstractmethod__', False)


################################################################################
### cached_property() - computed once per instance, cached as attribute
################################################################################

_NOT_FOUND = object()


class cached_property:
    def __init__(self, func):
        self.func = func
        self.attrname = None
        self.__doc__ = func.__doc__
        self.lock = RLock()

    def __set_name__(self, owner, name):
        if self.attrname is None:
            self.attrname = name
        elif name != self.attrname:
            raise TypeError(
                "Cannot assign the same cached_property to two different names "
                f"({self.attrname!r} and {name!r})."
            )

    def __get__(self, instance, owner=None):
        if instance is None:
            return self
        if self.attrname is None:
            raise TypeError(
                "Cannot use cached_property instance without calling __set_name__ on it.")
        try:
            cache = instance.__dict__
        except AttributeError:  # not all objects have __dict__ (e.g. class defines slots)
            msg = (
                f"No '__dict__' attribute on {type(instance).__name__!r} "
                f"instance to cache {self.attrname!r} property."
            )
            raise TypeError(msg) from None
        val = cache.get(self.attrname, _NOT_FOUND)
        if val is _NOT_FOUND:
            with self.lock:
                # check if another thread filled cache while we awaited lock
                val = cache.get(self.attrname, _NOT_FOUND)
                if val is _NOT_FOUND:
                    val = self.func(instance)
                    try:
                        cache[self.attrname] = val
                    except TypeError:
                        msg = (
                            f"The '__dict__' attribute on {type(instance).__name__!r} instance "
                            f"does not support item assignment for caching {self.attrname!r} property."
                        )
                        raise TypeError(msg) from None
        return val
