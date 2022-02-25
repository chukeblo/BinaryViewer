#include "HexDump.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "BinaryViewer.h"
#include "ViewerMode.h"

void hex_dump(sHexDumpInfo* info) {
    int address = info->begin_address - info->begin_address % MAX_CHARS;

    while (address <= info->end_address) {
        int top = address;
        for (int line = 0; line < MAX_LINES && address <= info->end_address; line++) {
            char buff[MAX_CHARS + 1] = { 0 };
            int count = 0;
            for (count = 0; count < MAX_CHARS && address <= info->end_address; count++) {
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
            while (count++ < MAX_CHARS) {
                printf("   ");
                buff[count] = ' ';
            }
            buff[count] = '\0';
            printf("|%s\n", buff);
        }

        if (info->viewer_mode == VIEWER_MODE_ON) {
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