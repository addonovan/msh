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

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "command.h"
#include "list.h"
#include "built_in.h"

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

    if ( command->tokens->size != 0 )
    {
      // history-relative options get expanded before they're placed
      // in the history
      char* action = list_get( command->tokens, 0 );
      if ( action[ 0 ] == '!' )
      {
        // if the command didn't resolve correctly, then skip the rest
        // of the iteration
        if ( ( command = built_in_run_history( command, history ) ) == NULL )
        {
          fprintf( stderr, "Command not in history.\n" );
          continue;
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
    built_in_showpids( command, pids );
  }
  // show the user their history
  else if ( strcmp( action, "history" ) == 0 )
  {
    built_in_history( command, history );
  }
  // change working directory
  else if ( strcmp( action, "cd" ) == 0 )
  {
    built_in_cd( command );
  }
  // print working directory
  else if ( strcmp( action, "pwd" ) == 0 )
  {
    build_in_pwd( command );
  }
  // check for binaries
  else
  {
    return command_exec( command );
  }

  return ( pid_t ) 0;
}



