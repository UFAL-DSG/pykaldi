cimport sym
cimport libfst


cdef class _Fst:
    pass 

cdef class SymbolTable:
    cdef sym.SymbolTable* table

{{#types}}

cdef class {{fst}}(_Fst):
    cdef libfst.{{fst}}* fst
    cdef public SymbolTable isyms, osyms

{{/types}}
