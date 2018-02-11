/*
 * Name: Austin Donovan
 * Id:   1001311620
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include "command.h"
#include "oo.h"

void command_init( command_t* this )
{
  this->string = NULL;
  list_init( &this->tokens );
}

void command_destroy( command_t* this )
{
  free( this->string );
  list_destroy( &this->tokens, NULL );
  memset( this, 0, sizeof( *this ) );
}

void command_read( command_t* this )
{
  printf( "msh> " );

  list_t* tokens = &this->tokens;

  // temporary list of all the characters in the string
  list_t* string = new( list );

#define BUFFER_SIZE 255
  int length = 0; 
  char buff[ BUFFER_SIZE ];
  bool in_quote = false;

  while ( !feof( stdin ) )
  {
    char c = getc( stdin );

    // clear the buffer if we're on a new word
    if ( length == 0 )
    {
      memset( buff, 0, BUFFER_SIZE );
    }
    else if ( length == BUFFER_SIZE - 1 )
    {
      printf( "Token starting with %s is too long\n", buff );
    }

    // capture everything between two (unescaped) quotes
    if ( c == '"' )
    {
      // toggle the state
      in_quote ^= true;
    }
    // whitespace denotes the end of a token (but only outside quotes)
    else if ( !in_quote && ( c == ' ' || c == '\t' || c == '\n' || c == '\r' ) )
    {
      // allocate the word on the stack
      char* word = calloc( length + 1, sizeof( char ) );
      memcpy( word, buff, length );

      // add the word to the list
      list_push( tokens, ( void* ) word );

      // reset the length to 0
      length = 0;
    }
    // just add any other printable char onto the buffer
    else if ( c >= 32 )
    {
      buff[ length ] = c;
      length += 1;
    }

    // if we found a newline, then that also means we end the line
    if ( c == '\r' || c == '\n' )
    {
      break;
    }
    else
    {
      char* other_c = malloc( sizeof( char ) );
      *other_c = c;
      list_push( string, other_c );
    }
  }
#undef BUFFER_SIZE

  // consolidate string into a single char*
  this->string = calloc( string->size + 1, sizeof( char ) );

  list_iter_t iter = list_iter_create( string );
  while ( list_iter_peek( &iter ) != NULL )
  {
    this->string[ iter.index ] = *( char* ) list_iter_pop( &iter );
  }

  // delete the char list
  delete( list, string, NULL );
}

const char* command_get_name( const command_t* this )
{
  return ( const char* ) list_get( &this->tokens, 0 );
}

pid_t command_exec( const command_t* this )
{
  pid_t child_pid = fork();

  if ( child_pid == -1 )
  {
    perror( "Failed to start child process " );
  }
  else if ( child_pid == 0 )
  {
#define SEARCH_PATH_COUNT 4
    char* search_paths[ SEARCH_PATH_COUNT ] = {
      ".",
      "/usr/local/bin",
      "/usr/bin",
      "/bin"
    };
    const char* name = command_get_name( this );
    char* program_path = calloc( 16 + strlen( name ), sizeof( char )); 

    // create a NULL-terminated array from our tokens array if it
    // doesn't already have it (I noticed that it wouldn't be NULL terminated
    // if you used the max number of tokens)
    char** args = calloc( this->tokens.size + 1, sizeof( char* ) );
    {
      list_iter_t iter = list_iter_create( &this->tokens );

      while ( list_iter_peek( &iter ) != NULL )
      {
        args[ iter.index ] = ( char* ) list_iter_pop( &iter );
      }
      args[ iter.index ] = NULL;
    }
    
    // the value of this may change, but it will always point to
    // the full path of the executable we're trying to run
    args[ 0 ] = program_path;

    // try all of the search paths
    int i;
    for ( i = 0; i < SEARCH_PATH_COUNT; i++ )
    {
      sprintf( program_path, "%s/%s", search_paths[ i ], name );
      execv( program_path, args );
    }

    printf( "%s: command not found\n", name );

    // I could deallocate the strdup'd tokens, but at this point
    // we're literally just going to suicide, so the OS can clean up
    // our memory
    exit( 1 );

#undef SEARCH_PATH_COUNT
  }

  return child_pid;
}


