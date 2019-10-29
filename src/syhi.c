#include "syhi.h"
#include "gordon.h"


const struct syhi c_highlights[] = {
    { "void", SH_BLUE }, { "int", SH_BLUE }, { "float", SH_MAGENTA }, { "#include", SH_MAGENTA }, { "#define", SH_MAGENTA },
    { "for", SH_MAGENTA }, { "while", SH_MAGENTA }, { "if", SH_MAGENTA }, { "else", SH_MAGENTA }, { "return", SH_MAGENTA }, { "break", SH_MAGENTA }, { "continue", SH_MAGENTA },
    { "struct", SH_BLUE }, { "int", SH_BLUE }, { "uint8_t", SH_BLUE },
    { "uint16_t", SH_BLUE }, { "uint32_t", SH_BLUE }, { "int8_t", SH_BLUE }, { "int16_t", SH_BLUE }, 
    { "int32_t", SH_BLUE }, { "//", SH_GREEN, 1 }, { "/*", SH_GREEN }, { "*/", SH_GREEN }

};

uint8_t syhi_color_c (char *p) {
    for(int x = 0; x < sizeof(c_highlights)/sizeof(struct syhi); x++) {
        if(strcmp(p, c_highlights[x].syntax) == 0) {

            return c_highlights[x].highlight;
        }
    }
    return 1;
}

uint8_t syhi_color (char *p) {
    if(strcmp(files[selected_file]->extension, ".c") == 0) {
        return syhi_color_c(p);
    }
    return 1;
}