/*
 * Name: Austin Donovan
 * Id:   1001311620
 */

#ifndef MSH_LIST_H
#define MSH_LIST_H

typedef struct list_t list_t;
typedef struct list_node_t list_node_t;
typedef struct list_iter_t list_iter_t;

//
// list
//

/**
 * A list structure.
 *
 * While this is intended to be abstract enough such that
 * the implementation doesn't matter, it is actually a 
 * linked list (because those are pretty easy to implement)
 */
struct list_t
{
  list_node_t* head;
  list_node_t* tail;

  unsigned int size;
};

/**
 * Creates a new list on the heap
 */
list_t* list_create();

/**
 * Pushes a new [item] onto the back of this list.
 * The list will take ownership over the item, so when [list_free] is
 * called, it will be deallocated.
 */
void list_push( list_t*, void* item );

/**
 * Removes the list item from the list, and returns it's value.
 * The item will no longer be owned by this list, and thus you
 * must remember to free it manually.
 */
void* list_pop( list_t* );

/**
 * Gets the item at the index.
 */
void* list_get( const list_t*, unsigned int index );

/**
 * Returns an iterator for this list.
 */
list_iter_t* list_iter( const list_t* );

/**
 * Deallocates this list and all of its data.
 */
void list_free( list_t* );

//
// list_iter
//

/**
 * A structure for iterating over a list starting at a
 * given position.
 */
struct list_iter_t
{
  list_node_t* current;
  unsigned int index;
};

/**
 * Creates an iterator starting at the given node.
 */
list_iter_t* list_iter_create( list_node_t* start );

/**
 * Returns the value at the current position, but won't 
 * progress forward. This will return NULL if there are 
 * no more items.
 */
void* list_iter_peek( const list_iter_t* );

/**
 * Returns the value at the current position, then moves
 * to the next item. This will return NULL if there are
 * no more items.
 */
void* list_iter_pop( list_iter_t* );

/**
 * Skips exactly [steps] elements ahead, if possible.
 */
void list_iter_jump( list_iter_t*, unsigned int steps );

/**
 * Destroys this list iterator.
 */
void list_iter_free( list_iter_t* );

//
// list_node
//

/**
 * An actual node in the linked list.
 */
struct list_node_t 
{
  void* data;
  list_node_t* next;
  list_node_t* prev;
};

/**
 * Creates a new list node, which has the given [data], but
 * has no following item.
 */
list_node_t* list_node_create( void* data );

/**
 * Destroys this list node and it's data.
 */
void list_node_free( list_node_t* );

#endif

