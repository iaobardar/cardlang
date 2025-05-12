#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "io.h"

size_t read_whole_file(const char* filename, void** buffer)
{
    puts("Reading file...");
    FILE* in_file = fopen(filename, "rb");
    if (!in_file) return 0;

    const size_t read_chunk_size = 512;
    size_t allocated = 0;
    size_t used = 0;
    uint8_t* data = NULL;

    do
    {
        allocated += read_chunk_size;
        data = realloc(data, allocated);
        
        if (!data) {puts("Out of memory for file read."); exit(1);}

        used += fread(data + used, 1, allocated - used, in_file);
        printf("read loop %zi %zi\n", allocated, used);
    }
    while (used == allocated);

    // Shrink back to fit only the used portion.
    *buffer = realloc(data, used);
    if (!*buffer) {puts("Out of memory for file read."); exit(1);}

    fclose(in_file);
    return used;
}
