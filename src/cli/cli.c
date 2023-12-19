#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../enigma/enigma.h"
#include "../enigma/reflector/reflector.h"
#include "../enigma/rotor/rotor.h"
#include "../helper/helper.h"
#include "cli.h"

#define INPUT_BUFFER_SIZE 50

typedef struct CLI_OPTIONS
{
    char *enigma_type;
    char *rotor_one_type;
    char *rotor_two_type;
    char *rotor_three_type;
    char *rotor_four_type;
    char *rotor_offsets;
    char *rotor_positions;
    char *reflector_type;
    char *plugboard;
    char *plaintext;
    int interactive;
    int help;
} CLI_OPTIONS;

void print_help(void)
{
    printf("Usage: enigma [OPTION]...\n");
    printf("Encrypts/decrypts text using the Enigma machine\n");
    printf("\n");
    printf("  -i,  --interactive  interactive mode\n");
    printf("  -h,  --help         display this help and exit\n");
    printf("\n");
    printf("Enigma options:\n");
    printf("  -e,  --enigma        Enigma type (M3, M4)\n");
    printf("  -r1, --rotor-one     first rotor type (1, 2, 3, 4, 5, 6, 7)\n");
    printf("  -r2, --rotor-two     second rotor type (1, 2, 3, 4, 5, 6, 7)\n");
    printf("  -r3, --rotor-three   third rotor type (1, 2, 3, 4, 5, 6, 7)\n");
    printf("  -r4, --rotor-four    fourth rotor type (1, 2, 3, 4, 5, 6, 7)\n");
    printf("  -o,  --offsets       rotor offset (ABC, AABC, etc)\n");
    printf("  -p,  --positions     rotor position (ABC, AABC, etc)\n");
    printf("  -r,  --reflector     reflector type (B, C)\n");
    printf("  -b,  --plugboard     plugboard (e.g. AB CD EF)\n");
    printf("  -t,  --plaintext     plaintext (A secret Text)\n");
    printf("\n");
    printf("Examples:\n");
    printf("  enigma -i\n");
    printf("  enigma -e M3 -r1 1 -r2 2 -r3 3 -o ABC -p ABC -r B -b AB CD EF\n");
    printf("\n");
}

void init_cli_options(CLI_OPTIONS *options)
{
    options->enigma_type      = NULL;
    options->rotor_one_type   = NULL;
    options->rotor_two_type   = NULL;
    options->rotor_three_type = NULL;
    options->rotor_four_type  = NULL;
    options->rotor_offsets    = NULL;
    options->rotor_positions  = NULL;
    options->reflector_type   = NULL;
    options->plugboard        = NULL;
    options->interactive      = -1;
    options->help             = -1;
}

int string_equals(const char *str1, const char *str2)
{
    return strcmp(str1, str2) == 0;
}

