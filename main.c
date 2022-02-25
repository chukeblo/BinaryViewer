#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "HexDumpInfo.h"

#define SUCCESS 1
#define FAILURE 0

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

int viewer_mode(int address, int end_address) {
    int command = 0;
    while (1) {
        printf("enter command(\'?\' for help): ");
        command = getchar();
        rewind(stdin);
        switch (command) {
        case 'f':
            if (address + PAGE <= end_address) {
                address += PAGE;
            }
            return address;

        case 'b':
            if (address - PAGE >= 0) {
                address -= PAGE;
            }
            return address;

        case '\?':
            printf("\'f\': forward page\n");
            printf("\'b\': back page\n");
            printf("\'q\': quit binview\n");
            printf("\'\?\': display this\n");
            printf("press enter ... ");
            while(getchar() != '\n'){
                rewind(stdin);
            }
            rewind(stdin);
            break;
        
        case 'q':
            return -1;

        default:
            break;
        }
    }
}

void hex_dump(FILE *fp, int begin_address, int end_address, int v_indicator) {
    int address = begin_address - begin_address % 16;
    int top = address;
    int data;
    int line = 0;

    while (address <= end_address) {
        for (line = 0; line < MAX_LINE && address <= end_address; line++) {
            char buff[16] = { 0 };
            int i = 0;
            for (i = 0; i < 16 && address <= end_address; i++) {
                data = fgetc(fp);

                if(i == 0){
                    printf("%08x: ", address);
                }

                if (address < begin_address) {
                    printf("   ");
                    *(buff + i) = ' ';
                } else {
                    printf("%02x ", (unsigned char)data);
                    if (isprint(data)) {
                        buff[i] = data;
                    } else {
                        buff[i] = '.';
                    }
                }
                address++;
            }
            if (i < 15) {
                for (int j = i; j < 16; j++) {
                    printf("   ");
                    buff[j] = ' ';
                }
            }
            buff[16] = '\0';
            printf("|%s\n", buff);
        }

        if (v_indicator == 1) {
            address = viewer_mode(top, end_address);
            if (address == -1) {
                return;
            }
            top = address;
            rewind(fp);
            address = 0;
            while (address < top) {
                data = fgetc(fp);
                address++;
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
    initialize(argc, argv, &info);
    
    if (info.begin_address < info.end_address) {
        hex_dump(info.file, info.begin_address, info.end_address, info.viewer_mode);
    }

    fclose(info.file);
    return 0;
}