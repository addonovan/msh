/*
 * Name: Austin Donovan
 * Id:   1001311620
 */

#include <stdio.h>
#include "command.h"
#include "shell.h"

int main()
{
  shell_t* shell = malloc( sizeof( *shell ) );
  shell_init( shell );

  // never have to worry about deleting the command,a
  // as its ownership is passed off into the shell
  // by shell_run_command.
  command_t* command = NULL;
  do
  {
    shell_wait( shell );
    fflush( stdout );

    command = malloc( sizeof( *command ) );
    command_init( command );
    command_read( command );
  }
  while ( shell_run_command( shell, command ) );

  shell_destroy( shell );
  free( shell );
}

