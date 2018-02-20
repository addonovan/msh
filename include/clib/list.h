/*
 * Name: Austin Donovan
 * Id:   1001311620
 */

#ifndef __CLIB_LIST_H__
#define __CLIB_LIST_H__

/** Defines the list type for the given name */
#define list_t( x ) list_##x##_t

/** Defines the list node type for the given name */
#define list_node_t( x ) list_node_##x##_t

/** Allocates a new list whose metadata resides on the stack. */
#define list( x ) ({                                                           \
      list_t( x ) tmp;                                                         \
      list_##x##_init( &tmp );                                                 \
      tmp;                                                                     \
    })

/** Allocates a new unmanaged list, whose metadata resides on the heap. */
#define list_u( x ) ({                                                         \
      list_t( x )* tmp = malloc( sizeof( list_t( x ) ) );                      \
      list_##x##_init( tmp );                                                  \
      tmp;                                                                     \
    })

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "preproc.h"
#include "vtable.h"

#endif // END OF INCLUDE GUARD

// we *must* know the type
#ifndef TYPE
  #error "TYPE must be defined before list.h is included"
  #define TYPE int
#endif

// if they didn't define COPY_VALUE, then we assume
// that we assume that the reference type should be
// a const pointer
#ifdef COPY_VALUE 
  #define REF_TYPE TYPE
  #define CONST_REF_TYPE TYPE
#else
  #define REF_TYPE TYPE*
  #define CONST_REF_TYPE const TYPE*
#endif

#define LN_P PREFIX(list_node, TYPE)
#define L_P  PREFIX(list, TYPE)

#define LN_T STRUCT_TYPE(list_node, TYPE)
#define L_T  STRUCT_TYPE(list, TYPE)

#define LN_VT CAT(__vtable, LN_P)
#define L_VT  CAT(__vtable, L_P)

#define LN_METHOD(x) METHOD_NAME(LN_P, x)
#define L_METHOD(x) METHOD_NAME(L_P, x)

#if defined(HEADER_ONLY) || !defined(IMPLEMENTATION_ONLY)

//
// Forward declare all of the typedefs
//

typedef struct LN_T LN_T; 
typedef struct L_T L_T;

//
// Vtables
//

// (method doc is on the implementation on vtabled files)

DEFINE_VTABLE(
  LN_T, 
  METHOD(void, LN_P, init, LN_T* this, REF_TYPE data),
  METHOD(void, LN_P, destroy, LN_T* this),
)

DEFINE_VTABLE(
  L_T,
  METHOD(void, L_P, init,    L_T* this),
  METHOD(void, L_P, destroy, L_T* this),

  METHOD(void, L_P, push, L_T* this, REF_TYPE item),
  METHOD(void, L_P, enqueue,  L_T* this, REF_TYPE item),

  METHOD(REF_TYPE, L_P, pop, L_T* this),
  METHOD(REF_TYPE, L_P, pop_back,  L_T* this),

  METHOD(REF_TYPE, L_P, remove, L_T* this, unsigned int index),

  METHOD(CONST_REF_TYPE, L_P, get, const L_T* this, unsigned int index)
)

//
// Struct definitions
//

/**
 * A node in the linked list for the given type.
 */
struct LN_T
{
  /** The data contained in this list. */
  REF_TYPE data;

  /** The previous node (or NULL if head) */
  LN_T* prev;

  /** The next node (or NULL if tail) */
  LN_T* next;

  /** A pointer to our vtable */
  const vtable_t(LN_T)* fun;
};

/**
 * A linked list of a given type.
 */
struct L_T
{
  /** The first element in the list */
  LN_T* head;

  /** The last element in the list */
  LN_T* tail;

  /** The number of elements in the list. */
  unsigned int size;

  /** A pointer to our vtable */
  const vtable_t(L_T)* fun;
};

#endif // HEADER

#if defined(IMPLEMENTATION_ONLY) || !defined(HEADER_ONLY) 

/** The constant vtable for list nodes */
vtable_t(LN_T)* LN_VT = NULL;

/** The constant vtable for lists */
vtable_t(L_T)* L_VT = NULL;

//
// list_node_t implementation
//

/**
 * Initializes the linked list node with the given [data] to
 * manage. This node will initialize the previous and next pointers
 * to NULL, as it hasn't been put in the list yet.
 */
void METHOD_NAME(LN_P, init)( LN_T* this, REF_TYPE data )
{
  this->fun = LN_VT;
  this->data = data;
  this->prev = NULL;
  this->next = NULL;
}

/**
 * Destroys the linked list node.
 *
 * This will maintain the linkage of the overall list (i.e. it'll
 * relink the previous and next nodes).
 */
void METHOD_NAME(LN_P, destroy)( LN_T* this )
{
  // maintain the surrounding nodes' linkage
  if ( this->prev != NULL )
  {
    this->prev->next = this->next;
  }
  if ( this->next != NULL )
  {
    this->next->prev = this->prev;
  }

#ifndef COPY_VALUE
  if ( this->data != ( REF_TYPE ) NULL )
  {
    // run destructor
  }
#endif
  memset( this, 0, sizeof( *this ) );
}

