/*
 * Name: Austin Donovan
 * Id:   1001311620
 */


#ifndef MSH_HANDLERS_H
#define MSH_HANDLERS_H

#include <signal.h>
#include <unistd.h>

typedef struct sigaction sigaction_t;
typedef struct handler_t handler_t;

/**
 * A signal handler structure.
 */
struct handler_t
{
  sigaction_t action;
  void ( *handler )( int );
};

/**
 * Creates a signal handler using the given [handler]
 * pointer as the callback.
 */
void handler_init( handler_t*, void ( *sig_handler )( int ) );

/**
 * Frees the given handler from memory.
 */
void handler_destroy( handler_t* );

#endif

