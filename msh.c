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

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

#define WHITESPACE " \t\n"
#define MAX_COMMAND_SIZE 255
#define MAX_NUM_ARGUMENTS 5

/**
 * A command type.
 */
typedef struct command_t {
  /** 
   * The original string used to parse tokens (kept around for freeing)
   */
  char* string;

  /** The tokens of the command string. */
  char* tokens[ MAX_NUM_ARGUMENTS ];

  /** The number of tokens found in this command. */
  unsigned int token_count;
} command_t;

/**
 * Reads a new command from the user. This also prints out the shell's
 * prompt.
 *
 * @return A new command structure.
 */
command_t* command_read();

/**
 * Frees a given command structure and its heap-allocated data.
 *
 * @param[this]
 *      The command to free.
 */
void command_free( command_t* );

/**
 * Tries to execute the given command.
 *
 * @param[this]
 *      The command to execute.
 * @return If the command was successfully found and executed.
 */
bool command_exec( command_t* );

int main()
{
  bool running = true;
  while( running )
  {
    command_t* command = command_read();

    // hard-coded actions
    if ( strcmp( command->tokens[ 0 ], "quit" ) == 0
      || strcmp( command->tokens[ 0 ], "exit" ) == 0 )
    {
      running = false;
    }
    // check for binaries
    else
    {
      command_exec( command );
    }

    command_free( command );
  }
  return 0;
}

//
// command_* implementations
//

command_t* command_read()
{
  printf( "msh> " );

  command_t* this = malloc( sizeof( command_t ) );
  this->string = calloc( sizeof( char ), MAX_COMMAND_SIZE );

  // Read the command from the commandline.  The
  // maximum command that will be read is MAX_COMMAND_SIZE
  // This while command will wait here until the user
  // inputs something since fgets returns NULL when there
  // is no input
  while( !fgets( this->string, MAX_COMMAND_SIZE, stdin ) );

  this->token_count = 0;
  
  // Pointer to point to the token
  // parsed by strsep
  char* arg_ptr;                                                      
  char* working_str = this->string;

  // Tokenize the input stringswith whitespace used as the delimiter
  while ( ( ( arg_ptr = strsep( &working_str, WHITESPACE ) ) != NULL ) && 
            ( this->token_count < MAX_NUM_ARGUMENTS ) )
  {
    this->tokens[ this->token_count ] = strndup( arg_ptr, MAX_COMMAND_SIZE );
    if( strlen( this->tokens[ this->token_count ] ) == 0 )
    {
      this->tokens[ this->token_count ] = NULL;
    }
    this->token_count++;
  }

  return this;
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
    
  }
  else
  {
    printf( "Hello from parent: %d\n", child_pid );
    int status;
    waitpid( child_pid, &status, 0 );
    if ( WIFSIGNALED( status ) )
    {
      printf( "Child exited with status %d\n", WTERMSIG( status ) );
    }
  }

  return false;
}

void command_free( command_t* this )
{
  free( this->string );
  free( this );
}

