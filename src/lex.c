#include <stdlib.h>
#include "token.h"
#include "lex.h"



char *buffer;
size_t buffer_size;
size_t pos;
size_t lineno;

token_t *cur_token;
token_t *next_token;


void cmm_ungetc()
{
    if (pos > 0)
        pos--;
}

char cmm_getc()
{
    return (pos == buffer_size) ? 0 : buffer[pos++];
}


token_t *get_token()
{   
    token_t *token;
    char sym;

    while(1)
    {
        sym = cmm_getc();
        if (sym == 0)
            break;

        if (isblank(sym))
            continue;

        if (sym == '\n')
            lineno++;

        else if (sym == '/')
            parse_comment();

        else if (isalpha(sym))
            token = parse_str();

        else if (isdigit(sym))
            token = parse_number();

        else 
        {
            fprintf(stderr, "unexpected char '%c'", sym);
        }
    }
}

void init_lex(char *buf, size_t size)
{
    buffer = buf;
    buffer_size = size;
    pos = 0;
    lineno = 1;
}

token_t *get_next_token()
{
    return next_token;
}


void parse_comment()
{

}

token_t *parse_str()
{
    token_t *token;
    char *init = buffer + (pos - 1);
    size_t n = 1;
    char sym;

    token = (token_t *)malloc(sizeof(token_t));
    if (!token)
    {
        perror("malloc : parse_str");
        exit(EXIT_FAILURE);
    }
    
    token->lineno = lineno;

    while(1)
    {
        sym = cmm_getc();
        n++;
        if (!isalnum(sym) && sym != '_')
        {
            cmm_ungetc();
            token_str_classify(token, init, n - 1);
            break;
        }
    }

    return token;
}


void token_str_classify(token_t *token, char *str, size_t n)
{
    int i, done = 0;    
    int vcmds[] = {TOKEN_CMD_IF, TOKEN_CMD_ELSE, TOKEN_CMD_WHILE, TOKEN_CMD_FOR, TOKEN_CMD_RETURN};
    char *cmds[] = {"if", "else", "while", "for", "return"};

    int vtypes[] = {CMM_TYPE_CHAR, CMM_TYPE_INT, CMM_TYPE_REAL, CMM_TYPE_BOOL, CMM_TYPE_VOID};
    char *types[] = {"char", "int", "real", "bool", "void"};

    for (i = 0; i < 5; i++)
        if (!strncmp(str, cmds[i], n))
        {
            token->type = vcmds[i];
            done = 1;
            break;
        }

    if (!done)
        for (i = 0; i < 5; i++)
            if (!strncmp(str, types[i], n))
            {
                token->type = TOKEN_CMM_TYPE;
                token->value.cmm_type_val = vtypes[i];
                done = 1;
                break;
            }

    if (!done)
    {
        token->type = TOKEN_ID;
        // TODO: store lexema
    }


}

token_t *parse_number()
{

}