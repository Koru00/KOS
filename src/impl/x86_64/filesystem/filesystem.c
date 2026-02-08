// file.c
#include "filesystem.h"
#include "print.h"

#define NUM_FILES 20

struct File
{
    char *name;
    char *text;
};

struct File files[NUM_FILES];
// size_t count_files = sizeof(files) / sizeof(files[0]);

static int init = 0;

void init_files()
{
    for (int i = 0; i < NUM_FILES; i++)
    {
        files[i].name = 'a' + i;
    }
}

/*
open the file
clearing the terminal
saving the file from row 0 to row command - 1
seting text of the file using a command and saying the file to edit
set the row of the command (like when I press enter set row + 1)
using like $save
set row of the command save - 1
*/

void file_create(char *name)
{
    files[1].name = name;
    files[1].text = "";
}

void file_print()
{
    printf("file: ");
    printf(files[1].name);
    printf('\n');
    printf(files[1].text);
}

void list_files()
{
    if (init == 0)
    {
        init_files();
        init = 1;
    }
    for (int i = 0; i < NUM_FILES; i++)
    {
        printf(files[i].name);
        printf('\n');
    }
}

void file_save(char* text)
{
    files[1].text = text;
}

void get_file_text()
{
    printf(files[1].text);
}