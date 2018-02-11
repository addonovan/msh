/*
 * Name: Austin Donovan
 * Id:   1001311620
 */

#ifndef MSH_SHELL_INS_H
#define MSH_SHELL_INS_H

#include <unistd.h>
#include "list.h"
#include "command.h"
#include "handlers.h"

typedef struct shell_t shell_t;

struct shell_t 
{
  /** A list of all commands run by the shell. */
  list_t cmd_history;

  /** A list of all pids run by the shell. */
  list_t pid_history;

  /** A list of all pids running in the background */
  list_t background_pids;

  /** The pid the shell is currently running. */
  pid_t current_pid;

  /** The signal handler, for SIGTSTP and SIGINT */
  handler_t handler;
};

/**
 * Initializes the given shell.
 */
void shell_init( shell_t* );

/**
 * Frees the members of the given shell. After this,
 * the shell can be freed (if it's on the heap), or
 * fall out of scope, without any memory leaks.
 */
void shell_delete( shell_t* );

/**
 * Marks the given shell as the "active" shell, such
 * that static methods (such as the signal handler)
 * will refer to its data.
 */
void shell_set_active( shell_t* );

#endif

