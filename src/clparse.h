#ifndef CLPARSE_H
#define CLPARSE_H

#include "cllex.h"

void cardlang_parse(const cardlang_token_t* tokens, size_t token_count, const char* src_text, size_t src_size);

#endif // CLPARSE_H
