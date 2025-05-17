// file.c
#include "file.h"
#include "print.h"


#define NUM_FILES 20

struct File
{
    char name[10];
    char *text;
};

struct File files[NUM_FILES];
//size_t count_files = sizeof(files) / sizeof(files[0]);

static int init = 0;

void init_files()
{
    for (int i = 0; i < NUM_FILES; i++){
        files[i].name[0] = 'a' + i;
        files[i].name[1] = '\0';
    }
}

void file_create(char name)
{
    files[1].name[0] = name;
    files[1].name[1] = '\0'; // null-terminate!
    files[1].text = "&start\n\n&end";
}


void file_print()
{
    print_str("file: ");
    print_str(files[1].name);
    print_char('\n');
    print_str(files[1].text);
}

void list_files() 
{
    if (init == 0) {
        init_files();
        init = 1;
    }
    for (int i = 0; i < NUM_FILES; i++)
    {
        print_str(files[i].name);
        print_char('\n');
    }
}
