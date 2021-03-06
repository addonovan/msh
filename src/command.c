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
#include "clib/memory.h"

void command_init( command_t* this )
{
  this->string = NULL;
  this->tokens = list_u(string);
}

void command_copy( command_t* this, const command_t* src )
{
  // initialize our command
  command_init( this );

  // duplicate the source string
  this->string = strdup( src->string );

  typeof(this->tokens->fun) vtable = this->tokens->fun;

  // copy each item from the source list
  unsigned int index = 0;
  while ( index < src->tokens->size )
  {
    char* token = strdup( vtable->get( src->tokens, index ) );
    vtable->enqueue( this->tokens, token );
    index += 1;
  }
}

void command_destroy( command_t* this )
{
  free( this->string );
  delete( this->tokens );
}

void command_read( command_t* this )
{
  printf( "msh> " );

  // temporary list of all the characters in the string
  list_t(char) string = list(char);

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
      char* word = calloc( sizeof( char ), length + 1 );
      memcpy( word, buff, length );

      // add the word to the list
      this->tokens->fun->enqueue( this->tokens, word );

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
      string.fun->enqueue( &string, c );
    }
  }
#undef BUFFER_SIZE

  // consolidate string into a single char*
  this->string = calloc( sizeof( char ), string.size + 1 );
  char* current = this->string;
  while ( string.size > 0 )
  {
    *current = string.fun->pop( &string );
    current++;
  }

  string.fun->destroy( &string );
}

const char* command_get_name( const command_t* this )
{
  return this->tokens->fun->get( this->tokens, 0 );
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
    // the name of the program is argument #1
    const char* program_name = this->tokens->fun->get( this->tokens, 0 );

    // allocate a enough space to concatenate the longest of the
    // search paths (14 chars), a slash (1 char), the program's name,
    // and then a terminate \0 (1 char)
    char* program_path = calloc( sizeof( char ), 16 + strlen( program_name ) ); 

    // create a NULL-terminated array from our tokens array if it
    // doesn't already have it (I noticed that it wouldn't be NULL terminated
    // if you used the max number of tokens)
    char** args = calloc( sizeof( char* ), this->tokens->size + 1 );
    {
      unsigned int index = 0;

      while ( index < this->tokens->size )
      {
        args[ index ] = this->tokens->fun->get( this->tokens, index );
        index += 1;
      }
      args[ index ] = NULL;
    }
    
    // the value of this may change, but it will always point to
    // the full path of the executable we're trying to run
    args[ 0 ] = program_path;

    // try all of the search paths
    int i;
    for ( i = 0; i < SEARCH_PATH_COUNT; i++ )
    {
      sprintf( program_path, "%s/%s", search_paths[ i ], program_name );
      execv( program_path, args );
    }

    printf( "%s: command not found\n", program_name );

    // I could deallocate the strdup'd tokens, but at this point
    // we're literally just going to suicide, so the OS can clean up
    // our memory
    exit( 1 );

#undef SEARCH_PATH_COUNT
  }

  return child_pid;
}


