#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "lex.h"



char *buffer;
size_t buffer_size;
size_t pos;
size_t lineno;
int initialized;

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
    token_t *token, *token_ret;
    char sym;

    token = (token_t *)malloc(sizeof(token_t));
    if (!token)
    {
        perror("malloc : parse_str");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        sym = cmm_getc();
        if (sym == 0)
        {
            token->type = TOKEN_END;
            break;
        }

        if (isblank(sym))
            continue;

        if (sym == '\n')
            lineno++;

        else if (sym == '/')
            parse_comment();
        
        else if (sym == '(' || sym == ')')
        {
            parse_par(token);
            break;
        }

        else if (sym == '[' || sym == ']')
        {
            parse_square_bracket(token);
            break;
        }

        else if (sym == '{' || sym == '}')
        {
            parse_curly_bracket(token);
            break;
        }

        else if (sym == '<' || sym == '>' || sym == '=' || sym == '!')
        {
            parse_op_rel(token);
            break;
        }

        else if (sym == ',')
        {
            parse_collon(token);
            break;
        }

        else if (sym == ';')
        {
            parse_semicollon(token);
            break;
        }

        else if (isalpha(sym))
        {
            parse_str(token);
            break;
        }

        else if (isdigit(sym))
        {
            parse_number(token);
            break;
        }

        else if (sym == '&')
        {
            parse_logical_and(token);
            if (token->type == TOKEN_UNEXPECTED)
            {
                fprintf(stderr, "unexpected char '%c' at line %lu\n", buffer[pos-1], lineno);
                continue;
            }
            break;
        }

        else if (sym == '|')
        {
            parse_logical_or(token);
            if (token->type == TOKEN_UNEXPECTED)
            {
                fprintf(stderr, "unexpected char '%c' at line %lu\n", buffer[pos-1], lineno);
                continue;
            }
            break;
        }

        else
        {
            fprintf(stderr, "unexpected char '%c' at line %lu\n", sym, lineno);
        }
    }

    if (!initialized)
        return token;

    token_ret = cur_token;
    cur_token = next_token;
    next_token = token;

    return token_ret;
}

void init_lex(char *buf, size_t size)
{
    buffer = buf;
    buffer_size = size;
    pos = 0;
    lineno = 1;
    cur_token = get_token();
    next_token = get_token();
    initialized = 1;

}

token_t *get_next_token()
{
    return cur_token;
}


void parse_comment()
{

}

void parse_logical_and(token_t *token)
{
    char sym = cmm_getc();
    token->lineno = lineno;
    if (sym == '&')
        token->type = TOKEN_LOGICAL_AND;
    else
    {
        token->type = TOKEN_UNEXPECTED;
        cmm_ungetc();
    }
}


void parse_logical_or(token_t *token)
{
    char sym = cmm_getc();
    token->lineno = lineno;
    if (sym == '|')
        token->type = TOKEN_LOGICAL_OR;
    else
    {
        token->type = TOKEN_UNEXPECTED;
        cmm_ungetc();
    }
}

