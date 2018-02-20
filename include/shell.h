/*
 * Name: Austin Donovan
 * Id:   1001311620
 */

#ifndef MSH_SHELL_INS_H
#define MSH_SHELL_INS_H

#include <unistd.h>
#include "command.h"
#include "handlers.h"
#include "generic.h"

typedef struct shell_t shell_t;

struct shell_t 
{
  /** A list of all commands run by the shell. */
  list_t(command_t)* cmd_history;

  /** A list of all pids run by the shell. */
  list_t(pid_t)* pid_history;

  /** A list of all pids running in the background */
  list_t(pid_t)* background_pids;

  /** The pid for the current foreground process (or zero for none). */
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
void shell_destroy( shell_t* );

/**
 * Marks the given shell as the "active" shell, such
 * that static methods (such as the signal handler)
 * will refer to its data.
 */
void shell_set_active( const shell_t* );

/**
 * Suspends the current foreground process for this
 * shell.
 */
void shell_suspend( shell_t* );

/**
 * Resumes the last process the shell suspended, only
 * if it is not currently running any process in the
 * foreground. This will then return the pid of the
 * resumed process.
 */
pid_t shell_resume( shell_t* );

/**
 * Causes the thread to block, waiting for the
 * [current_pid] to stop.
 */
void shell_wait( shell_t* );

/**
 * Runs the command on the given shell.
 * If the command causes a process to be run, then
 * the [current_pid] will be updated.
 *
 * This will return [false] if the shell session
 * should terminate.
 */
bool shell_run_command( shell_t*, command_t* command );

#endif

