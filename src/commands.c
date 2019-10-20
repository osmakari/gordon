#include "commands.h"
#include "gordon.h"

struct command *commands[MAX_COMMANDS] = { NULL };

uint8_t _command_save (int argc, char *argv[]) {
    if(strlen(files[selected_file]->path) == 0) {
        // Cannot save file with empty name
        return 0;
    }
    FILE *ff = fopen(files[selected_file]->path, "w");
    fwrite(files[selected_file]->data, 1, files[selected_file]->size, ff);
    fclose(ff);
    return 1;
}

uint8_t _command_saveas (int argc, char *argv[]) {
    if(argc > 1) {
        files[selected_file]->path = argv[1];
        FILE *ff = fopen(files[selected_file]->path, "w");
        fwrite(files[selected_file]->data, 1, files[selected_file]->size, ff);
        fclose(ff);
        render_tabsel();
        return 1;
    }
    return 0;
}

uint8_t _command_open (int argc, char *argv[]) {

    if(argc > 1) {
        gfile_open(argv[1]);
        return 1;
    }
    return 0;
}

uint8_t _command_help (int argc, char *argv[]) {
    
}

void init_commands () {
    command_create("save", _command_save, "Saves the file");
    command_create("saveas", _command_saveas, "Saves the file as the second parameter");
    command_create("open", _command_open, "Opens a file");
}

struct command *command_create (char *name, uint8_t (*func)(int argc, char *argv[]), char *description) {
    for(int i = 0; i < MAX_COMMANDS; i++) {
        if(commands[i] == NULL) {
            commands[i] = (struct command *)malloc(sizeof(struct command));
            commands[i]->name = name;
            commands[i]->func = func;
            commands[i]->description = description;
            return commands[i];

        }
    }
    return NULL;
}