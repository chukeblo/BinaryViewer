#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "HexDumpInfo.h"

#define SUCCESS 1
#define FAILURE 0

#define FORWARD_COMMAND 'f'
#define BACK_COMMAND 'b'
#define HELP_COMMAND '\?'
#define QUIT_COMMAND 'q'
#define ENTER_KEY '\n'
#define QUIT_VIEWER_MODE -1

#define MAX_LINE 24
#define PAGE (MAX_LINE * 16)

void disp_instruction() {
    printf("usage: binview <filename> [-b<adress(hex)>] [-e<adress(hex)>] [-v]\n");
    printf("  <filename>  ... input filename\n");
    printf("  -b<address> ... begin address\n");
    printf("  -e<address> ... end address\n");
    printf("  -v          ... viewer mode\n");
    return;
}

void wait_enter_key() {
    int key = 0;
    do {
        key = getwchar();
        rewind(stdin);
    } while (key != ENTER_KEY); 
}

void show_help() {
    printf("\'f\': forward page\n");
    printf("\'b\': back page\n");
    printf("\'q\': quit binary viewer\n");
    printf("\'\?\': display this\n");
    printf("press enter ... ");
    wait_enter_key();
}

int viewer_mode(int address, int end_address) {
    int command = 0;
    while (1) {
        printf("enter command(\'?\' for help): ");
        command = getchar();
        rewind(stdin);
        switch (command) {
        case FORWARD_COMMAND:
            if (address + PAGE <= end_address) {
                address += PAGE;
            }
            return address;
        case BACK_COMMAND:
            if (address - PAGE >= 0) {
                address -= PAGE;
            }
            return address;
        case HELP_COMMAND:
            show_help();
            break;
        case QUIT_COMMAND:
            return QUIT_VIEWER_MODE;
        default:
            break;
        }
    }
}

void hex_dump(sHexDumpInfo* info) {
    int address = info->begin_address - info->begin_address % 16;

    while (address <= info->end_address) {
        int top = address;
        for (int line = 0; line < MAX_LINE && address <= info->end_address; line++) {
            char buff[17] = { 0 };
            int count = 0;
            for (count = 0; count < 16 && address <= info->end_address; count++) {
                if(count == 0){
                    printf("%08x: ", address);
                }

                int data = fgetc(info->file);

                if (address < info->begin_address) {
                    printf("   ");
                    buff[count] = ' ';
                } else {
                    printf("%02x ", (unsigned char)data);
                    buff[count] = isprint(data) ? data : '.';
                }
                address++;
            }
            while (count++ < 16) {
                printf("   ");
                buff[count] = ' ';
            }
            buff[count] = '\0';
            printf("|%s\n", buff);
        }

        if (info->viewer_mode == 1) {
            address = viewer_mode(top, info->end_address);
            if (address == QUIT_VIEWER_MODE) {
                return;
            }
            rewind(info->file);
            for (int i = 0; i < address; i++) {
                fgetc(info->file);
            }
        }
    }
    return;
}

int initialize(int argc, char* argv[], sHexDumpInfo* info) {
    const int filenameIndicator = '.';

    info->file = NULL;
    info->begin_address = 0;
    info->end_address = -1;
    info->file_size = 0;
    info->viewer_mode = 0;

    if (argc < 2 || argc > 5) {
        printf("initialize() received invalid number of arguments: argc=%d\n", argc);
        disp_instruction();
        return FAILURE;
    }

    char* file_name = NULL;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == 'b') {
                sscanf(argv[i], "-b%X", &info->begin_address);
            } else if (argv[i][1] == 'e') {
                sscanf(argv[i], "-e%X", &info->end_address);
            } else if (argv[i][1] == 'v') {
                info->viewer_mode = 1;
            } else {
                printf("initialize() received unsupported option: \"%s\"\n", argv[i]);
                disp_instruction();
                return FAILURE;
            }
        } else if (strchr(argv[i], filenameIndicator) != NULL) {
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

    if (info->viewer_mode == 1) {
        info->begin_address = 0;
        info->end_address = info->file_size;
    }

    return SUCCESS;
}

int main(int argc, char *argv[]) {
    sHexDumpInfo info;
    if (initialize(argc, argv, &info) == FAILURE) {
        return -1;
    }
    
    if (info.begin_address < info.end_address) {
        hex_dump(&info);
    }

    fclose(info.file);
    return 0;
}