void save_input(CLI_OPTIONS *options, int argc, char **argv)
{
    int i = 1;

    while (i < argc)
    {
        printf("argv[%d]: %s\n", i, argv[i]);

        if (string_equals(HELP, argv[i]) || string_equals(HELP_SHORT, argv[i]))
        {
            printf("Help\n");
            options->help = 1;
        }
        else if (string_equals(INTERACTIVE, argv[i]) ||
                 string_equals(INTERACTIVE_SHORT, argv[i]))
        {
            options->interactive = 1;
        }
        else if (string_equals(ENIGMA, argv[i]) ||
                 string_equals(ENIGMA_SHORT, argv[i]))
        {
            options->enigma_type = argv[i + 1];
            i++;
        }
        else if (string_equals(ROTOR_ONE, argv[i]) ||
                 string_equals(ROTOR_ONE_SHORT, argv[i]))
        {
            options->rotor_one_type = argv[i + 1];
            i++;
        }
        else if (string_equals(ROTOR_TWO, argv[i]) ||
                 string_equals(ROTOR_TWO_SHORT, argv[i]))
        {
            options->rotor_two_type = argv[i + 1];
            i++;
        }
        else if (string_equals(ROTOR_THREE, argv[i]) ||
                 string_equals(ROTOR_THREE_SHORT, argv[i]))
        {
            options->rotor_three_type = argv[i + 1];
            i++;
        }
        else if (string_equals(ROTOR_FOUR, argv[i]) ||
                 string_equals(ROTOR_FOUR_SHORT, argv[i]))
        {
            options->rotor_four_type = argv[i + 1];
            i++;
        }
        else if (string_equals(ROTOR_OFFSETS, argv[i]) ||
                 string_equals(ROTOR_OFFSETS_SHORT, argv[i]))
        {
            options->rotor_offsets = argv[i + 1];
            i++;
        }
        else if (string_equals(ROTOR_POSITIONS, argv[i]) ||
                 string_equals(ROTOR_POSITIONS_SHORT, argv[i]))
        {
            options->rotor_positions = argv[i + 1];
            i++;
        }
        else if (string_equals(REFLECTOR, argv[i]) ||
                 string_equals(REFLECTOR_SHORT, argv[i]))
        {
            options->reflector_type = argv[i + 1];
            i++;
        }
        else if (string_equals(PLUGBOARD, argv[i]) ||
                 string_equals(PLUGBOARD_SHORT, argv[i]))
        {
            options->plugboard = argv[i + 1];
            i++;
        }
        else if (string_equals(PLAINTEXT, argv[i]) ||
                 string_equals(PLAINTEXT_SHORT, argv[i]))
        {
            options->plaintext = argv[i + 1];
            i++;
        }
        else
        {
            printf("Invalid option: %s\n", argv[i]);
            exit(1);
        }

        i++;

        if (i >= argc)
        {
            break;
        }
    }
}

int validate_cli_options(CLI_OPTIONS *options)
{
    if (options->enigma_type == NULL)
    {
        printf("Enigma type is required\n");
        return 1;
    }

    return 0;
}

int enigma_type_char_to_int(char *enigma_type)
{
    printf("Enigma type: %s\n", enigma_type);

    if (strcmp("M3", enigma_type) == 0)
    {
        return M3;
    }
    else if (strcmp("M4", enigma_type) == 0)
    {
        return M4;
    }
    else
    {
        return -1;
    }
}

Enigma *querry_input_non_interactive(CLI_OPTIONS *options)
{
    Enigma *enigma = (Enigma *)malloc(sizeof(Enigma));

    if (validate_cli_options(options) != 0)
    {
        // printf("Invalid options\n");
        exit(1);
    }

    enigma->type        = enigma_type_char_to_int(options->enigma_type);
    enigma->rotor_one   = create_rotor(options->rotor_one_type[0] - '0',
                                       options->rotor_positions[0] - 'A',
                                       options->rotor_offsets[0] - 'A');
    enigma->rotor_two   = create_rotor(options->rotor_two_type[0] - '0',
                                       options->rotor_positions[1] - 'A',
                                       options->rotor_offsets[1] - 'A');
    enigma->rotor_three = create_rotor(options->rotor_three_type[0] - '0',
                                       options->rotor_positions[2] - 'A',
                                       options->rotor_offsets[2] - 'A');
    if (enigma->type == M4)
    {
        enigma->rotor_four = create_rotor(options->rotor_four_type[0] - '0',
                                          options->rotor_positions[3] - 'A',
                                          options->rotor_offsets[3] - 'A');
    }

    enigma->reflector = create_reflector_by_type(options->reflector_type[0]);
    if (options->plugboard == NULL)
    {
        enigma->plugboard = create_plugboard("");
    }
    else
    {
        enigma->plugboard = create_plugboard(options->plugboard);
    }
    enigma->plaintext = options->plaintext;

    return enigma;
}
Enigma *query_input(int argc, char **argv)
{
    CLI_OPTIONS *options = malloc(sizeof(CLI_OPTIONS));
    init_cli_options(options);
    save_input(options, argc, argv);

    if (options->help == 1)
    {
        print_help();
        exit(0);
    }

    // if (options->interactive)
    // {
    //     return query_input_interactive();
    // }
    // elseconst char *restrict format, ...

    return querry_input_non_interactive(options);

    // return NULL;
}

// Enigma *query_input()
// {
//     char input[INPUT_BUFFER_SIZE];
//     char secondary_input[INPUT_BUFFER_SIZE];
//     char ternary_input[INPUT_BUFFER_SIZE];

