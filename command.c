#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include "command.h"


command_t* command_read()
{
#define WHITESPACE " \t\n"
  printf( "msh> " );

  command_t* this = malloc( sizeof( command_t ) );
  this->string = calloc( sizeof( char ), MAX_COMMAND_SIZE );
  this->tokens = list_create();

  // Read the command from the commandline.  The
  // maximum command that will be read is MAX_COMMAND_SIZE
  // This while command will wait here until the user
  // inputs something since fgets returns NULL when there
  // is no input
  while( !fgets( this->string, MAX_COMMAND_SIZE, stdin ) );
  
  // Pointer to point to the token
  // parsed by strsep
  char* arg_ptr;                                                      
  char* working_str = this->string;

  // Tokenize the input stringswith whitespace used as the delimiter
  while ( ( arg_ptr = strsep( &working_str, WHITESPACE ) ) != NULL )
  {
    // copy the token and append it if it isn't empty
    char* token = strndup( arg_ptr, MAX_COMMAND_SIZE );
    if ( strlen( token ) != 0 )
    {
      list_push( this->tokens, ( void* ) token );
    }
  }

  return this;
#undef WHITESPACE
}

bool command_exec( command_t* this )
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

  return true;
}

void command_free( command_t* this )
{
  free( this->string );
  free( this );
}


