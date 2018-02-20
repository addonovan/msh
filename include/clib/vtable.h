/*
 * Name: Austin Donovan
 * Id:   1001311620
 */

#ifndef __CLIB_VTABLE_H__
#define __CLIB_VTABLE_H__

#include "generic.h"
#include "preproc.h"

/** Generates the typename for a vtable of the given type. */
#define vtable_t(x)                                                            \
  CAT(__vtable_, x)

/** Preprocessor indirection to turn a method into a function pointer */
#define VTABLE_PTR_METHOD(x) CAT(_VTABLE_PTR_, x)

/** Generates a function pointer for the given function */
#define _VTABLE_PTR_METHOD(return, type, name, ...)                            \
  return ( *name ) ( __VA_ARGS__ );

/** Preprocessor indirection to turn a method into a fucntion declaration */
#define VTABLE_DEC_METHOD(x) CAT(_VTABLE_DEC_, x)

/** Generates a function declaration for the given function */
#define _VTABLE_DEC_METHOD(return, type, name, ...)                            \
  return type##_##name( __VA_ARGS__ );

/**
 * Defines a vtable for the given type, generates the structure
 * definition (filling it with pointers for the supplied methods),
 * then creates the function declarations as well (to reduce rewriting
 * the same code multiple times).
 */
#define DEFINE_VTABLE(type, ...)                                               \
  typedef struct vtable_t(type) vtable_t(type);                                \
  struct vtable_t(type)                                                        \
  {                                                                            \
    EVAL(MAP(VTABLE_PTR_METHOD, __VA_ARGS__))                                  \
  };                                                                           \
  EVAL(MAP(VTABLE_DEC_METHOD, __VA_ARGS__))

#endif

