#ifndef MSH_LIST_H
#define MSH_LIST_H

typedef struct list_t list_t;
typedef struct list_node_t list_node_t;
typedef struct list_iter_t list_iter_t;

//
// list
//

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

