// filesystem.c
#include "filesystem.h"
#include "print.h"

#define NUM_FILES 20

struct File
{
    char name[64];
    char text[1024];
    int active;
};

struct File files[NUM_FILES];
static int init = 0;

void init_files()
{
    for (int i = 0; i < NUM_FILES; i++)
    {
        files[i].name[0] = '\0';
        files[i].text[0] = '\0';
        files[i].active = 0;
    }
}

void file_create(char *name)
{
    if (init == 0)
    {
        init_files();
        init = 1;
    }
    
    // Find first empty slot
    for (int i = 0; i < NUM_FILES; i++)
    {
        if (!files[i].active)
        {
            // Copy name safely
            int j = 0;
            while (name[j] != '\0' && j < 63) {
                files[i].name[j] = name[j];
                j++;
            }
            files[i].name[j] = '\0';
            files[i].text[0] = '\0';
            files[i].active = 1;
            return;
        }
    }
}

void file_print()
{
    // Find first active file
    for (int i = 0; i < NUM_FILES; i++)
    {
        if (files[i].active)
        {
            printf("file: ");
            printf(files[i].name);
            printf("\n");
            printf(files[i].text);
            return;
        }
    }
    printf("No files found\n");
}

void list_files()
{
    if (init == 0)
    {
        init_files();
        init = 1;
    }
    
    printf("Files:\n");
    for (int i = 0; i < NUM_FILES; i++)
    {
        if (files[i].active)
        {
            printf(files[i].name);
            printf("\n");
        }
    }
}

void file_save(char* text)
{
    // Find first active file
    for (int i = 0; i < NUM_FILES; i++)
    {
        if (files[i].active)
        {
            // Copy text safely
            int j = 0;
            while (text[j] != '\0' && j < 1023) {
                files[i].text[j] = text[j];
                j++;
            }
            files[i].text[j] = '\0';
            return;
        }
    }
}

void get_file_text()
{
    // Find first active file
    for (int i = 0; i < NUM_FILES; i++)
    {
        if (files[i].active)
        {
            printf(files[i].text);
            return;
        }
    }
    printf("No files found\n");
}