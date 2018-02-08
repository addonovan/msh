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

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments

typedef struct command_t {
  char* string;

  char* tokens[ MAX_NUM_ARGUMENTS ];

  unsigned int token_count;

} command_t;

command_t* command_read();
void command_free( command_t* this );

void read_input( char* cmd_str );

int main()
{
  while( 1 )
  {
    // Print out the msh prompt
    command_t* command = command_read();

    int token_index = 0;
    for( token_index = 0; token_index < command->token_count; token_index++ ) 
    {
      printf("token[%d] = %s\n", token_index, command->tokens[ token_index ] );  
    }

    command_free( command );
  }
  return 0;
}

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

void command_free( command_t* this )
{
  free( this->string );
  free( this );
}

