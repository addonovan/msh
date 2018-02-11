/*
 * Name: Austin Donovan
 * Id:   1001311620
 */

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "handlers.h"

void handler_init( handler_t* this, void ( *sig_handler )( int ) )
{
  memset( &this->action, 0, sizeof( this->action ) );
  this->action.sa_handler = sig_handler;

  if ( sigaction( SIGINT, &this->action, NULL ) < 0 )
  {
    perror( "sigaction: " );
  }
  
  if ( sigaction( SIGTSTP, &this->action, NULL ) < 0 )
  {
    perror( "sigaction: " );
  }
}

void handler_destroy( handler_t* this )
{
  // TODO unregister for signal handlers
}

