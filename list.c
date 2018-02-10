#include <stdlib.h>
#include "list.h"

// 
// list
//

list_t* list_create()
{
  list_t* this = malloc( sizeof( list_t ) );

  this->head = NULL;
  this->tail = NULL;
  this->size = 0;

  return this;
}

void list_push( list_t* this, void* item )
{
  list_node_t* node = list_node_create( item );

  if ( this->tail == NULL ) // => this->head == NULL also
  {
    this->head = node;
    this->tail = node;
  }
  else
  {
    this->tail->next = node;
    node->prev = this->tail;
    this->tail = node;
  }

  this->size += 1;
}

void* list_pop( list_t* this )
{
  if ( this->size == 0 ) return NULL;

  list_node_t* node = this->tail;
  
  // save a pointer to the data first
  void* data = node->data;

  // prevent data from being freed by list_node_free( node )
  node->data = NULL;

  // finally, delete the list_node_t
  this->head = node->prev;
  list_node_free( node );

  // actually decrease the size of the list
  this->size -= 1;

  return data;
}

void* list_get( const list_t* this, unsigned int index )
{
  if ( index >= this->size ) return NULL;

  list_node_t* current = this->head;

  unsigned int i;
  for ( i = 0; i < index; i++ )
  {
    current = current->next;
  }

  return current->data;
}

list_iter_t* list_iter( const list_t* this )
{
  return list_iter_create( this->head );
}

void list_iter_jump( list_iter_t* this, unsigned int steps )
{
  unsigned int target = this->index + steps;
  while ( this->current != NULL && this->index != target )
  {
    this->current = this->current->next;
    this->index += 1;
  }
}

void list_free( list_t* this )
{
  // list_node_free will maintain the list's linkage
  while ( this->head->next != NULL )
  {
    list_node_free( this->head->next );
  }
  list_node_free( this->head );
  free( this );
}

//
// list_iter
//

list_iter_t* list_iter_create( list_node_t* start )
{
  list_iter_t* this = malloc( sizeof( list_iter_t ) );
  
  this->current = start;
  this->index = 0;

  return this;
}

void* list_iter_peek( const list_iter_t* this )
{
  if ( this->current == NULL ) return NULL;

  return this->current->data;
}

void* list_iter_pop( list_iter_t* this )
{
  if ( this->current == NULL ) return NULL;
  
  void* val = list_iter_peek( this );

  this->current = this->current->next;
  this->index += 1;

  return val;
}

void list_iter_free( list_iter_t* this )
{
  free( this );
}

//
// list_node
//

list_node_t* list_node_create( void* data )
{
  list_node_t* this = malloc( sizeof( list_node_t ) );

  this->next = NULL;
  this->prev = NULL;
  this->data = data;

  return this;
}

void list_node_free( list_node_t* this )
{
  if ( this->data != NULL )
  {
    free( this->data );
  }

  // preserve the overall linkage of the list
  if ( this->prev != NULL )
  {
    this->prev->next = this->next;
  }
  if ( this->next != NULL )
  {
    this->next->prev = this->prev;
  }

  free( this );
}

