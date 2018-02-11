/*
 * Name: Austin Donovan
 * Id:   1001311620
 */

#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "oo.h"

//
// list_node
//

void list_node_init( list_node_t* this, void* data )
{
  this->next = NULL;
  this->prev = NULL;
  this->data = data;
}

void list_node_destroy( list_node_t* this )
{
  // preserve the overall linkage of the list
  if ( this->prev != NULL )
  {
    this->prev->next = this->next;
  }
  if ( this->next != NULL )
  {
    this->next->prev = this->prev;
  }
}

// 
// list
//

void list_init( list_t* this, destructor* destructor )
{
  this->head = NULL;
  this->tail = NULL;
  this->size = 0;
  this->destructor = destructor;
}

void list_destroy( list_t* this )
{
  void* data; 
  while ( ( data = list_pop( this ) ) != NULL )
  {
    // call the applicable destructor for the item
    if ( this->destructor != NULL )
    {
      this->destructor( data );
    }

    // then just free it from the heap
    free( data );
  }
}

void list_push( list_t* this, void* item )
{
  list_node_t* node = new( list_node, item );

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

  // prevent data from being deleted by list_node_destroy( node )
  node->data = NULL;

  // finally, delete the list_node_t
  this->tail = node->prev;
  delete( list_node, node );

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

void list_iter_jump( list_iter_t* this, unsigned int steps )
{
  unsigned int target = this->index + steps;
  while ( this->current != NULL && this->index != target )
  {
    this->current = this->current->next;
    this->index += 1;
  }
}

//
// list_iter
//

list_iter_t list_iter_create( const list_t* list )
{
  list_iter_t this;
  this.current = list->head;
  this.index = 0;
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

