#include "commands.h"
#include "print.h"
#include "heap_alloc.h"
#include "file.h"
#include "power.h"
#include "string.h"

int tokenize(char *input, char **tokens, int max_tokens)
{
    int i = 0;
    while (*input && i < max_tokens)
    {
        // Skip whitespace or commas
        while (*input == ' ' || *input == ',')
        {
            input++;
        }
        if (*input == '\0')
            break;

        tokens[i++] = input;
        while (*input && *input != ' ' && *input != ',')
        {
            input++;
        }

        if (*input)
        {
            *input = '\0'; // Null terminate token
            input++;
        }
    }
    return i; // Number of tokens
}

uint8_t color_from_string(const char *str)
{
    if (str_eq(str, "black"))
        return 0;
    if (str_eq(str, "blue"))
        return 1;
    if (str_eq(str, "green"))
        return 2;
    if (str_eq(str, "cyan"))
        return 3;
    if (str_eq(str, "red"))
        return 4;
    if (str_eq(str, "magenta"))
        return 5;
    if (str_eq(str, "brown"))
        return 6;
    if (str_eq(str, "lightGray"))
        return 7;
    if (str_eq(str, "darkGray"))
        return 8;
    if (str_eq(str, "lightBlue"))
        return 9;
    if (str_eq(str, "lightGreen"))
        return 10;
    if (str_eq(str, "lightCyan"))
        return 11;
    if (str_eq(str, "lightRed"))
        return 12;
    if (str_eq(str, "pink"))
        return 13;
    if (str_eq(str, "yellow"))
        return 14;
    if (str_eq(str, "white"))
        return 15;
    return 15; // default white
}
/*
void commands(char *text)
{
    print_newline();

    char *tokens[5]; // supports up to 5 tokens
    int count = tokenize(text, tokens, 5);

    if (str_eq(tokens[0], "$setColor"))
    {

        uint8_t fg = color_from_string(tokens[1]);
        uint8_t bg = color_from_string(tokens[2]);
        print_set_color(fg, bg);

        if (count < 4)
        {
            return;
        }
        else if (str_eq(tokens[3], "-R"))
        {
            reload_screen();
        }
    }
    else if (str_eq(tokens[0], "$ls"))
    {
        list_files();
    }
    else if (str_eq(tokens[0], "$w"))
    {
        file_create(tokens[1]);
    }
    else if (str_eq(tokens[0], "$ket"))
    {
        file_print();
    }
    else if (str_eq(tokens[0], "$fc"))
    {
        file_save(tokens[1]);
    }
    else if (str_eq(tokens[0], "$ft"))
    {
        get_file_text();
    }
    else if (str_eq(tokens[0], "$clear"))
    {
        print_clear();
    }
    else if (str_eq(tokens[0], "$logo"))
    {
        print_logo();
    }
    else if (str_eq(tokens[0], "$shutdown"))
    {
        set_running(OFF);
    }
    else if (str_eq(tokens[0], "$isRunning"))
    {
        print_str(int_to_str(get_running()));
    }
    else
    {
        print_str("comands not found: ");
        print_str(tokens[0]);
    }
}
    */