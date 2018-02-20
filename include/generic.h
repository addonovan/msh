#ifndef __MSH_GENERIC_H__
#define __MSH_GENERIC_H__

#include <sys/types.h>
#define HEADER_ONLY
#define TYPE pid_t
#define COPY_VALUE
#include "clib/list.h"

#define HEADER_ONLY
#define TYPE int
#define COPY_VALUE
#include "clib/list.h"

typedef char* string;
#define HEADER_ONLY
#define TYPE string
#define COPY_VALUE
#include "clib/list.h"

#define HEADER_ONLY
#define TYPE char
#define COPY_VALUE
#include "clib/list.h"

#include "command.h"
#define HEADER_ONLY
#define TYPE command_t
#include "clib/list.h"

#endif