//     printf("Type of Enigma (M3, M4):            ");
//     fgets(input, INPUT_BUFFER_SIZE, stdin);

//     if (strncmp("M3", input, 2) )
//     {
//         printf("Fourth Rotor (1, 2, 3, 4, 5, 6, 7):  ");
//         fgets(input, INPUT_BUFFER_SIZE, stdin);
//         printf("First Rotor offset ('A' or 'B' etc): ");
//         fgets(secondary_input, INPUT_BUFFER_SIZE, stdin);
//         printf("Position ('A' or 'B' etc):           ");
//         fgets(ternary_input, INPUT_BUFFER_SIZE, stdin);
//         Rotor *rotorFour = create_rotor(input[0] - '0', ternary_input[0]
//         - 'A',
//                                         secondary_input[0] - 'A');
//     } else if (strncmp("M4", input, 2) )
//     {
//         printf("Fourth Rotor (1, 2, 3, 4, 5, 6, 7):  ");
//         fgets(input, INPUT_BUFFER_SIZE, stdin);
//         printf("First Rotor offset ('A' or 'B' etc): ");
//         fgets(secondary_input, INPUT_BUFFER_SIZE, stdin);
//         printf("Position ('A' or 'B' etc):           ");
//         fgets(ternary_input, INPUT_BUFFER_SIZE, stdin);
//         Rotor *rotorFour = create_rotor(input[0] - '0', ternary_input[0]
//         - 'A',
//                                         secondary_input[0] - 'A');
//     } else
//     {
//         printf("Invalid Enigma type\n");
//         exit(1);
//     }

//     printf("First Rotor (1, 2, 3, 4, 5, 6, 7):   ");
//     fgets(input, INPUT_BUFFER_SIZE, stdin);
//     printf("First Rotor offset ('A' or 'B' etc): ");
//     fgets(secondary_input, INPUT_BUFFER_SIZE, stdin);
//     printf("Position ('A' or 'B' etc): ");
//     fgets(ternary_input, INPUT_BUFFER_SIZE, stdin);
//     Rotor *rotorOne = create_rotor(input[0] - '0', ternary_input[0] -
//     'A',
//                                    secondary_input[0] - 'A');

//     printf("Second Rotor (1, 2, 3, 4, 5, 6, 7):  ");
//     fgets(input, INPUT_BUFFER_SIZE, stdin);
//     printf("First Rotor offset ('A' or 'B' etc): ");
//     fgets(secondary_input, INPUT_BUFFER_SIZE, stdin);
//     printf("Position ('A' or 'B' etc):           ");
//     fgets(ternary_input, INPUT_BUFFER_SIZE, stdin);
//     Rotor *rotorTwo = create_rotor(input[0] - '0', ternary_input[0] -
//     'A',
//                                    secondary_input[0] - 'A');

//     printf("Third Rotor (1, 2, 3, 4, 5, 6, 7):   ");
//     fgets(input, INPUT_BUFFER_SIZE, stdin);
//     printf("First Rotor offset ('A' or 'B' etc): ");
//     fgets(secondary_input, INPUT_BUFFER_SIZE, stdin);
//     printf("Position ('A' or 'B' etc):           ");
//     fgets(ternary_input, INPUT_BUFFER_SIZE, stdin);
//     Rotor *rotorThree = create_rotor(input[0] - '0', ternary_input[0] -
//     'A',
//                                      secondary_input[0] - 'A');

//     printf("Reflector (B, C):                    ");
//     fgets(input, INPUT_BUFFER_SIZE, stdin);
//     Reflector *reflector = create_reflector_by_type(input[0]);

//     printf("Plugboard (e.g. AB CD EF):           ");
//     fgets(input, INPUT_BUFFER_SIZE, stdin);
//     Plugboard *plugboard = create_plugboard(input);

//     Enigma *enigma      = (Enigma *)malloc(sizeof(Enigma));
//     enigma->rotor_one   = rotorOne;
//     enigma->rotor_two   = rotorTwo;
//     enigma->rotor_three = rotorThree;
//     enigma->plugboard   = plugboard;
//     enigma->reflector   = reflector;

//     return enigma;
// }
