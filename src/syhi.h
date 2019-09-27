#ifndef SYHI_H
#define SYHI_H

#define SH_CYAN    10
#define SH_GREEN   11
#define SH_BLUE    12
#define SH_MAGENTA 13
#define SH_RED     14

#include <stdint.h>
#include <stdio.h>
#include <string.h>

struct syhi {
    const char *syntax;
    uint8_t highlight;
    uint8_t until; // 0 = this word, 1 = until newline, 2 = whole line
};

extern const struct syhi c_highlights[];

uint8_t syhi_color_c (char *p);

#endif