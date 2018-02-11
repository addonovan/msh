/*
 * Name: Austin Donovan
 * Id:   1001311620
 */

#ifndef MSH_OO_H
#define MSH_OO_H

#include <stdlib.h>

// these ONLY work with GCC,
// but they do work with really old versions of GCC
// too, such as v4.1.2, which omega happens to have!

/**
 * Similar to c++'s new keyword. Allocates memory
 * on the stack for the given item, then calls
 * the respective _init method with the given varargs.
 */
#define new( type, ... ) ({                             \
    type ## _t* x = malloc( sizeof( type ## _t ) );     \
    type ## _init( x, ##__VA_ARGS__ );                  \
    x;                                                  \
  })                                                    \


#define copy( src ) ({                                  \
    unsigned int size = sizeof( *src );                 \
    void* x = malloc( size );                           \
    memcpy( x, src, size );                             \
    x;                                                  \
  })                                                    \
                                                        \
                                                        \

/** 
 * Similar to c++'s delete keyword. Destroys
 * the value at the pointer, [x], using the
 * method corresponding to [type], then 
 * invokes `free` on [x], and sets [x] to 
 * `NULL`.
 */
#define delete( type, x )                               \
  do                                                    \
  {                                                     \
    type ## _destroy( x );                              \
    free( x );                                          \
    x = NULL;                                           \
  }                                                     \
  while ( 0 )

#endif

