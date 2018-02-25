/*
 * Name: Austin Donovan
 * Id:   1001311620
 */

#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "shell.h"
#include "clib/memory.h"

// terminal colors
#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

//
// Static
//

static shell_t* g_active_shell = NULL;

/**
 * The signal handler.
 *
 * This will forward the [signal] to the [g_active_shell]
 * for it to handle.
 */
static void signal_handler( int signal )
{
  // no active shell? don't do anything
  if ( g_active_shell == NULL ) return;

  // shell not running anything? don't forward the signal
  if ( g_active_shell->current_pid == 0 ) return;

  switch ( signal )
  {
    // special handler for ^Z
    case SIGTSTP:
      shell_suspend( g_active_shell );
      break;

    // just forward all other signals to the current process's pgroup
    default:
      kill( -g_active_shell->current_pid, signal );
      break;
  }
}

//
// Declarations
//

/**
 * Tries to run a built-in shell command, will return `true`
 * if there was a matching command, otherwise `false`.
 */
bool shell_run_bi( shell_t*, const command_t* command );

/**
 * Built-in shell command for changing directories
 */
void shell_bi_cd( const shell_t*, const command_t* command );

/**
 * Built-in shell command for printing the current
 * working directory
 */
void shell_bi_pwd( const shell_t*, const command_t* command );

/**
 * Built-in shell command for printing the shell's
 * history.
 */
void shell_bi_history( const shell_t*, const command_t* command );

/**
 * Built-in shell command for printing the shell's
 * pid history.
 */
void shell_bi_showpids( const shell_t*, const command_t* command );

/**
 * Built-in shell command for running a command from
 * the shell's history.
 */
void shell_bi_run_history( shell_t*, const command_t* command );

//
// Definitions
//

void shell_init( shell_t* this )
{
  this->cmd_history = list_u(command_t);
  this->pid_history = list_u(pid_t);
  this->background_pids = list_u(pid_t);

  this->current_pid = ( pid_t ) 0;

  handler_init( &this->handler, &signal_handler );
}

void shell_destroy( shell_t* this )
{
  delete( this->cmd_history );
  delete( this->pid_history );
  delete( this->background_pids );

  this->current_pid = ( pid_t ) 0;

  handler_destroy( &this->handler );

  // unmark ourselves from the active shell position
  // if we are
  if ( g_active_shell == this )
  {
    g_active_shell = NULL;
  }
}

void shell_set_active( const shell_t* this )
{
  g_active_shell = ( shell_t* ) this;
}

void shell_suspend( shell_t* this )
{
  // if we aren't running anything, just don't do anything
  if ( this->current_pid == 0 ) return;
  
  // push it onto the stack of waiting processes
  pid_t pid = this->current_pid;
  this->background_pids->fun->push( this->background_pids, pid );

  // suspend the process and it's process group
  kill( -this->current_pid, SIGTSTP );

  // tell the user
  printf( "\r[%d]  + %d suspended\n", this->background_pids->size, pid );

  this->current_pid = 0;
}

pid_t shell_resume( shell_t* this )
{
  if ( this->background_pids->size == 0 )
  {
    printf( "No stopped jobs\n" );
    return ( pid_t ) 0;
  }

  unsigned int size = this->background_pids->size;
  pid_t pid = this->background_pids->fun->pop( this->background_pids );

  // notify the user
  printf( "[%d]  - %d continued\n", size, pid );

  // tell the process (and its group) to resume
  kill( -pid, SIGCONT );

  return pid;
}

void shell_wait( shell_t* this )
{
  // if we don't have an active process, then just don't
  // do anything
  if ( this->current_pid == 0 ) return;

  // wait until the process exits
  int status;
  waitpid( this->current_pid, &status, 0 );

  // if the program died by signal, print the signal
  if ( WIFSIGNALED( status ) )
  {
    const char* signal_text = strsignal( WTERMSIG( status ) );

    printf( KRED "! [%d] %s\n" KNRM, this->current_pid, signal_text );
  }

  // if the program emmitted a non-zero exit code
  if ( WIFEXITED( status ) && WEXITSTATUS( status ) != 0 )
  {
    printf( KRED "! " KNRM );
  }

  // child process is now dead
  this->current_pid = ( pid_t ) 0;
}

bool shell_run_command( shell_t* this, command_t* command )
{
  // absolutely do not run anything if there is still a
  // foreground process
  if ( this->current_pid != 0 ) return true;

  // add the command to our history
  this->cmd_history->fun->enqueue( this->cmd_history, command );

  const char* name = command_get_name( command );   

  if ( strcmp( name, "exit" ) == 0
    || strcmp( name, "quit" ) == 0 )
  {
    return false;
  }
  else if ( strcmp( name, "fg" ) == 0 )
  {
    // resume the next suspended process, and set it as the
    // foreground process (i.e. wait for it)
    pid_t pid = shell_resume( this );
    this->current_pid = pid;
  }
  else if ( strcmp( name, "bg" ) == 0 )
  {
    // just resume it, don't set it as the foreground process
    shell_resume( this );
  }
  // try to run a built-in command, if this fails, then
  // finally try to run the command by searching paths
  else if ( !shell_run_bi( this, command ) )
  {
    // set the currently running process (in case a signal arrives,
    // so the correct process will receive it)
    pid_t pid = command_exec( command );
    this->current_pid = pid;
    this->pid_history->fun->enqueue( this->pid_history, pid );
  }

  return true;  
}

