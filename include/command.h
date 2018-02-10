/*
 * Name: Austin Donovan
 * Id:   1001311620
 */

#ifndef MSH_COMMAND_H
#define MSH_COMMAND_H

#include <unistd.h>
#include <stdbool.h>
#include "list.h"

/**
 * A command type.
 */
typedef struct command_t {
  /** 
   * The original string used to parse tokens (kept around for freeing)
   */
  char* string;

  /** A list of all of the tokens in this command */
  list_t* tokens;

} command_t;

/**
 * Reads a new command from the user. This also prints out the shell's
 * prompt.
 */
command_t* command_read();

/**
 * Frees this command structure and its heap-allocated data.
 */
void command_free( command_t* );

/**
 * Tries to execute the given command. This will return the pid of the
 * child process which ran (or is running).
 */
pid_t command_exec( const command_t* );

#endif

