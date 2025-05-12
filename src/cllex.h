#ifndef CLLEX_H
#define CLLEX_H

#include <stdint.h>

enum cardlang_token_id
{
    T_ERROR,
    T_RESERVED,
    T_NUMBER,
    T_STRING,
    T_IDENTIFIER,
    T_count,
};

static const char* TOKEN_NAMES[] = {
    "T_ERROR",
    "T_RESERVED",
    "T_NUMBER",
    "T_STRING",
    "T_IDENTIFIER",
};

typedef struct
{
    unsigned short type;
    size_t src_idx;
    size_t src_len;
    size_t payload;
} cardlang_token_t;

typedef struct
{
    char* src_text;
    size_t src_size;

    cardlang_token_t* tokens;
    size_t token_count;
} cardlang_lexed_program_t;

typedef struct
{
    char* sample_string;
    unsigned short class;
    unsigned short id;
} reserved_symbol_t;

typedef enum
{
    CL_RESERVED_SYMBOL_OPERATOR,
    CL_RESERVED_SYMBOL_FLOW,
} reserved_symbol_class_t;

typedef enum
{
    CL_OPERATOR_PLUS,
    CL_OPERATOR_MINUS,
    CL_OPERATOR_MULTIPLY,
    CL_OPERATOR_DIVIDE,
    CL_OPERATOR_ASSIGN,
} operator_id_t;

static const int OPERATOR_PRECEDENCE[] = {
    0, // CL_OPERATOR_PLUS
    0, // CL_OPERATOR_MINUS
    1, // CL_OPERATOR_MULTIPLY
    1, // CL_OPERATOR_DIVIDE
    2, // CL_OPERATOR_ASSIGN
};

static const reserved_symbol_t RESERVED_SYMBOLS[] = {
    //{"(", CL_RESERVED_SYMBOL_OPERATOR, CL_RESERVED_SYMBOL_OPERATOR_PLUS},
    //{")", CL_RESERVED_SYMBOL_OPERATOR, CL_RESERVED_SYMBOL_OPERATOR_PLUS},
    {"+", CL_RESERVED_SYMBOL_OPERATOR, CL_OPERATOR_PLUS},
    {"-", CL_RESERVED_SYMBOL_OPERATOR, CL_OPERATOR_MINUS},
    {"*", CL_RESERVED_SYMBOL_OPERATOR, CL_OPERATOR_MULTIPLY},
    {"/", CL_RESERVED_SYMBOL_OPERATOR, CL_OPERATOR_DIVIDE},
    {"=", CL_RESERVED_SYMBOL_OPERATOR, CL_OPERATOR_ASSIGN},
};

static const size_t reserved_symbols_count = sizeof(RESERVED_SYMBOLS) / sizeof(reserved_symbol_t);

void cardlang_lex(char* program_text, size_t program_size, cardlang_token_t** tokens, size_t* token_count);

#endif // CLLEX_H
