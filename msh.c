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

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "command.h"
#include "list.h"

int main()
{
  bool running = true;
  while( running )
  {
    command_t* command = command_read();

    // hard-coded actions
    if ( command->tokens->size != 0 )
    {
      char* action = list_get( command->tokens, 0 );

      if ( strcmp( action, "quit" ) == 0
        || strcmp( action, "exit" ) == 0 )
      {
        running = false;
      }
      // check for binaries
      else
      {
        command_exec( command );
      }
    }

    command_free( command );
  }
  return 0;
}

