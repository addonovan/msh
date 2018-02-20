/*
 * Name: Austin Donovan
 * Id:   1001311620
 */

#ifndef __CLIB_MEMORY_H__
#define __CLIB_MEMORY_H__

/**
 * Runs the destructor on the heap-allocated type (requires
 * a vtable), frees the memory, and sets the pointer back
 * to NULL (for easier debugging).
 */
#define delete(x)                                                              \
  do                                                                           \
  {                                                                            \
    x->fun->destroy( x );                                                      \
    free( x );                                                                 \
    x = NULL;                                                                  \
  }                                                                            \
  while ( 0 );                                                                 \

#endif

