#include "lex.h"


void start_parse(char *buffer, size_t buffer_size)
{
	init_lex(buffer, buffer_size);
	get_token();
}