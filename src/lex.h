#ifndef _LEX_H
#define _LEX_H

#include "token.h"

token_t *get_token();
void cmm_ungetc();
char cmm_getc();
void token_str_classify(token_t *token, char *str);
void parse_comment();
void parse_str(token_t *token);
void parse_number(token_t *token);
void parse_par(token_t *token);
void parse_square_bracket(token_t *token);
void parse_curly_bracket(token_t *token);
void parse_collon(token_t *token);
void parse_semicollon(token_t *token);
void parse_op_rel(token_t *token);
void cmm_debug_token(token_t *token);

#endif
