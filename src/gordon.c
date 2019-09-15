#include "gordon.h"

#define KEY_CTRL_PAGEUP 563
#define KEY_CTRL_PAGEDOWN 558

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

    gfile_open("src/gordon.h");
    gfile_open("src/gordon.c");


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

        if(ip == KEY_LEFT) {
            if(cursor_x > 0) {
                cursor_x--;
                files[selected_file]->cursor_pos--;
            }
            move(cursor_y, cursor_x);
            refresh();
        }
        else if(ip == KEY_RIGHT) {
            uint16_t ll = 0;
            while(files[selected_file]->data[files[selected_file]->cursor_pos + ll] != '\n' && 
                    files[selected_file]->data[files[selected_file]->cursor_pos + ll] != 0) {
                
                ll++;
            }
            if(ll > 0) {
                cursor_x++;
                files[selected_file]->cursor_pos++;
            }
            move(cursor_y, cursor_x);
            refresh();
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
            gg->data = (char*)malloc(gg->size);
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
        if(f->data[x] == '\n') {
            rw++;
            if(rw >= SCREEN_HEIGHT) {
                break;
            }
        }
        printw("%c", f->data[x]);
        x++;
    }
    cursor_x = 0;
    cursor_y = 1;
    for(int y = 0; y < f->cursor_pos - f->screen_top; y++) {
        if(f->data[x] == '\n') {
            cursor_x = 0;
            cursor_y++;
        }
    }
    

    move(cursor_y, cursor_x);

    refresh();
}