#ifndef MSH_COMMAND_H
#define MSH_COMMAND_H

#include <stdbool.h>
#include "list.h"

#define MAX_COMMAND_SIZE 255
#define MAX_NUM_ARGUMENTS 10

/**
 * A command type.
 */
typedef struct command_t {
  /** 
   * The original string used to parse tokens (kept around for freeing)
   */
  char* string;

  /** The tokens of the command string. */
  // additional token to account for tokens[ 0 ] = executable name
  char* tokens[ MAX_NUM_ARGUMENTS + 1 ];

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

#endif

