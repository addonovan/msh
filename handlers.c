#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "handlers.h"

typedef struct sigaction sigaction_t;

void handle_init()
{
  sigaction_t action;
  memset( &action, 0, sizeof( sigaction_t ) );

  action.sa_handler = &handle_signal;

  if ( sigaction( SIGINT, &action, NULL ) < 0 )
  {
    perror( "sigaction: " );
    return;
  }
}

void handle_signal( int signal )
{
  switch ( signal )
  {
    case SIGINT:
      handle_sigint();
      break;
  }
}

void handle_sigint()
{
  printf( "Why would you try to kill me? :(\n" );
}

