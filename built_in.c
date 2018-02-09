#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "built_in.h"
#include "list.h"
#include "command.h"

void print_pids( const list_t* pids, unsigned int count )
{
  unsigned int start = pids->size - count;
  list_iter_t* iter = list_iter( pids );
  list_iter_jump( iter, start );

  printf( "Showing last %d pids (of %d total)\n", count, pids->size );

  int i;
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


void print_history( const list_t* history, unsigned int count )
{
  unsigned int start = history->size - count;
  list_iter_t* iter = list_iter( history );
  list_iter_jump( iter, start );

  printf( "Showing last %d commands (of %d total)\n", count, history->size );

  int i;
  for ( i = 0; i < count; i++ )
  {
    command_t* command = list_iter_pop( iter );
    printf( "%d: %s\n", i, command->string );
  }

  list_iter_free( iter );
}

void built_in_history( const command_t* command, const list_t* history )
{
  unsigned int count = 15;

  if ( history->size < count )
  {
    count = history->size;
  }

  print_history( history, count );
}

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

void build_in_pwd( const command_t* command )
{
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

