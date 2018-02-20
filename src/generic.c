#include "generic.h"

#include <sys/types.h>
#define IMPLEMENTATION_ONLY
#define TYPE pid_t
#define COPY_VALUE
#include "clib/list.h"

#define IMPLEMENTATION_ONLY
#define TYPE int
#define COPY_VALUE
#include "clib/list.h"

typedef char* string;
#define IMPLEMENTATION_ONLY
#define TYPE string
#define COPY_VALUE
#include "clib/list.h"

#define IMPLEMENTATION_ONLY
#define TYPE char
#define COPY_VALUE
#include "clib/list.h"

#include "command.h"
#define IMPLEMENTATION_ONLY
#define TYPE command_t
#include "clib/list.h"