//
// list implementation
//

/**
 * Initializes the given list.
 */
DEF_METHOD(void, L_P, init, L_T* this)
{
  if ( L_VT == NULL )
  {
    L_VT = calloc( 1, sizeof( *L_VT ) );
    L_VT->destroy = &METHOD_NAME(L_P,  destroy );
    L_VT->push = &METHOD_NAME(L_P,  push );
    L_VT->enqueue = &METHOD_NAME(L_P,  enqueue );
    L_VT->pop = &METHOD_NAME(L_P,  pop );
    L_VT->pop_back = &METHOD_NAME(L_P,  pop_back );
    L_VT->remove = &METHOD_NAME(L_P,  remove );
    L_VT->get = &METHOD_NAME(L_P,  get );

    // while we're at it, let's create the vt for our nodes
    // so we don't have to worry about it in the future
    LN_VT = calloc( 1, sizeof( *LN_VT ) );
    LN_VT->init = &METHOD_NAME(LN_P, init);
    LN_VT->destroy = &METHOD_NAME(LN_P, destroy);
  } 

  this->fun = L_VT;
  this->head = NULL;
  this->tail = NULL;
  this->size = 0;
}

/**
 * Destroys the given list.
 */
DEF_METHOD(void, L_P, destroy, L_T* this)
{
  // if we have no elements, there's nothing to free
  if ( this->size == 0 ) return;

  // free all of our nodes
  // (n.b. list_node_*_destroy will fix linkage
  LN_T* current = this->head;
  while ( current->next != NULL )
  {
    LN_T* next = current->next;
    LN_VT->destroy( next );
    free( next );
  }
  LN_VT->destroy( current );
  free( current );

  // zero ourselves out to indicate that we're dead
  memset( this, 0, sizeof( *this ) );
}

/**
 * Pushes a new item of type T onto the front (=> index=0)
 * of this list.
 */
DEF_METHOD(void, L_P, push, L_T* this, REF_TYPE item)
{
  LN_T* node = malloc( sizeof( LN_T ) );
  LN_VT->init( node, item );

  node->next = this->head;
  if ( this->size == 0 )
  {
    this->tail = node;
  }
  else
  {
    this->head->prev = node;
  }
  this->head = node;

  this->size += 1;
}

/**
 * Puts an item of type T onto the back (=> index=list.size)
 * of this list.
 */
DEF_METHOD(void, L_P, enqueue, L_T* this, REF_TYPE item)
{
  LN_T* node = malloc( sizeof( LN_T ) );
  LN_VT->init( node, item );

  node->prev = this->tail;
  if ( this->size == 0 )
  {
    this->head = node;
  }
  else
  {
    this->tail->next = node;
  }
  this->tail = node;

  this->size += 1;
}

/**
 * Removes and returns the first element from this list.
 * 
 * Assertions:
 * * this->size > 0
 */
DEF_METHOD(REF_TYPE, L_P, pop, L_T* this)
{
  return L_VT->remove( this, 0 );
}

/**
 * Removes and returns the last element from this list.
 *
 * Assertions:
 * * this->size > 0
 */
DEF_METHOD(REF_TYPE, L_P, pop_back,  L_T* this)
{
  return L_VT->remove( this, this->size - 1 );
}

/**
 * Removes and returns the nth element from this list.
 *
 * Assertions:
 * * this->size > index 
 */
DEF_METHOD(REF_TYPE, L_P, remove, L_T* this, unsigned int index)
{
  assert( this->size > index );

  // TODO start from back if index >= size/2
 
  LN_T* current = this->head;
  unsigned int i;
  for ( i = 0; i < index; i++ )
  {
    current = current->next;
  }

  REF_TYPE item = current->data;

  // if it's not a copied value, then we're going to
  // need to prevent the destructor from deleting it
#ifndef COPY_VALUE
  current->data = ( REF_TYPE ) NULL;
#endif

  if ( current->next == NULL )
  {
    this->tail = current->prev;
  }
  if ( current->prev == NULL )
  {
    this->head = current->next;
  }

  LN_VT->destroy( current );
  free( current );

  this->size -= 1;

  return item; 
}

/**
 * Returns a constant reference to the item in the nth position
 * of this list.
 */
DEF_METHOD(CONST_REF_TYPE, L_P, get, const L_T* this, unsigned int index)
{
  // TODO start from back if index >= size/2
 
  LN_T* current = this->head;
  unsigned int i;
  for ( i = 0; i < index; i++ )
  {
    current = current->next;
  }

#ifdef COPY_VALUE
  return current->data;
#else
  return current->data;
#endif
}

#endif // IMPLEMENTATION

// don't leak any of our preprocessor symbols
#undef IMPLEMENTATION_ONLY
#undef HEADER_ONLY
#undef HEADER_ONLY
#undef L_METHOD
#undef LN_METHOD
#undef L_VT
#undef LN_VT
#undef L_T
#undef LN_T
#undef L_P
#undef LN_P
#undef CONST_REF_TYPE
#undef REF_TYPE
#undef COPY_VALUE
#undef TYPE

