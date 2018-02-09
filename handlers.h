#ifndef MSH_HANDLERS_H
#define MSH_HANDLERS_H

#include <signal.h>
#include <unistd.h>

typedef struct sigaction sigaction_t;
typedef struct handler_t handler_t;

struct handler_t
{
  sigaction_t action;
  void ( *handler )( int );
};

handler_t* handler_create( void ( *handler )( int ) );

void handler_free( handler_t* );

#endif

