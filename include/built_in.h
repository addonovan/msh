/*
 * Name: Austin Donovan
 * Id:   1001311620
 */

#ifndef MSH_BUILT_INS_H
#define MSH_BUILT_INS_H

#include "command.h"
#include "list.h"

/**
 * Attempts to perform the built-in [command], provided one
 * exists.
 * 
 * This will take ownership over [command].
 *
 * This will return an owned command to be executed (e.g.
 * no built-in command found, or to run a historical item),
 * otherwise it will return NULL to signify that no action
 * should be taken.
 */
command_t* built_in_run( command_t* command );

/**
 * Grabs the specified command from the history list.
 * This will take ownership over the current command, then
 * return an owned copy of the new command to execute, which
 * may be NULL if the command was illegal.
 */
command_t* built_in_run_history( 
    command_t* command, 
    const list_t* history 
);

/**
 * Shows the last `n` (by default 10) pids from the [pids]
 * list.
 */
void built_in_showpids( const command_t* command, const list_t* pids );

/**
 * Shows the last `n` (by default 15) pids from the [history]
 * list.
 */
void built_in_history( const command_t* command, const list_t* history );

/**
 * Changes the current working directory to the second
 * argument of [command].
 */
void built_in_cd( const command_t* command );

/**
 * Prints the program's current working directory.
 */
void build_in_pwd( const command_t* command );

#endif

