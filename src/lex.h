#ifndef _LEX_H
#define _LEX_H

#include "token.h"

token_t *get_token();
void cmm_ungetc();
char cmm_getc();
void token_str_classify(token_t *token, char *str, size_t n);
void parse_comment();
token_t *parse_str();
token_t *parse_number();

#endif
