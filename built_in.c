#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "built_in.h"
#include "list.h"
#include "command.h"

//
// !!, !<n>, and !+<n>
//

/**
 * Runs an item from an offset in the history list.
 * [action] is just the string to parse into a command, while [history] is
 * the list of history items.
 */
command_t* get_history_item(
   const char* action,
   const list_t* history
)
{
  unsigned int index;

  // + means we're going to be using the absolute position
  // in the list
  if ( action[ 1 ] == '+' )
  {
    index = strtol( action + 2, NULL, 0 );
  }
  // otherwise, we'll just use the past 15 commands
  else
  {
    int offset = history->size - 15;
    if ( offset < 0 )
    {
      offset = 0;
    }


    index = offset + strtol( action + 1, NULL, 0 );
  }

  return ( command_t* ) list_get( history, index );
} 

command_t* built_in_run_history( 
    command_t* command, 
    const list_t* history 
)
{
  char* action = list_get( command->tokens, 0 );
  command_t* prev = NULL;

  // run the last command
  if ( strcmp( action, "!!" ) == 0 )
  {
    prev = ( command_t* ) list_get( history, history->size - 1 );
  }
  else
  {
    prev = get_history_item( action, history );
  }

  // this command has out-lived its usefulness
  command_free( command );

  return prev;
}

//
// showpids
//

/**
 * Prints the last [count] pids from the [pids] list.
 */
void print_pids( const list_t* pids, unsigned int count )
{
  unsigned int start = pids->size - count;
  list_iter_t* iter = list_iter( pids );
  list_iter_jump( iter, start );

  printf( "Showing last %d pids (of %d total)\n", count, pids->size );

  unsigned int i;
  for ( i = 0; i < count; i++ )
  {
    printf( "%d: %d\n", i, *( ( pid_t* ) list_iter_pop( iter ) ) );
  }

  list_iter_free( iter );
}

void built_in_showpids( const command_t* command, const list_t* pids )
{
  unsigned int count = 10;

  char* arg0 = list_get( command->tokens, 1 );
  char* arg1 = list_get( command->tokens, 2 );

  if ( arg0 != NULL )
  {
    if ( strcmp( arg0, "--all" ) == 0
      || strcmp( arg0, "-a" ) == 0 )
    {
      count = pids->size;
    }
    else if ( strcmp( arg0, "--count" ) == 0 
           || strcmp( arg0, "-c" ) == 0 )
    {
      if ( arg1 == NULL )
      {
        fprintf( stderr, "--count requires an integral option\n" );
        return;
      }
      else
      {
        count = strtol( arg1, NULL, 0 );
      }
    }
  }

  if ( pids->size < count )
  {
    count = pids->size;
  }

  print_pids( pids, count );
}


//
// history
//

/**
 * Prints the last [count] items from the [history] list.
 */
void print_history( const list_t* history, unsigned int count )
{
  unsigned int start = history->size - count;
  list_iter_t* iter = list_iter( history );
  list_iter_jump( iter, start );

  printf( "Showing last %d commands (of %d total)\n", count, history->size );

  unsigned int i;
  for ( i = 0; i < count; i++ )
  {
    command_t* command = list_iter_pop( iter );
    printf( "%d: %s\n", i, command->string );
  }

  list_iter_free( iter );
}

void built_in_history( const command_t* command, const list_t* history )
{
  (void)(command);

  unsigned int count = 15;

  if ( history->size < count )
  {
    count = history->size;
  }

  print_history( history, count );
}

//
// cd
//

void built_in_cd( const command_t* command )
{
  char* dir; 
  char* arg = list_get( command->tokens, 1 );

  // no arguments? send them home
  if ( arg == NULL )
  {
    dir = getenv( "HOME" );
  }
  // otherwise, navigate to the given directory
  else
  {
    dir = arg;
  }

  chdir( dir );
}

//
// pwd
//

void build_in_pwd( const command_t* command )
{
  (void)(command);

  char cwd[ 1024 ]; // I really hope it's not longer than this
  if ( getcwd( cwd, sizeof( cwd ) ) != NULL ) 
  {
    printf( "%s\n", cwd );
  }
  else
  {
    perror( "getcwd() error\n" );
  }
}

