#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "handlers.h"

handler_t* handler_create( void ( *handler )( int ) )
{
  handler_t* this = calloc( 1, sizeof( handler_t ) );
  this->action.sa_handler = handler;

  if ( sigaction( SIGINT, &this->action, NULL ) < 0 )
  {
    perror( "sigaction: " );
    free( this );
    return NULL;
  }
  
  if ( sigaction( SIGTSTP, &this->action, NULL ) < 0 )
  {
    perror( "sigaction: " );
    free( this );
    return NULL;
  }

  return this;
}

void handler_free( handler_t* this )
{
  free( this );
}

