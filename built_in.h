#ifndef MSH_BUILT_INS_H
#define MSH_BUILT_INS_H

#include "command.h"
#include "list.h"

command_t* built_in_run_history( 
    command_t* command, 
    const list_t* history 
);

void built_in_showpids( const command_t* command, const list_t* pids );

void built_in_history( const command_t* command, const list_t* history );

void built_in_cd( const command_t* command );

void build_in_pwd( const command_t* command );

#endif

