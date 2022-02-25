#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

int main(int argc, char *argv[]) {
    FILE *fp = NULL;
    char *fileName = NULL;
    int begin_address = 0;
    int end_address = -1;
    int fileSize = 0;
    int v_indicator = 0;
    const int filenameIndicator = '.';

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == 'b') {
                sscanf(argv[i], "-b%X", &begin_address);
            } else if (argv[i][1] == 'e') {
                sscanf(argv[i], "-e%X", &end_address);
            } else if (argv[i][1] == 'v') {
                v_indicator = 1;
            } else {
                printf("unspecified option: \"%s\"\n", argv[i]);
                disp_instruction();
                return -1;
            }
        } else if (strchr(argv[i], filenameIndicator) != NULL) {
            fileName = argv[i];
        }
    }

    if (fileName == NULL) {
        printf("ERROR: no filename has been input.\n");
        disp_instruction();
        return -1;
    } else if ((fp = fopen(fileName, "rb")) == NULL) {
        printf("ERROR: the file failed to be opened.\n");
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
    rewind(fp);

    if (end_address < 0 || end_address > fileSize) {
        end_address = fileSize;
    }

    if (v_indicator == 1) {
        begin_address = 0;
        end_address = fileSize;
    }
    
    if (begin_address < end_address) {
        hex_dump(fp, begin_address, end_address, v_indicator);
    }

    fclose(fp);
    return 0;
}