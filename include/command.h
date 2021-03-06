/*
 * Name: Austin Donovan
 * Id:   1001311620
 */

#ifndef __MSH_COMMAND_H__
#define __MSH_COMMAND_H__

typedef struct command_t command_t;

#include <unistd.h>
#include <stdbool.h>
#include "generic.h"

/**
 * A command type.
 */
struct command_t {
  
  /** The string entered by the user */
  char* string;

  /** A list of all of the tokens in this command */
  list_t(string)* tokens;

};

/**
 * Initializes a command.
 */
void command_init( command_t* );

void command_copy( command_t* this, const command_t* src );

/**
 * Reads a new command from the user. This also prints out the shell's
 * prompt.
 */
void command_read( command_t* );

/**
 * Deletes the data allocated for this command structure,
 * so it can be freed or drop out of scope, without any
 * memory leaks.
 */
void command_destroy( command_t* );

/**
 * Gets an immutable pointer to the first token of this
 * command (or NULL if none exist). This only borrows
 * the value, it does not transfer ownership.
 */
const char* command_get_name( const command_t* );

/**
 * Tries to execute the given command. This will return the pid of the
 * child process which ran (or is running).
 */
pid_t command_exec( const command_t* );

#endif

