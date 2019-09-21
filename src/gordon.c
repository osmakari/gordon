#include "gordon.h"

#define KEY_CTRL_PAGEUP 563
#define KEY_CTRL_PAGEDOWN 558
#define KEY_BACKSP 127
#define KEY_DEL 330
#define KEY_RET 13

struct gfile *files[MAX_FILES] = { NULL };

uint16_t cursor_x = 0;
uint16_t cursor_y = 0;

int selected_file = 0;

int main (int argc, char *argv[]) {
    initscr();
    cbreak();
    noecho();
    
    keypad(stdscr, TRUE);
    nonl();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_CYAN);
    init_pair(3, COLOR_BLACK, COLOR_MAGENTA);

    getmaxyx(stdscr, SCREEN_HEIGHT, SCREEN_WIDTH);
    clear();

    refresh();

    render_tabsel();
    uint8_t swi = 0;

    for(int x = 1; x < argc; x++) {
        if(swi == 0) {
            if(argv[x][0] == '-') {
                // arg
            }
            else {
                // Open file
                gfile_open(argv[x]);
            }
        }
        
    }


    while(1) {
        int ip = getch();
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
            if(cursor_x > 0) {
                //cursor_x--;
                files[selected_file]->cursor_pos--;
            }
            //move(cursor_y, cursor_x);
            //refresh();
            render_file(files[selected_file]);
        }
        else if(ip == KEY_RIGHT) {
            uint16_t ll = 0;
            while(files[selected_file]->data[files[selected_file]->cursor_pos + ll] != '\n' && 
                    files[selected_file]->data[files[selected_file]->cursor_pos + ll] != 0) {
                
                ll++;
            }
            if(ll > 0) {
                //cursor_x++;
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
                    ll = files[selected_file]->size - 1;
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

            //move(cursor_y, cursor_x);
            //refresh();
            render_file(files[selected_file]);
        }
        else if(ip == KEY_UP) {
            size_t ll = files[selected_file]->cursor_pos - 1;
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
                size_t sp_p = ll;
                while(files[selected_file]->data[sp_p] != '\n' && sp_p > 0) {
                    line_len++;
                    sp_p--;
                }

                size_t xp = line_len - 1;
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
            render_file(files[selected_file]);
            //move(cursor_y, cursor_x);
            //refresh();
        }
        else if(ip == KEY_BACKSP) {
            if(files[selected_file]->cursor_pos > 0) {
                for(int x = files[selected_file]->cursor_pos - 1; x < files[selected_file]->size - 2; x++) {
                    files[selected_file]->data[x] = files[selected_file]->data[x + 1];
                }
                files[selected_file]->size--;
                files[selected_file]->cursor_pos--;
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

            if(files[selected_file]->allocated < files[selected_file]->size + 1) {
                files[selected_file]->data = realloc(files[selected_file]->data, files[selected_file]->allocated + 64);
                files[selected_file]->allocated += 64;
            }
            for(int x = files[selected_file]->size - 1; x > files[selected_file]->cursor_pos; x--) {
                files[selected_file]->data[x] = files[selected_file]->data[x - 1];
            }
            files[selected_file]->data[files[selected_file]->cursor_pos] = (char)ip;
            files[selected_file]->cursor_pos++;
            render_file(files[selected_file]);
            
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
        printw("%s", files[x]->path);
        offset += strlen(files[x]->path) + 1;

        if(x == selected_file) {
            attroff(COLOR_PAIR(3));
            attron(COLOR_PAIR(2));
        }
    }
    attroff(COLOR_PAIR(2));
    refresh();
}

struct gfile *gfile_open (char *path) {
    // Open file

    FILE *f = fopen(path, "r");

    if(f == NULL) {

        return NULL;
    }

    fseek(f, 0L, SEEK_END);
    size_t sz = ftell(f);
    fseek(f, 0L, SEEK_SET);

    for(int x = 0; x < MAX_FILES; x++) {
        if(files[x] == NULL) {
            struct gfile *gg = (struct gfile*)malloc(sizeof(struct gfile));
            gg->path = path;
            gg->size = sz;
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
    while(x < f->size) {
        if(f->data[x] == 0)
            break;

        if(f->data[x] == '\n') {
            rw++;
            if(rw >= SCREEN_HEIGHT - 1) {
                break;
            }
        }
        printw("%c", f->data[x]);
        x++;
    }
    cursor_x = 0;
    cursor_y = 1;
    for(int y = f->screen_top; y < f->cursor_pos; y++) {
        if(f->data[y] == '\n') {
            cursor_x = 0;
            cursor_y++;
            continue;
        }
        cursor_x++;
    }

    move(cursor_y, cursor_x);

    refresh();
}