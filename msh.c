// The MIT License (MIT)
// 
// Copyright (c) 2016, 2017 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define WHITESPACE " \t\n"
#define MAX_COMMAND_SIZE 255
#define MAX_NUM_ARGUMENTS 10

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "command.h"
#include "list.h"

void print_pids( const list_t* pids, unsigned int count );
void print_history( const list_t* history, unsigned int count );
pid_t run_command( 
    const list_t* pids, 
    const list_t* history, 
    const command_t* command 
);

int main()
{
  list_t* pids = list_create(); 
  list_t* history = list_create();

  while( true )
  {
    command_t* command = command_read();

    // hard-coded actions
    if ( command->tokens->size != 0 )
    {
      // history-relative options get expanded before they're placed
      // in the history
      char* action = list_get( command->tokens, 0 );
      if ( action[ 0 ] == '!' )
      {
        command_t* prev = NULL;

        // run the last command
        if ( strcmp( action, "!!" ) == 0 )
        {
          prev = ( command_t* ) list_get( history, history->size - 1 );
        }
        else
        {
          long index = strtol( action + 1, NULL, 0 );
          prev = ( command_t* ) list_get( history, index );
        }

        // this command has out-lived its usefulness
        command_free( command );

        // tell the user if the action didn't exist
        if ( prev == NULL )
        {
          fprintf( stderr, "Command not in history.\n" );
          continue;
        }
        // expand back to the previous command
        else
        {
          command = prev;
        }
      }

      // store the command in our history for later use
      list_push( history, command );

      pid_t* pid = malloc( sizeof( pid_t ) );
      *pid = run_command( pids, history, command );
      if ( *pid == 0 )
      {
        free( pid );
      }
      else
      {
        list_push( pids, ( void* ) pid );
      }
    }
  }

  return 0;
}

pid_t run_command( 
    const list_t* pids, 
    const list_t* history, 
    const command_t* command 
)
{
  char* action = list_get( command->tokens, 0 );

  // should we exit?
  if ( strcmp( action, "quit" ) == 0
    || strcmp( action, "exit" ) == 0 )
  {
    exit( 0 );
  }
  // show process ids?
  else if ( strcmp( action, "showpids" ) == 0 )
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
          return ( pid_t ) 0;
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
  // show the user their history
  else if ( strcmp( action, "history" ) == 0 )
  {
    unsigned int count = 15;

    if ( history->size < count )
    {
      count = history->size;
    }

    print_history( history, count );
  }
  // check for binaries
  else
  {
    return command_exec( command );
  }

  return ( pid_t ) 0;
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