//
// Built-in Command definitions
//

bool shell_run_bi( shell_t* this, const command_t* command )
{
  const char* name = command_get_name( command );

  // just test the command's name against the built-in ones

  if ( strcmp( name, "cd" ) == 0 )
  {
    shell_bi_cd( this, command );
    return true;
  }
  else if ( strcmp( name, "pwd" ) == 0 )
  {
    shell_bi_pwd( this, command );
    return true;
  }
  else if ( strcmp( name, "history" ) == 0 )
  {
    shell_bi_history( this, command );
    return true;
  }
  else if ( strcmp( name, "showpids" ) == 0 )
  {
    shell_bi_showpids( this, command );
    return true;
  }
  else if ( name[ 0 ] == '!' )
  {
    shell_bi_run_history( this, command );
    return true;
  }

  // couldn't find a command, so oh well
  return false;
}

void shell_bi_cd( const shell_t* this, const command_t* command )
{
  // unused, just here for symmetry
  ( void )( this );

  const char* dir; 

  if ( command->tokens->size < 2 )
  {
    dir = getenv( "HOME" );
  }
  else
  {
    dir = command->tokens->fun->get( command->tokens, 1 );
  }

  chdir( dir );
}

void shell_bi_pwd( const shell_t* this, const command_t* command )
{
  // unused, just here for symmetry
  ( void )( this );
  ( void )( command );

  char cwd[ 1024 ]; // I really hope it's not longer than this
  if ( getcwd( cwd, sizeof( cwd ) ) != NULL ) 
  {
    printf( "%s\n", cwd );
  }
  else
  {
    perror( "getcwd() error\n" );
  }
}

void shell_bi_history( const shell_t* this, const command_t* command )
{
  unsigned int count = 15;

  if ( this->cmd_history->size < count )
  {
    count = this->cmd_history->size;
  }

  // jump ahead to the first element we should print
  unsigned int offset = this->cmd_history->size - count;

  typeof( this->cmd_history->fun->get ) get = this->cmd_history->fun->get;

  int index = offset;
  for ( ; index < this->cmd_history->size; index++ )
  {
    const command_t* command = get( this->cmd_history, index );
    printf( "%d: %s\n", index - offset, command->string );
  }
}

void shell_bi_showpids( const shell_t* this, const command_t* command )
{
  unsigned int count = 10;

  if ( this->pid_history->size < count )
  {
    count = this->pid_history->size;
  }

  // jump ahead to the first element we should print
  unsigned int index = this->pid_history->size - count;

  typeof( this->pid_history->fun->get ) get = this->pid_history->fun->get;

  unsigned int offset = index;

  for ( ; index < this->pid_history->size; index++ )
  {
    pid_t pid = get( this->pid_history, index );
    printf( "%d: %d\n", index - offset, pid );
  }
}

void shell_bi_run_history( shell_t* this, const command_t* command )
{
  unsigned int index;

  const char* name = command_get_name( command );

  // !! => last item
  if ( strcmp( name, "!!" ) == 0 )
  {
    index = this->cmd_history->size - 1;
  }
  // !+<num> => absolute offset
  else if ( name[ 1 ] == '+' )
  {
    index = strtol( name + 2, NULL, 0 );
  }
  // !<num> => offset since 15th-to-last item
  else
  {
    // start at the 15th-to-last item
    if ( this->cmd_history->size <= 15 )
    {
      index = 0;
    }
    else
    {
      index = this->cmd_history->size - 16;
    }

    // then add our offset from the user
    index += strtol( name + 1, NULL, 0 );
  }

  // the original command we need to duplicate
  const command_t* srccmd = this->cmd_history->fun->get( 
      this->cmd_history, 
      index 
  );

  // make a duplicate of the srccmd
  command_t* newcmd = malloc( sizeof( command_t* ) );
  command_copy( newcmd, srccmd );

  // remove the most recent command (i.e. the one that got us
  // here) from the command history, so it will be replaced
  // by the actual item that was run
  command_t* oldcmd = this->cmd_history->fun->pop_back( this->cmd_history );
  command_destroy( oldcmd );
  free( oldcmd );

  // (indirectly) recursively let this new command be executed
  shell_run_command( this, newcmd );

  // what to do with the result?
  // I guess just do nothing, because if we somehow previously
  // ran a command to exit, then we wouldn't have said item in
  // our history, as the shell would've exited
}

