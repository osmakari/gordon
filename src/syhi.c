#include "syhi.h"


const struct syhi c_highlights[] = {
    { "void", SH_BLUE }, { "int", SH_BLUE }, { "#include", SH_MAGENTA }, { "#define", SH_MAGENTA },
    { "for", SH_BLUE }, { "struct", SH_BLUE }, { "int", SH_BLUE }, { "uint8_t", SH_BLUE },
    { "uint16_t", SH_BLUE }, { "uint32_t", SH_BLUE }, { "int8_t", SH_BLUE }, { "int16_t", SH_BLUE }, 
    { "int32_t", SH_BLUE }, { "//", SH_GREEN }, { "/*", SH_GREEN }, { "*/", SH_GREEN },

};

uint8_t syhi_color_c (char *p) {
    for(int x = 0; x < sizeof(c_highlights)/sizeof(struct syhi); x++) {
        if(strcmp(p, c_highlights[x].syntax) == 0) {

            return c_highlights[x].highlight;
        }
    }
    return 1;
}