/*
 * Name: Austin Donovan
 * Id:   1001311620
 */

#ifndef __LIBC_GENERIC_H__
#define __LIBC_GENERIC_H__

#include "preproc.h"

/** Defines a method. Used to pass methods as a tuple to other macros */
#define METHOD(return, type, name, ...)                                        \
  METHOD(return, type, name, __VA_ARGS__)

/** Creates a method header for the given method */
#define DEF_METHOD(return, type, name, ...)                                    \
  return METHOD_NAME(type, name)( __VA_ARGS__ )

/** Concatenates the type onto the method name, to generate a unique value */
#define METHOD_NAME(type, name)                                                \
  CAT(type, _##name)

/** Prefixes the name to the beginning of the type */
#define PREFIX(name, type)                                                     \
  CAT(name, CAT(_, type))

/** Creates the typename for a structure type */
#define STRUCT_TYPE(name, type)                                                \
  CAT(PREFIX(name, type), _t)

#endif