void parse_str(token_t *token)
{
    char *init = buffer + (pos - 1);
    size_t n = 1;
    char sym;
    
    token->lineno = lineno;

    while(1)
    {
        sym = cmm_getc();
        n++;
        if (!isalnum(sym) && sym != '_')
        {
            cmm_ungetc();
            token->value.id_val = (char *)malloc(n);
            if (!token->value.id_val)
            {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            memset(token->value.id_val, 0, n);
            strncpy(token->value.id_val, init, n-1);
            token_str_classify(token, init);
            break;
        }
    }
}


void token_str_classify(token_t *token, char *str)
{
    int i, done = 0;    
    int vcmds[] = {TOKEN_CMD_IF, TOKEN_CMD_ELSE, TOKEN_CMD_WHILE, TOKEN_CMD_FOR, TOKEN_CMD_RETURN};
    char *cmds[] = {"if", "else", "while", "for", "return"};

    int vtypes[] = {CMM_TYPE_CHAR, CMM_TYPE_INT, CMM_TYPE_REAL, CMM_TYPE_BOOL, CMM_TYPE_VOID};
    char *types[] = {"char", "int", "real", "bool", "void"};

    for (i = 0; i < 5; i++)
        if (!strcmp(token->value.id_val, cmds[i]))
        {
            free(token->value.id_val);
            token->type = vcmds[i];
            done = 1;
            break;
        }

    if (!done)
        for (i = 0; i < 5; i++)
            if (!strcmp(token->value.id_val, types[i]))
            {
                free(token->value.id_val);
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

void parse_number(token_t *token)
{
    char sym;
    char *str;
    size_t n = 1;
    char *init = buffer + (pos - 1);

    token->type = TOKEN_INT_CONST;
    token->lineno = lineno;

    while(1)
    {
        sym = cmm_getc();
        n++;
        if (!isdigit(sym))
        {
            cmm_ungetc();
            break;
        }
    }

    sym = cmm_getc();
    if (sym == '.')
    {
        token->type = TOKEN_REAL_CONST;    
        while(1)
        {
            sym = cmm_getc();
            n++;
            if (!isdigit(sym))
            {
                cmm_ungetc();
                n--;
                break;
            }
        }
    }
    else
    {
        cmm_ungetc();
        n--;
    }

    str = (char *)malloc(n);
    if (!str)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    memset(str, 0, n);
    strncpy(str, init, n);
    //printf("[%s]\n", str);

    if (token->type == TOKEN_INT_CONST)
        token->value.int_val = atoi(str);
    else
        token->value.real_val = atof(str);

    free(str);

}

void parse_par(token_t *token)
{
    token->type = (buffer[pos-1] == '(') ? TOKEN_LPAR : TOKEN_RPAR;
    token->lineno = lineno;       
}

void parse_square_bracket(token_t *token)
{
    token->type = (buffer[pos-1] == '[') ? TOKEN_LSQUARE_BRACKET : TOKEN_RSQUARE_BRACKET;
    token->lineno = lineno;   
}

void parse_curly_bracket(token_t *token)
{
    token->type = (buffer[pos-1] == '{') ? TOKEN_LCURLY_BRACKET : TOKEN_RCURLY_BRACKET;
    token->lineno = lineno;
}

void parse_collon(token_t *token)
{
    token->type = TOKEN_COLLON;
    token->lineno = lineno;
}

void parse_semicollon(token_t *token)
{
    token->type = TOKEN_SEMICOLLON;
    token->lineno = lineno;
}

void parse_op_rel(token_t *token)
{
    token->type = TOKEN_OP_REL;
    token->lineno = lineno;

    char prev_sym = buffer[pos-1];
    char sym = cmm_getc();
    if (prev_sym == '<')
    {
        if (sym == '=')
            token->value.op_rel_val = OP_REL_LTE;
        else
        {
            token->value.op_rel_val = OP_REL_LT;
            cmm_ungetc();
        }
    }
    else if (prev_sym == '>')
    {
        if (sym == '=')
            token->value.op_rel_val = OP_REL_GTE;
        else
        {
            token->value.op_rel_val = OP_REL_GT;
            cmm_ungetc();
        }
    }

    else if (prev_sym == '=')
    {
        if (sym == '=')
            token->value.op_rel_val = OP_REL_EQU;
        else
        {
            token->type = TOKEN_ATTRIB_OP;
            cmm_ungetc();
        }
    }
    else if (prev_sym == '!')
    {
        if (sym == '=')
            token->value.op_rel_val = OP_REL_NEQU;
        else
        {
            token->type = TOKEN_UNARY_NEG_OP;
        }
    }
}

void cmm_debug_token(token_t *token)
{
    if (!token)
        return;

    switch(token->type)
    {
        case TOKEN_ID: 
        {
            printf("<line: %lu, TOKEN_ID, \"%s\">\n", token->lineno, token->value.id_val);            
        }
        break;

        case TOKEN_OP_REL:
        {
            printf("<line: %lu, TOKEN_OP_REL, ", token->lineno);
            switch(token->value.op_rel_val)
            {
                case OP_REL_EQU: printf("OP_REL_EQU>\n"); break;
                case OP_REL_NEQU: printf("OP_REL_NEQU>\n"); break;
                case OP_REL_LT: printf("OP_REL_LT>\n"); break;
                case OP_REL_LTE: printf("OP_REL_LTE>\n"); break;
                case OP_REL_GT: printf("OP_REL_GT>\n"); break;
                case OP_REL_GTE: printf("OP_REL_GTE>\n"); break;
            }
        }
        break;

        case TOKEN_INT_CONST:
        {
            printf("<line: %lu, TOKEN_INT_CONST, %d>\n", token->lineno, token->value.int_val);
        }
        break;

        case TOKEN_REAL_CONST:
        {
            printf("<line: %lu, TOKEN_REAL_CONST, %f>\n", token->lineno, token->value.real_val);
        }
        break;

        case TOKEN_CHAR_CONST:
        {
            printf("<line: %lu, TOKEN_CHAR_CONST, char(%02x)>\n", token->lineno, token->value.char_val);
        }
        break;

        case TOKEN_STR_CONST:
        {
            printf("<line: %lu, TOKEN_STR_CONST, %s>\n", token->lineno, token->value.str_val);
        }
        break;

        case TOKEN_CMM_TYPE:
        {
            printf("<line: %lu, TOKEN_CMM_TYPE, ", token->lineno);
            switch(token->value.cmm_type_val)
            {
                case CMM_TYPE_CHAR: printf("CMM_TYPE_CHAR>\n"); break;
                case CMM_TYPE_INT: printf("CMM_TYPE_INT>\n"); break;
                case CMM_TYPE_REAL: printf("CMM_TYPE_REAL>\n"); break;
                case CMM_TYPE_BOOL: printf("CMM_TYPE_BOOL>\n"); break;
                case CMM_TYPE_VOID: printf("CMM_TYPE_VOID>\n"); break;
            }
        }
        break;

        case TOKEN_LPAR: printf("<line: %lu, TOKEN_LPAR, '('>\n", token->lineno); break;
        case TOKEN_RPAR: printf("<line: %lu, TOKEN_RPAR, ')'>\n", token->lineno); break;
        case TOKEN_LSQUARE_BRACKET: printf("<line: %lu, TOKEN_LSQUARE_BRACKET, '['>\n", token->lineno); break;
        case TOKEN_RSQUARE_BRACKET: printf("<line: %lu, TOKEN_RSQUARE_BRACKET, ']'>\n", token->lineno); break;
        case TOKEN_LCURLY_BRACKET: printf("<line: %lu, TOKEN_LCURLY_BRACKET, '{'>\n", token->lineno); break;
        case TOKEN_RCURLY_BRACKET: printf("<line: %lu, TOKEN_RCURLY_BRACKET, '}'>\n", token->lineno); break;
        case TOKEN_COLLON: printf("<line: %lu, TOKEN_COLLON, ','>\n", token->lineno); break;
        case TOKEN_SEMICOLLON: printf("<line: %lu, TOKEN_SEMICOLLON, ';'>\n",token->lineno); break;
        case TOKEN_UNARY_NEG_OP: printf("<line: %lu, TOKEN_UNARY_NEG_OP, '!'>\n", token->lineno); break;
        case TOKEN_CMD_IF: printf("<line: %lu, TOKEN_CMD_IF, \"if\">\n", token->lineno); break;
        case TOKEN_CMD_ELSE: printf("<line: %lu, TOKEN_CMD_ELSE, \"else\">\n", token->lineno); break;
        case TOKEN_CMD_WHILE: printf("<line: %lu, TOKEN_CMD_WHILE, \"while\">\n", token->lineno); break;
        case TOKEN_CMD_FOR: printf("<line: %lu, TOKEN_CMD_FOR, \"while\">\n", token->lineno); break;
        case TOKEN_CMD_RETURN: printf("<line: %lu, TOKEN_CMD_RETURN, \"return\">\n", token->lineno); break;
        case TOKEN_PLUS_SIGN: printf("<line: %lu, TOKEN_PLUS_SIGN, '+'>\n", token->lineno); break;
        case TOKEN_MINUS_SIGN: printf("<line: %lu, TOKEN_MINUS_SIGN, '-'>\n", token->lineno); break;
        case TOKEN_MUL_SIGN: printf("<line: %lu, TOKEN_MUL_SIGN, '*'>\n", token->lineno); break;
        case TOKEN_DIV_SIGN: printf("<line: %lu, TOKEN_DIV_SIGN, '/'>\n", token->lineno); break;
        case TOKEN_LOGICAL_AND: printf("<line: %lu, TOKEN_LOGICAL_AND, \"&&\">\n", token->lineno); break;
        case TOKEN_LOGICAL_OR: printf("<line: %lu, TOKEN_LOGICAL_OR, \"||\">\n", token->lineno); break;
        case TOKEN_ATTRIB_OP: printf("<line: %lu, TOKEN_ATTRIB_OP, '='>\n", token->lineno); break;

    }
}
