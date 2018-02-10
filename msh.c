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
#include "list.h"
#include "built_in.h"
#include "handlers.h"

// text colors:
#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

/** The pid of the currently-running foreground child process. */
pid_t g_current_pid = ( pid_t ) 0;

/** A global list of all process currently suspended */
list_t* g_backgrounded = NULL;

/**
 * Runs the specified [command], using the list of [pids],
 * and the command [history], and returns the pid of the
 * created process, if any; otherwise, it returns 0
 */
pid_t run_command( 
    const list_t* pids, 
    const list_t* history, 
    const command_t* command 
);

/**
 * The system signal handler. Listens for SIGTSTP and SIGINT
 * and forwards them to the child process.
 */
static void handle_sig( int signal );

/**
 * Waits for the currently running foreground child
 * (whose pid is stored in g_current_pid).
 */
void wait_child();

int main()
{
  handler_t* handler = handler_create( &handle_sig );

  list_t* pids = list_create(); 
  list_t* history = list_create();

  g_backgrounded = list_create();

  while( true )
  {
    // make sure all of the output is written before continuing
    fflush( stdout );

    command_t* command = command_read();

    // don't do anything on an empty command (except delete it)
    if ( command->tokens->size == 0 )
    {
      command_free( command );
      continue;
    }

    // history-relative options get expanded before they're placed
    // in the history
    char* action = list_get( command->tokens, 0 );
    if ( action[ 0 ] == '!' )
    {
      // if the command didn't resolve correctly, then skip the rest
      // of the iteration
      if ( ( command = built_in_run_history( command, history ) ) == NULL )
      {
        printf( KRED "Command not in history.\n" KNRM );
        continue;
      }
    }
    else
    {
      // store the command in our history for later use
      list_push( history, command );

      pid_t* pid = malloc( sizeof( pid_t ) );
      *pid = run_command( pids, history, command );
      g_current_pid = *pid;

      if ( *pid == 0 )
      {
        free( pid );
      }
      else
      {
        list_push( pids, ( void* ) pid );
      }
    }

    wait_child();
  }

  // free our locally-allocated lists
  list_free( pids );
  list_free( history );
  handler_free( handler );

  return 0;
}

void wait_child()
{
  if ( g_current_pid == 0 )
    return;

  int status;
  waitpid( g_current_pid, &status, 0 );

  if ( WIFSIGNALED( status ) )
  {
    int exit_signal = WTERMSIG( status );
    char* exit_text = strsignal( exit_signal );

    printf( KRED "! [%d] killed by %s\n" KNRM, g_current_pid, exit_text );
  }

  // if we received a non-zero exit code, that means bad, so tell
  // the user that the program exitted incorrectly
  if ( WIFEXITED( status ) )
  {
    int exit_status = WEXITSTATUS( status );
    if ( exit_status != 0 )
    {
      // print a red bang before the next shell line
      // (denotes non-zero exit code)
      printf( KRED "! " KNRM );
    }  
  }
}

void handle_sig( int signal )
{
  if ( g_current_pid == 0 )
    return;

  switch ( signal )
  {
    case SIGINT:
      kill( g_current_pid, SIGINT );
      break;

    case SIGTSTP:
      kill( g_current_pid, SIGTSTP );

      // add its pid to the list of backgrounded processes
      pid_t* new_pid = malloc( sizeof( pid_t ) );
      *new_pid = g_current_pid;
      list_push( g_backgrounded, new_pid );

      // \r overwrites the ^Z
      printf( "\r[%d]  + %d suspended\n", g_backgrounded->size, *new_pid );
      break;
  }
}

pid_t run_command( 
    const list_t* pids, 
    const list_t* history, 
    const command_t* command 
)
{
  char* action = list_get( command->tokens, 0 );

  // should we exit?
  if ( strcmp( action, "quit" ) == 0
    || strcmp( action, "exit" ) == 0 )
  {
    exit( 0 );
  }
  else if ( strcmp( action, "fg" ) == 0
         || strcmp( action, "bg" ) == 0 )
  {
    if ( g_backgrounded->size == 0 )
    {
      printf( "No stopped jobs\n" );
      return ( pid_t ) 0;
    }
    else
    {
      // get the last item from the backgrounded list, then
      // move it into the stack instead of the heap (because
      // we take ownership over any item returned from 
      // list_pop)
      pid_t stack;
      pid_t* pid = ( pid_t* ) list_pop( g_backgrounded );
      stack = *pid;
      free( pid );

      // tell the process to continue
      kill( stack, SIGCONT );
      printf( "[%d]  - %d continued\n", g_backgrounded->size + 1, stack ); 

      // if we're running the task in the background, then
      // we shouldn't return a pid, because the main loop will
      // wait on any non-zero pid we return from here.
      return strcmp( action, "bg" ) == 0 ? ( pid_t ) 0 : stack;
    }
  }
  // show process ids?
  else if ( strcmp( action, "showpids" ) == 0 )
  {
    built_in_showpids( command, pids );
  }
  // show the user their history
  else if ( strcmp( action, "history" ) == 0 )
  {
    built_in_history( command, history );
  }
  // change working directory
  else if ( strcmp( action, "cd" ) == 0 )
  {
    built_in_cd( command );
  }
  // print working directory
  else if ( strcmp( action, "pwd" ) == 0 )
  {
    build_in_pwd( command );
  }
  // check for binaries
  else
  {
    return command_exec( command );
  }

  return ( pid_t ) 0;
}



