#define _GNU_SOURCE

#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include "command.h"


command_t* command_read()
{
  printf( "msh> " );

  command_t* this = malloc( sizeof( command_t ) );
  this->tokens = list_create();

  // temporary list of all the characters in the string
  list_t* string = list_create();

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
      fprintf( stderr, "Token starting with %s is too long\n", buff );
      return NULL;
    }

    // capture everything between two (unescaped) quotes
    if ( c == '"' )
    {
      // toggle the state
      in_quote ^= true;
    }
    // whitespace denotes the end of a token (but only outside quotes)
    else if ( !in_quote && c == ' ' || c == '\t' || c == '\n' || c == '\r' )
    {
      // allocate the word on the stack
      char* word = calloc( sizeof( char ), length + 1 );
      memcpy( word, buff, length );

      // add the word to the list
      list_push( this->tokens, ( void* ) word );

      // reset the length to 0
      length = 0;
    }
    // just add any other character onto the buffer
    else
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
  this->string = calloc( sizeof( char ), string->size + 1 );
  list_iter_t* iter = list_iter( string );
  while ( list_iter_peek( iter ) != NULL )
  {
    this->string[ iter->index ] = *( char* ) list_iter_pop( iter );
  }
  list_iter_free( iter );
  list_free( string );

  return this;
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
    char* program_name = strdup( this->tokens->head->data );
    char* program_path = calloc( sizeof( char ), 16 + strlen( program_name ) ); 

    // create a NULL-terminated array from our tokens array if it
    // doesn't already have it (I noticed that it wouldn't be NULL terminated
    // if you used the max number of tokens)
    char** args = calloc( sizeof( char* ), this->tokens->size + 1 );
    {
      list_iter_t* iter = list_iter( this->tokens );

      while ( list_iter_peek( iter ) != NULL )
      {
        args[ iter->index ] = ( char* ) list_iter_pop( iter );
      }
      args[ iter->index ] = NULL;
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

    // I could deallocate the strdup'd tokens, but at this point
    // we're literally just going to suicide, so the OS can clean up
    // our memory
    exit( 1 );

#undef SEARCH_PATH_COUNT
  }
  else
  {
    int status;
    if ( waitpid( child_pid, &status, 0 ) == -1 )
    {
      printf( "Failed to wait for child!\n" );
    }

    if ( WIFSIGNALED( status ) )
    {
      int exit_signal = WTERMSIG( status );
      printf( "[%d] exited by signal %d\n", child_pid, exit_signal );
    }

    // if we received a non-zero exit code, that means bad, so tell
    // the user that the program exitted incorrectly
    if ( WIFEXITED( status ) )
    {
      int exit_status = WEXITSTATUS( status );
      if ( exit_status != 0 )
      {
        printf( "X[%d] ", exit_status );
      }  
    }
  }

  return child_pid;
}

void command_free( command_t* this )
{
  free( this->string );
  free( this );
}


