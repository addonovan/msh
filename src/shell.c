/*
 * Name: Austin Donovan
 * Id:   1001311620
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "shell.h"
#include "oo.h"

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

    // just forward all other signals to the current process 
    default:
      kill( g_active_shell->current_pid, signal );
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
  list_init( &this->cmd_history ); 
  list_init( &this->pid_history );
  list_init( &this->background_pids );

  this->current_pid = ( pid_t ) 0;

  handler_init( &this->handler, &signal_handler );
}

void shell_destroy( shell_t* this )
{
  list_destroy( &this->cmd_history ); 
  list_destroy( &this->pid_history );
  list_destroy( &this->background_pids );

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

  // suspend the process
  kill( this->current_pid, SIGTSTP );

  // push it onto the stack of waiting processes
  pid_t* pid = malloc( sizeof( pid_t ) );
  *pid = this->current_pid;
  list_push( &this->background_pids, pid );

  // tell the user (\r to cover up the ^Z)
  printf( "\r[%d]  + %d suspended\n", this->background_pids.size, *pid );

  this->current_pid = 0;
}

pid_t shell_resume( shell_t* this )
{
  // get the last pid from our list
  pid_t* raw = list_pop( &this->background_pids );

  // no suspended jobs? tell the user, but then stop
  if ( raw == NULL )
  {
    printf( "No stopped jobs\n" );
    return ( pid_t ) 0;
  }

  pid_t pid = *raw;
  
  // we took ownership of the pid, so we have to free it
  free( raw );

  // notify the user
  printf( "[%d]  - %d continued\n", this->background_pids.size + 1, pid );

  // tell the process to resume
  kill( pid, SIGCONT );

  return pid;
}

void shell_wait( shell_t* this )
{
  // if we don't have an active process, then just don't
  // do anything
  if ( this->current_pid == 0 ) return;

  // wait until the process exits
  int status = 0;
  waitpid( this->current_pid, &status, 0 );

  // this means that the process was suspended
  if ( this->current_pid == 0 ) return;

  // if the program died by signal, print the signal
  // (but don't print the ^Z and resume signals, those
  // aren't what we're looking for)
  if ( WIFSIGNALED( status ) )
  {
    int signal = WTERMSIG( status );
    const char* signal_text = strsignal( signal );

    // tell the user the pid, the signal text, and the signal value
    // (\r to cover up a possible ^C)
    printf( KRED "\r! " );
    printf( "[%d] %s (%d)", this->current_pid, signal_text, signal );
    printf( KNRM "\n" );
  }
  // if the program emmitted a non-zero exit code
  else if ( WIFEXITED( status ) && WEXITSTATUS( status ) != 0 )
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
  list_push( &this->cmd_history, command );

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
    pid_t* pid = malloc( sizeof( pid_t ) );
    *pid = command_exec( command );
    this->current_pid = *pid;
    list_push( &this->pid_history, pid );
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

  char* dir; 
  char* arg = list_get( &command->tokens, 1 );

  // no arguments? send them home
  if ( arg == NULL )
  {
    dir = getenv( "HOME" );
  }
  // otherwise, navigate to the given directory
  else
  {
    dir = arg;
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

unsigned int get_print_count( 
    unsigned int default_value,
    const command_t* command, 
    const list_t* list 
)
{
  unsigned int count = default_value;

  list_iter_t iter = list_iter_create( &command->tokens );
  list_iter_jump( &iter, 1 );

  // parse arguments
  const char* param;
  while ( ( param = list_iter_pop( &iter ) ) != NULL )
  {
    if ( strcmp( param, "--all" ) == 0
      || strcmp( param, "-a" ) == 0 )
    {
      count = list->size;
    }
    else if ( strcmp( param, "--count" ) == 0
           || strcmp( param, "-c" ) == 0 )
    {
      param = list_iter_pop( &iter );
      if ( param == NULL ) return 0;

      count = strtol( param, NULL, 0 );
      if ( count <= 0 ) return 0;
    }
  }

  // make sure we don't go into negative offsets
  if ( count > list->size )
  {
    count = list->size;
  }

  return count;
}

void shell_bi_history( const shell_t* this, const command_t* command )
{
  unsigned int count = get_print_count( 15, command, &this->cmd_history );

  // jump ahead to the first element we should print
  list_iter_t iter = list_iter_create( &this->cmd_history );
  list_iter_jump( &iter, this->cmd_history.size - count );

  // actually print out the history item
  unsigned int i;
  for ( i = 0; i < count; i++ )
  {
    command_t* command = list_iter_pop( &iter );
    printf( "%d: %s\n", i, command->string );
  }
}

void shell_bi_showpids( const shell_t* this, const command_t* command )
{
  unsigned int count = get_print_count( 10, command, &this->pid_history );

  // jump ahead to the first element we should print
  list_iter_t iter = list_iter_create( &this->pid_history );
  list_iter_jump( &iter, this->pid_history.size - count );

  // actually print out the history item
  unsigned int i;
  for ( i = 0; i < count; i++ )
  {
    pid_t* pid = list_iter_pop( &iter );
    printf( "%d: %d\n", i, *pid );
  }
}

void shell_bi_run_history( shell_t* this, const command_t* command )
{
  unsigned int index;

  const char* name = command_get_name( command );

  // !! => last item
  if ( strcmp( name, "!!" ) == 0 )
  {
    index = this->cmd_history.size - 1;
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
    if ( this->cmd_history.size <= 15 )
    {
      index = 0;
    }
    else
    {
      index = this->cmd_history.size - 16;
    }

    // then add our offset from the user
    index += strtol( name + 1, NULL, 0 );
  }

  list_iter_t iter = list_iter_create( &this->cmd_history );
  list_iter_jump( &iter, index );

  // TODO perform a deep-copy of the memory, so the lists
  // don't reference the same items
  command_t* newcmd = copy( ( command_t* ) list_iter_pop( &iter ) );

  // remove the most recent command (i.e. the one that got us
  // here) from the command history, so it will be replaced
  // by the actual item that was run
  command_t* oldcmd = list_pop( &this->cmd_history );
  delete( command, oldcmd );

  // (indirectly) recursively let this new command be executed
  shell_run_command( this, newcmd );

  // what to do with the result?
  // I guess just do nothing, because if we somehow previously
  // ran a command to exit, then we wouldn't have said item in
  // our history, as the shell would've exited
}

