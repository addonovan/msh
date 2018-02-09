#ifndef MSH_HANDLERS_H
#define MSH_HANDLERS_H

#include <signal.h>

void handle_init();

void handle_signal( int signal );

void handle_sigint();

#endif

