#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cllex.h"

typedef struct 
{
    char next;
    char peek;

    char* _input_text;
    size_t _input_size;
    size_t _next_read_idx;

    cardlang_token_t* _tokens;
    size_t _tokens_count;
    size_t _tokens_allocated;
} lexer_t;

inline void update_view(lexer_t* lexer_state)
{
    int64_t remaining = lexer_state->_input_size - lexer_state->_next_read_idx;
    
    lexer_state->next =
        remaining > 0
        ? lexer_state->_input_text[lexer_state->_next_read_idx]
        : '\0';

    lexer_state->peek = 
        remaining > 1
        ? lexer_state->_input_text[lexer_state->_next_read_idx + 1]
        : '\0';
}

inline void grow_token_allocation(lexer_t* lexer_state, size_t count)
{
    assert(lexer_state != NULL);

    lexer_state->_tokens_allocated += count;
    lexer_state->_tokens = realloc(lexer_state->_tokens, sizeof(cardlang_token_t) * lexer_state->_tokens_allocated);
    if (!lexer_state->_tokens)
    {
        puts("Out of memory for token storage.");
        exit(1);
    }
}

inline void push_token(lexer_t* lexer_state, unsigned short type, size_t src_idx, size_t src_len, size_t payload)
{
    assert(lexer_state != NULL);
    assert(lexer_state->_tokens != NULL);

    if (lexer_state->_tokens_count >= lexer_state->_tokens_allocated)
    {
        grow_token_allocation(lexer_state, 512);
    }

    cardlang_token_t* token = &lexer_state->_tokens[lexer_state->_tokens_count++];
    token->type = type;
    token->src_idx = src_idx;
    token->src_len = src_len;
    token->payload = payload;
}

inline lexer_t init(char* program_text, size_t program_size)
{
    assert(program_text != NULL);
    lexer_t lexer_state = {
        ._input_text = program_text,
        ._input_size = program_size,
        ._next_read_idx = 0,
    };
    update_view(&lexer_state);
    grow_token_allocation(&lexer_state, 512);
    return lexer_state;
}

inline void advance(lexer_t* lexer_state)
{
    lexer_state->_next_read_idx++;
    update_view(lexer_state);
}

int read_whitespace(lexer_t* lexer_state)
{
    int whitespace_hit = 0;
    while (lexer_state->next == ' '
          || lexer_state->next == '\t'
          || lexer_state->next == '\n' 
          || lexer_state->next == '\r')
    {
        whitespace_hit = 1;
        advance(lexer_state);
    }

    return whitespace_hit;
}

int read_comments(lexer_t* lexer_state)
{
    int comment_hit = 0;
    if (lexer_state->next == '#')
    {
        comment_hit = 1;
        advance(lexer_state);
        while ((lexer_state->next != '\n') && (lexer_state->next != '\0'))
            advance(lexer_state);
    }
    if (comment_hit)
    {
        printf("Comment\n");
        return 1;
    }
    return comment_hit;
}

int read_reserved(lexer_t* lexer_state)
{
    size_t start_read = lexer_state->_next_read_idx;

    for (size_t i = 0; i < reserved_symbols_count; i++)
    {
        const char* symbol = RESERVED_SYMBOLS[i].sample_string;
        int hit = 1;
        while (*symbol != '\0')
        {
            if (lexer_state->next != *symbol)
            {
                hit = 0;
                break;
            }
            advance(lexer_state);
            symbol++;
        }

        if (hit)
        {
            push_token(lexer_state, T_RESERVED, start_read, lexer_state->_next_read_idx - start_read, i);
            printf("Reserved symbol: %.*s\n", (int)(lexer_state->_next_read_idx - start_read), lexer_state->_input_text + start_read);
            return 1;
        }

        lexer_state->_next_read_idx = start_read;
        update_view(lexer_state);
    }
    return 0;
}

int read_number(lexer_t* lexer_state)
{
    size_t start_read = lexer_state->_next_read_idx;
    int number_hit = 0;

    while (lexer_state->next >= '0' && lexer_state->next <= '9')
    {
        number_hit = 1;
        advance(lexer_state);
    }

    if (number_hit)
    {
        push_token(lexer_state, T_NUMBER, start_read, lexer_state->_next_read_idx - start_read, 0);
        printf("Number: %.*s\n", (int)(lexer_state->_next_read_idx - start_read), lexer_state->_input_text + start_read);
        return 1;
    }
    return 0;
}

void cardlang_lex(char* program_text, size_t program_size, cardlang_token_t** tokens, size_t* token_count)
{
    printf("Lexing...\n");
    assert(program_text != NULL);
    assert(program_size > 0);
    
    *tokens = NULL;


    lexer_t lexer_state = init(program_text, program_size);

    while (1)
    {
        puts("loop");
        printf("next: %c\n", lexer_state.next);
        if      (read_whitespace(&lexer_state));
        else if (read_comments  (&lexer_state));
        else if (read_reserved  (&lexer_state));
        else if (read_number    (&lexer_state));
        //else if (read_identifier(&lexer_state));
        else break;
    }

    lexer_state._tokens = realloc(lexer_state._tokens, sizeof(cardlang_token_t) * lexer_state._tokens_count);
    if (!lexer_state._tokens)
    {
        puts("Out of memory for token shrink.");
        exit(1);
    }

    puts("Finished lexing");

    for (size_t i = 0; i < lexer_state._tokens_count; i++)
    {
        cardlang_token_t* token = &lexer_state._tokens[i];
        switch (token->type)
        {
            case T_RESERVED:
                printf("(%zu-%zu) %s : %s\n", token->src_idx, token->src_idx + token->src_len, TOKEN_NAMES[token->type], RESERVED_SYMBOLS[token->payload].sample_string);
                break;
            case T_NUMBER:
                printf("(%zu-%zu) %s : %.*s\n", token->src_idx, token->src_idx + token->src_len, TOKEN_NAMES[token->type], (int)token->src_len, lexer_state._input_text + token->src_idx);
                break;
            default:
                printf("%s\n", TOKEN_NAMES[token->type]);
        }
    }

    *tokens = lexer_state._tokens;
    *token_count = lexer_state._tokens_count;
}
