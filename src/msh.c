/*
 * Name: Austin Donovan
 * Id:   1001311620
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "command.h"
#include "shell.h"
#include "oo.h"

int main()
{
  shell_t* shell = new( shell );

  // never have to worry about deleting the command,a
  // as it's ownership is passed off into the shell
  // by shell_run_command.
  command_t* command = NULL;
  do
  {
    command = new( command );
    command_read( command );
  }
  while ( shell_run_command( shell, command ) );

  delete( shell, shell );
}

