#include "Initialize.h"

#include <stdio.h>
#include <string.h>

#include "BinaryViewer.h"

#define OPTION_PREFIX '-'
#define BEGIN_ADDRESS_OPTION 'b'
#define END_ADDRESS_OPTION 'e'
#define VIEWER_MODE_OPTION 'v'
#define FILE_NAME_INDICATOR '.'

static void disp_instruction() {
    printf("usage: binview <filename> [-b<adress(hex)>] [-e<adress(hex)>] [-v]\n");
    printf("  <filename>  ... input filename\n");
    printf("  -b<address> ... begin address\n");
    printf("  -e<address> ... end address\n");
    printf("  -v          ... viewer mode\n");
    return;
}

int initialize(int argc, char* argv[], sHexDumpInfo* info) {
    info->file = NULL;
    info->begin_address = 0;
    info->end_address = -1;
    info->file_size = 0;
    info->viewer_mode = VIEWER_MODE_OFF;

    if (argc < 2 || argc > 5) {
        printf("initialize() received invalid number of arguments: argc=%d\n", argc);
        disp_instruction();
        return FAILURE;
    }

    char* file_name = NULL;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == OPTION_PREFIX) {
            if (argv[i][1] == BEGIN_ADDRESS_OPTION) {
                sscanf(argv[i], "-b%X", &info->begin_address);
            } else if (argv[i][1] == END_ADDRESS_OPTION) {
                sscanf(argv[i], "-e%X", &info->end_address);
            } else if (argv[i][1] == VIEWER_MODE_OPTION) {
                info->viewer_mode = VIEWER_MODE_ON;
            } else {
                printf("initialize() received unsupported option: \"%s\"\n", argv[i]);
                disp_instruction();
                return FAILURE;
            }
        } else if (strchr(argv[i], FILE_NAME_INDICATOR) != NULL) {
            file_name = argv[i];
        }
    }

    if (file_name == NULL) {
        printf("ERROR: no filename has been input.\n");
        disp_instruction();
        return FAILURE;
    } else if ((info->file = fopen(file_name, "rb")) == NULL) {
        printf("initialize() failed to open file: file name=%s\n", file_name);
        return FAILURE;
    }

    fseek(info->file, 0, SEEK_END);
    info->file_size = ftell(info->file);
    rewind(info->file);

    if (info->end_address < 0 || info->end_address > info->file_size) {
        info->end_address = info->file_size;
    }

    if (info->viewer_mode == VIEWER_MODE_ON) {
        info->begin_address = 0;
        info->end_address = info->file_size;
    }

    return SUCCESS;
}