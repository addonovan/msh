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

void print_pids( list_t* pids, unsigned int count );

int main()
{
  list_t* pids = list_create(); 

  bool running = true;
  while( running )
  {
    command_t* command = command_read();

    // hard-coded actions
    if ( command->tokens->size != 0 )
    {
      char* action = list_get( command->tokens, 0 );

      // should we exit?
      if ( strcmp( action, "quit" ) == 0
        || strcmp( action, "exit" ) == 0 )
      {
        running = false;
      }
      // show process ids?
      else if ( strcmp( action, "showpids" ) == 0 )
      {
        unsigned int count = 10;

        char* flag = list_get( command->tokens, 1 );
        if ( pids->size < 10 || ( flag != NULL && strcmp( flag, "--all" ) ) )
        {
          count = pids->size;
        }

        print_pids( pids, count );
      }
      // check for binaries
      else
      {
        pid_t* pid = malloc( sizeof( pid_t ) );
        *pid = command_exec( command );
        list_push( pids, pid );
      }
    }

    command_free( command );
  }
  return 0;
}

void print_pids( list_t* pids, unsigned int count )
{
  unsigned int start = pids->size - count;
  list_iter_t* iter = list_iter( pids );
  list_iter_jump( iter, start );

  printf( "Starting at %d\n", start );
  printf( "Showing %d pids (of %d total)\n", count, pids->size );

  int i;
  for ( i = 0; i < count; i++ )
  {
    printf( "%d: %d\n", i, *( ( pid_t* ) list_iter_pop( iter ) ) );
  }

  list_iter_free( iter );
}

