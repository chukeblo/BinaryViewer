#ifndef _HEX_DUMP_INFO_H_
#define _HEX_DUMP_INFO_H_

#include <stdio.h>

typedef struct HexDumpInfo {
    FILE *file;
    int begin_address;
    int end_address;
    int file_size;
    unsigned char viewer_mode;
} sHexDumpInfo;

#endif  // _HEX_DUMP_INFO_H_