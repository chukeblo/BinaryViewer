#include "ViewerMode.h"

#include <stdio.h>

#include "BinaryViewer.h"

#define FORWARD_COMMAND 'f'
#define BACK_COMMAND 'b'
#define HELP_COMMAND '\?'
#define QUIT_COMMAND 'q'
#define ENTER_KEY '\n'

#define PAGE (MAX_CHARS * MAX_LINES)

static void wait_enter_key() {
    int key = 0;
    do {
        key = getwchar();
        rewind(stdin);
    } while (key != ENTER_KEY); 
}

static void show_help() {
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