#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "clparse.h"

typedef enum
{
    AST_NODE_ERROR,
    AST_NODE_OPERATOR,
    AST_NODE_NUMBER,
} ast_node_type_t;

typedef struct ast_node_t
{
    unsigned short type;
    size_t children_idx[4];
    size_t token_idx;
} ast_node_t;

typedef struct
{
    const char* src_text;
    size_t src_size;
    const cardlang_token_t* tokens;
    size_t token_count;

    size_t token_read_idx;

    ast_node_t* ast_pool;
    size_t ast_pool_count;
    size_t ast_pool_allocated;
} parser_t;

inline const cardlang_token_t* read(parser_t* parser_state)
{
    assert(parser_state != NULL);
    if(parser_state->token_read_idx >= parser_state->token_count) return NULL;
    return &parser_state->tokens[parser_state->token_read_idx];
}

inline size_t new_ast_node(parser_t* parser_state)
{
    assert(parser_state != NULL);
    
    size_t idx = parser_state->ast_pool_count++;
    if (parser_state->ast_pool_count >= parser_state->ast_pool_allocated)
    {
        parser_state->ast_pool_allocated += 512;
        parser_state->ast_pool = realloc(parser_state->ast_pool, sizeof(cardlang_token_t) * parser_state->ast_pool_allocated);
        if (!parser_state->ast_pool)
        {
            puts("Out of memory for AST storage.");
            exit(1);
        }
    }

    return idx;
}

size_t parse_increasing_expression(parser_t* parser_state, int surface)
{
    assert(parser_state != NULL);
    assert(parser_state->token_read_idx < parser_state->token_count);

    size_t lhs_idx = new_ast_node(parser_state);
    ast_node_t* lhs = &parser_state->ast_pool[lhs_idx];

    printf("parse_increasing_expression(%d) token[%zu]\n", surface, parser_state->token_read_idx);
    const cardlang_token_t* token = read(parser_state);
    assert(token != NULL);
    assert(token->type == T_NUMBER || token->type == T_IDENTIFIER);
    lhs->token_idx = parser_state->token_read_idx++;
    lhs->type = AST_NODE_NUMBER;
    
    while(1)
    {
        const cardlang_token_t* token = read(parser_state);
        if (token == NULL 
            || token->type != T_RESERVED
            || RESERVED_SYMBOLS[token->payload].class != CL_RESERVED_SYMBOL_OPERATOR)
            break;
        int precidence = OPERATOR_PRECEDENCE[RESERVED_SYMBOLS[token->payload].id];
        if (precidence <= surface) break;

        size_t op_node_i = new_ast_node(parser_state);
        parser_state->ast_pool[op_node_i].token_idx = parser_state->token_read_idx++;
        parser_state->ast_pool[op_node_i].type = AST_NODE_OPERATOR;
        parser_state->ast_pool[op_node_i].children_idx[0] = lhs_idx;
        parser_state->ast_pool[op_node_i].children_idx[1] = parse_increasing_expression(parser_state, precidence);
        lhs_idx = op_node_i;
    }

    return lhs_idx;
}

size_t parse_expression(parser_t* parser_state)
{
    return parse_increasing_expression(parser_state, -1);
}

/*
parse:
    lhs = value(eat());
    while prec(peek()) > surface
    {
        lhs = op(lhs, operator(eat()), parse());
    }
    return lhs;

*/

void print_ast(parser_t* parser, size_t node_idx)
{
    const ast_node_t* node = &parser->ast_pool[node_idx];
    if (node->type == AST_NODE_OPERATOR)
    {
        printf("(");
        print_ast(parser, node->children_idx[0]);
        printf(" %s ", RESERVED_SYMBOLS[parser->tokens[node->token_idx].payload].sample_string);
        print_ast(parser, node->children_idx[1]);
        printf(")");
    }
    else if (node->type == AST_NODE_NUMBER)
    {
        printf("%.*s", (int)parser->tokens[node->token_idx].src_len, parser->src_text + parser->tokens[node->token_idx].src_idx);
    }
}

void cardlang_parse(const cardlang_token_t* tokens, size_t token_count, const char* src_text, size_t src_size)
{
    puts("Parsing tokens...");

    assert(tokens != NULL);
    assert(token_count > 0);

    parser_t parser = {
        .src_text = src_text,
        .src_size = src_size,

        .tokens = tokens,
        .token_count = token_count,
    };
    
    size_t root_idx = parse_expression(&parser);

    print_ast(&parser, root_idx);
    free(parser.ast_pool);
}
