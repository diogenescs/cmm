#include "lex.h"


void start_parse(char *buffer, size_t buffer_size)
{
	token_t *token;
    init_lex(buffer, buffer_size);
    do
    {
    	token = get_token();
    }
    while (token->type != TOKEN_END);
}