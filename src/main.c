#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "io.h"
#include "cllex.h"
#include "clparse.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <program file path>\n", argv[0]);
        return 1;
    }

    char* loaded_program;
    size_t loaded_program_size = read_whole_file(argv[1], (void**)&loaded_program);
    assert(loaded_program != NULL);
    assert(loaded_program_size > 0);

    printf("Loaded program size: %zu\n", loaded_program_size);
    if (loaded_program_size == 0)
    {
        printf("Failed to load program file: %s\n", argv[1]);
        return 1;
    }

    cardlang_token_t* tokens = NULL;
    size_t token_count = 0;

    cardlang_lex(loaded_program, loaded_program_size, &tokens, &token_count);
    cardlang_parse(tokens, token_count, loaded_program, loaded_program_size);
    free(loaded_program);

    return 0;
}
