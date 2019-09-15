#ifndef _GORDON_H
#define _GORDON_H

#include <stdio.h>
#include <curses.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILES 16

uint16_t SCREEN_WIDTH;
uint16_t SCREEN_HEIGHT;

extern uint16_t cursor_x;
extern uint16_t cursor_y;

struct gfile {
    FILE *file_pointer;
    char *path;
    char *data;
    size_t size;

    size_t cursor_pos;
    size_t screen_top;
};

extern struct gfile *files[MAX_FILES];

extern int selected_file;

struct gfile *gfile_open (char *path);

void render_tabsel ();

void render_file (struct gfile *f);

#endif