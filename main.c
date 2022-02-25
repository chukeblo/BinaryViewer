#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "BinaryViewer.h"
#include "Initialize.h"
#include "HexDump.h"
#include "HexDumpInfo.h"

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