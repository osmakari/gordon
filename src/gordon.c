#include "gordon.h"
#include "syhi.h"
#include <locale.h>

#define KEY_CTRL_PAGEUP 563
#define KEY_CTRL_PAGEDOWN 558
#define KEY_BACKSP 127
#define KEY_DEL 330
#define KEY_RET 13
#define KEY_PAGEDOWN 338
#define KEY_PAGEUP 339

struct gfile *files[MAX_FILES] = { NULL };

uint16_t cursor_x = 0;
uint16_t cursor_y = 0;

int selected_file = -1;

uint8_t mode = 0; // 0 = insert, 1 = command, 2 = buttondebug

int main (int argc, char *argv[]) {
    
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    
    keypad(stdscr, TRUE);
    nonl();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_CYAN);
    init_pair(3, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(4, COLOR_BLACK, COLOR_WHITE);

    // Syntax highlight COLORS
    init_pair(SH_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(SH_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(SH_BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(SH_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(SH_RED, COLOR_RED, COLOR_BLACK);

    getmaxyx(stdscr, SCREEN_HEIGHT, SCREEN_WIDTH);
    clear();

    refresh();

    

    render_tabsel();
    uint8_t swi = 0;

    for(int x = 1; x < argc; x++) {
        if(swi == 0) {
            if(argv[x][0] == '-') {
                // arg
                if(argv[x][1] == 'b') {
                    mode = 2;
                }
            }
            else {
                // Open file
                gfile_open(argv[x]);
            }
        }
        
    }

    if(selected_file < 0) {
        gfile_open_empty();
    }

    char cbuf[1024] = { 0 };
    int16_t cbufp = 0;


    while(1) {
        int ip = getch();
        if(mode == 2) {
            printw("%i", ip);
            continue;
        }

        if(ip == 27) {
            mode = (mode == 1 ? 0 : 1);
            if(mode == 1) {
                // Render command line
                render_commandline();
                memset(cbuf, 0, 1024);
            }
            else if(mode == 0) {
                clear();
                render_tabsel();
                render_file(files[selected_file]);
            }
            continue;
        }
        
        if(mode == 0) {
            if(ip == KEY_CTRL_PAGEUP) {
                if(selected_file < MAX_FILES && files[selected_file + 1] != NULL) {
                    selected_file++;
                }
                render_tabsel();
                render_file(files[selected_file]);
            }
            else if(ip == KEY_CTRL_PAGEDOWN) {
                if(selected_file > 0 && files[selected_file - 1] != NULL) {
                    selected_file--;
                }
                render_tabsel();
                render_file(files[selected_file]);
            }
            else if(ip == KEY_LEFT) {
                if(files[selected_file]->cursor_pos > 0) {
                    //cursor_x--;
                    files[selected_file]->cursor_pos--;
                }
                //move(cursor_y, cursor_x);
                //refresh();
                render_file(files[selected_file]);
            }
            else if(ip == KEY_RIGHT) {
                if(files[selected_file]->data[files[selected_file]->cursor_pos] != 0 && files[selected_file]->cursor_pos < files[selected_file]->size) {
                    files[selected_file]->cursor_pos++;
                }
                //move(cursor_y, cursor_x);
                //refresh();
                render_file(files[selected_file]);
            }
            else if(ip == KEY_DOWN) {
                size_t ll = files[selected_file]->cursor_pos;
                uint8_t f = 0;
                size_t rc = 0;
                while(files[selected_file]->data[ll] != '\n') {
                    if(ll > files[selected_file]->size) {
                        ll = files[selected_file]->size;
                        f = 1;
                        //cursor_x = rc;
                        files[selected_file]->cursor_pos = ll;
                        break;
                    }
                    ll++;
                    rc++;
                }
                if(!f) {
                    ll++;
                    size_t xp = 0;
                    while(files[selected_file]->data[ll] != '\n' && xp != cursor_x) {
                        if(ll > files[selected_file]->size) {
                            ll = files[selected_file]->size - 1;
                            //cursor_x = xp;
                            files[selected_file]->cursor_pos = ll;
                            break;
                        }
                        ll++;
                        xp++;
                    }
                    //cursor_x = xp;
                    //cursor_y++;
                    files[selected_file]->cursor_pos = ll;
                }

                if(getcursorline(files[selected_file]) > SCREEN_HEIGHT - 2) {
                    int32_t lp = files[selected_file]->screen_top;
                    while(files[selected_file]->data[files[selected_file]->screen_top] != '\n') {
                        files[selected_file]->screen_top++;
                    }
                    files[selected_file]->screen_top++;
                    clear();
                    render_tabsel();
                }

                //move(cursor_y, cursor_x);
                //refresh();
                render_file(files[selected_file]);
            }
            else if(ip == KEY_UP) {
                int32_t ll = files[selected_file]->cursor_pos - 1;
                uint8_t f = 0;
                while(files[selected_file]->data[ll] != '\n') {
                    if(ll < 0) {
                        ll = 0;
                        f = 1;
                        //cursor_x = 0;
                        files[selected_file]->cursor_pos = ll;
                        break;
                    }
                    ll--;
                }
                
                
                if(!f) {
                    if(files[selected_file]->data[ll - 1] != '\n') {
                        ll--;
                    }
                    size_t line_len = 0;
                    int32_t sp_p = ll;
                    while(files[selected_file]->data[sp_p] != '\n' && sp_p >= 0) {
                        line_len++;
                        sp_p--;
                    }

                    int32_t xp = line_len - 1;
                    while(files[selected_file]->data[ll] != '\n' && xp != cursor_x) {
                        if(ll < 0) {
                            ll = 0;
                            //cursor_x = xp;
                            break;
                        }
                        ll--;
                        xp--;
                    }
                    //cursor_x = xp;
                    //cursor_y--;
                    files[selected_file]->cursor_pos = ll;
                }

                if(getcursorline(files[selected_file]) < 0) {
                    files[selected_file]->screen_top = files[selected_file]->cursor_pos;
                    while(files[selected_file]->data[files[selected_file]->screen_top] != '\n' && files[selected_file]->screen_top != 0) {
                        files[selected_file]->screen_top--;
                    }
                    clear();
                    render_tabsel();
                    //files[selected_file]->screen_top++;
                }

                render_file(files[selected_file]);
                //move(cursor_y, cursor_x);
                //refresh();
            }
            else if(ip == KEY_PAGEDOWN) {
                uint32_t lc = 0;
                while(files[selected_file]->cursor_pos < files[selected_file]->size && files[selected_file]->data[files[selected_file]->cursor_pos] != 0) {
                    if(files[selected_file]->data[files[selected_file]->cursor_pos] == '\n') {
                        lc++;
                        if(lc == SCREEN_HEIGHT - 1) {
                            files[selected_file]->cursor_pos++;
                            break;
                        }

                    }
                    files[selected_file]->cursor_pos++;
                }
                render_file(files[selected_file]);
            }
            else if(ip == KEY_BACKSP) {
                if(files[selected_file]->cursor_pos > 0) {
                    uint8_t clr = 0;
                    if(files[selected_file]->data[files[selected_file]->cursor_pos - 1] == '\n') {
                        clr = 1;
                    }
                    for(int x = files[selected_file]->cursor_pos - 1; x < files[selected_file]->size - 1; x++) {
                        files[selected_file]->data[x] = files[selected_file]->data[x + 1];
                        files[selected_file]->data[x + 1] = 0;
                    }
                    files[selected_file]->size--;
                    files[selected_file]->cursor_pos--;
                    move(getcursorline(files[selected_file]) + 1, 0);
                    for(int x = 0; x < SCREEN_WIDTH - 1; x++) {
                        printw(" ");
                    }
                    if(clr) {
                        clear();
                        render_tabsel();
                    }
                    render_file(files[selected_file]);
                }
            }
            else if(ip == KEY_DEL) {
                if(files[selected_file]->cursor_pos < files[selected_file]->size - 1) {
                    for(int x = files[selected_file]->cursor_pos; x < files[selected_file]->size - 2; x++) {
                        files[selected_file]->data[x] = files[selected_file]->data[x + 1];
                    }
                    files[selected_file]->size--;
                    
                    render_file(files[selected_file]);
                }
            }
            else {

                if(ip == KEY_RET) {
                    ip = '\n';
                }

                if(files[selected_file]->allocated < files[selected_file]->size + 2) {
                    files[selected_file]->data = realloc(files[selected_file]->data, files[selected_file]->allocated + 64);
                    files[selected_file]->allocated += 64;
                    memset(files[selected_file]->data + files[selected_file]->size, 0, 64);
                }
                for(int x = files[selected_file]->size - 1; x > files[selected_file]->cursor_pos; x--) {
                    files[selected_file]->data[x] = files[selected_file]->data[x - 1];
                }
                files[selected_file]->data[files[selected_file]->cursor_pos] = (char)ip;
                files[selected_file]->cursor_pos++;
                files[selected_file]->size++;
                render_file(files[selected_file]);
                
                
            }
        }
        else if(mode == 1) {
            attron(COLOR_PAIR(4));
            if(ip == KEY_RET) {
                command_parse(cbuf);
                render_commandline();
                memset(cbuf, 0, cbufp + 1);
                cbufp = 0;
                continue;
            }
            if(ip == KEY_BACKSP) {
                
                if(cbufp > 0) {
                    cbufp--;
                    cbuf[cbufp] = 0; 
                    move(SCREEN_HEIGHT - 1, cbufp + 1);
                    printw(" ");
                    move(SCREEN_HEIGHT - 1, cbufp + 1);
                    refresh();
                }
                
                continue;
            }
            
            printw("%c", ip);
            cbuf[cbufp] = ip;
            cbufp++;
            attroff(COLOR_PAIR(4));
        }
    }

    return 0;
}

void render_tabsel () {
    
    attron(COLOR_PAIR(2));
    for(int x = 0; x < SCREEN_WIDTH; x++) {
        move(0, x);
        if(x == 0) {
            printw("<");
            continue;
        }
        if(x == SCREEN_WIDTH - 1) {
            printw(">");
            continue;
        }
        printw(" ");
    }
    
    
    uint16_t offset = 1;
    for(int x = 0; x < MAX_FILES; x++) {
        if(files[x] == NULL)
            break;

        if(x == selected_file) {
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(3));
        }

        move(0, offset);
        if(strlen(files[x]->path) > 0) {
            printw("%s", files[x]->path);

        }
        else {
            printw("*new file*");
        }
        offset += strlen(files[x]->path) + 1;

        if(x == selected_file) {
            attroff(COLOR_PAIR(3));
            attron(COLOR_PAIR(2));
        }
    }
    attroff(COLOR_PAIR(2));
    refresh();
}

struct gfile *gfile_open_empty () {
    for(int x = 0; x < MAX_FILES; x++) {
        if(files[x] == NULL) {
            struct gfile *gg = (struct gfile*)malloc(sizeof(struct gfile));
            gg->path = "";
            gg->size = 1;
            gg->exists = 0;
            gg->data = (char*)malloc(gg->size + 64);
            gg->allocated = 64;
            gg->cursor_pos = 0;
            gg->screen_top = 0;
            files[x] = gg;
            selected_file = x;

            clear();

            render_tabsel();

            refresh();

            render_file(gg);

            return gg;
        }
    }
    return NULL;
}

struct gfile *gfile_open (char *path) {
    // Open file

    FILE *f = fopen(path, "r");

    if(f == NULL) {
        for(int x = 0; x < MAX_FILES; x++) {
            if(files[x] == NULL) {
                struct gfile *gg = (struct gfile*)malloc(sizeof(struct gfile));
                gg->path = (char*)malloc(strlen(path) + 1);
                memcpy(gg->path, path, strlen(path) + 1);
                gg->size = 1;
                gg->exists = 0;
                gg->data = (char*)malloc(gg->size + 64);
                gg->allocated = 64;
                gg->cursor_pos = 0;
                gg->screen_top = 0;
                files[x] = gg;
                selected_file = x;

                clear();

                render_tabsel();

                refresh();

                render_file(gg);

                return gg;
            }
        }
    }

    fseek(f, 0L, SEEK_END);
    size_t sz = ftell(f);
    fseek(f, 0L, SEEK_SET);

    for(int x = 0; x < MAX_FILES; x++) {
        if(files[x] == NULL) {
            struct gfile *gg = (struct gfile*)malloc(sizeof(struct gfile));
            gg->path = (char*)malloc(strlen(path) + 1);
            memcpy(gg->path, path, strlen(path) + 1);
            gg->size = sz;
            gg->exists = 1;
            gg->data = (char*)malloc(gg->size + 64);
            gg->allocated = sz + 64;
            gg->cursor_pos = 0;
            gg->screen_top = 0;
            fread(gg->data, gg->size, 1, f);
            files[x] = gg;
            selected_file = x;

            clear();

            render_tabsel();

            refresh();

            render_file(gg);
            fclose(f);

            return gg;
        }
    }
    fclose(f);
    return NULL;
}

void render_file (struct gfile *f) {
    if(f == NULL)
        return;

    move(1, 0);
    size_t x = f->screen_top;
    uint16_t rw = 0;
    uint8_t sh_state = 0;
    size_t sh_point = 0;
    uint16_t sh_buf_p = 0;
    char fbf[128] = { 0 };
    uint8_t at = 0;


    int x_pos = 0;

    uint8_t x_offset = 0;

    size_t xxx = f->cursor_pos - 1;
    if(f->cursor_pos == 0)
        xxx = 0;
    uint16_t llen = 0;
    while(xxx > 0 && f->data[xxx] != '\n') {
        llen++;
        xxx--;
    }

    if(llen > SCREEN_WIDTH - 2) {
        x_offset = llen - (SCREEN_WIDTH - 2);
    }

    while(x < f->size) {
        if(f->data[x] == 0)
            break;


        if(sh_state == 0) {
            sh_point = x;
            
            while(f->data[sh_point] != '\n' && f->data[sh_point] != ' ' && f->data[sh_point] != '\t' && f->data[sh_point] != ',' && f->data[sh_point] != ';' && f->data[sh_point] != '(' && sh_point < f->size) {
                fbf[sh_buf_p++] = f->data[sh_point++];
            }
            at = syhi_color_c(fbf);
            if(at >= 10) {
                attron(COLOR_PAIR(at));
            }
            
            
            sh_state = 1;
        }
        
        if(sh_state == 1) {
            if(f->data[x] == '\n' || f->data[x] == ' ' || f->data[x] == '\t' || f->data[x] == ',' || f->data[x] == ';' || f->data[x] == '(') {
                if(at >= 10) {
                    attroff(COLOR_PAIR(at));
                }
                sh_state = 0;
                sh_buf_p = 0;
                memset(fbf, 0, 128);
                at = 0;
            }
        }

        if(f->data[x] == '\n') {
            rw++;
            x_pos = 0;
            printw("\n");
            x++;
            if(rw >= SCREEN_HEIGHT - 1) {
                break;
            }
            continue;
        }
        x_pos++;
        if(x_pos < x_offset + 1) {
            x++;
            continue;
        }
        if(f->data[x] == '\t') {
            //uint16_t cx = getcurx(stdscr);
            //uint16_t cy = getcury(stdscr);
            //move(cy, cx + 4);
            for(int g = 0; g < 4; g++) {
                printw(" ", 0);
            }
            
            x++;
            continue;
        }
        
        
        if(x_pos - x_offset - 1 < SCREEN_WIDTH - 2) {
            //addch((const chtype)f->data[x]);
            printw("%c", f->data[x]);
        }
        else {
            
            if(x_pos - x_offset - 1 == SCREEN_WIDTH - 2) {
                move(getcury(stdscr), SCREEN_WIDTH - 2);
                attron(COLOR_PAIR(SH_RED));
                printw(">");
                attroff(COLOR_PAIR(SH_RED));
            }
            
        }
        x++;
    }
    if(at > 10) {
        attroff(COLOR_PAIR(at));
    }
    cursor_x = 0;
    cursor_y = 1;
    for(int y = f->screen_top; y < f->cursor_pos; y++) {
        if(f->data[y] == '\n') {
            cursor_x = 0;
            cursor_y++;
            continue;
        }
        if(f->data[y] == '\t') {
            cursor_x += 4;
            continue;
        }
        cursor_x++;
    }
    cursor_x -= x_offset;

    move(cursor_y, cursor_x);

    refresh();
}

void render_commandline () {
    attron(COLOR_PAIR(4));
    move(SCREEN_HEIGHT - 1, 0);
    printw(">");
    for(int x = 1; x < SCREEN_WIDTH - 1; x++) {
        printw(" ");
    }
    attroff(COLOR_PAIR(4));
    move(SCREEN_HEIGHT - 1, 1);
    refresh();
}

uint8_t command_parse (char *c) {
    char cb[1024] = { 0 };
    uint16_t ix = 0;
    while(c[ix] != 0 && c[ix] != ' ' && c[ix] != '\t') {
        cb[ix] = c[ix];
        ix++;
    }
    if(strcmp(cb, "save") == 0) {
        if(strlen(files[selected_file]->path) == 0) {
            // Cannot save file with empty name
            return 2;
        }
        FILE *ff = fopen(files[selected_file]->path, "w");
        fwrite(files[selected_file]->data, 1, files[selected_file]->size - 1, ff);
        fclose(ff);
        return 1;
    }
    if(strcmp(cb, "saveas") == 0) {
        ix++;
        memset(cb, 0, 1024);
        uint16_t ib = 0;
        while(c[ix] != 0 && c[ix] != ' ' && c[ix] != '\t') {
            cb[ib] = c[ix];
            ix++;
            ib++;
        }
        if(strlen(cb) > 0) {
            files[selected_file]->path = cb;
            FILE *ff = fopen(files[selected_file]->path, "w");
            fwrite(files[selected_file]->data, 1, files[selected_file]->size - 1, ff);
            fclose(ff);
            render_tabsel();
            return 1;
        }
        return 2;
    }
    if(strcmp(cb, "open") == 0) {
        ix++;
        memset(cb, 0, 1024);
        uint16_t ib = 0;
        while(c[ix] != 0 && c[ix] != ' ' && c[ix] != '\t') {
            cb[ib] = c[ix];
            ix++;
            ib++;
        }
        if(strlen(cb) > 0) {
            gfile_open(cb);
            return 1;
        }
        return 2;
    }
    return 0;
}

int16_t getcursorline (struct gfile *f) {
    int32_t l = 0;
    int8_t dir = (f->screen_top > f->cursor_pos) ? -1 : 1;
    if(dir == 1) {
        for(int x = f->screen_top; x < f->cursor_pos; x++) {
            if(f->data[x] == '\n')
                l++;
        }
    }
    else {
        for(int x = f->screen_top; x > f->cursor_pos; x--) {
            if(f->data[x] == '\n')
                l--;
        }
    }
    
    return l;
}