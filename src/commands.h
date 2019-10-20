#ifndef _COMMANDS_H
#define _COMMANDS_H

#define MAX_COMMANDS 64

#include "stdint.h"

struct command {
    char *name;
    uint8_t (*func)(int argc, char *argv[]);
    char *description;
};

extern struct command *commands[MAX_COMMANDS];

struct command *command_create (char *name, uint8_t (*func)(int argc, char *argv[]), char *description);

void init_commands ();

#endif