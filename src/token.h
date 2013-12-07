#ifndef _TOKEN_H
#define _TOKEN_H

#include <stdio.h>

enum token_type
{
    TOKEN_ID,
    TOKEN_OP_REL,// == | != | < | <= | > | >=
    TOKEN_CMM_TYPE, // char |  int  | real | bool | void
    TOKEN_LPAR, // '('
    TOKEN_RPAR, // ')'
    TOKEN_LSQUARE_BRACKET, // '['
    TOKEN_RSQUARE_BRACKET, // ']'
    TOKEN_LCURLY_BRACKET, // '{'
    TOKEN_RCURLY_BRACKET, // '}'
    TOKEN_COLLON, // ','
    TOKEN_SEMICOLLON, // ';'
    TOKEN_UNARY_NEG_OP, // '!'
    TOKEN_CMD_IF,
    TOKEN_CMD_ELSE,
    TOKEN_CMD_WHILE,
    TOKEN_CMD_FOR,
    TOKEN_CMD_RETURN,
    TOKEN_PLUS_SIGN, // '+'
    TOKEN_MINUS_SIGN, // '-'
    TOKEN_MUL_SIGN, // '*'
    TOKEN_DIV_SIGN, // '/'
    TOKEN_LOGICAL_AND, // && 
    TOKEN_LOGICAL_OR, // ||
    TOKEN_ATTRIB_OP // '='
};

enum cmm_type
{
    CMM_CHAR,
    CMM_INT,
    CMM_REAL,
    CMM_BOOL,
    CMM_VOID
};

enum op_rel
{
    OP_REL_EQU,
    OP_REL_NEQU,
    OP_REL_LT,
    OP_REL_LTE,
    OP_REL_GT,
    OP_REL_GTE
};


typedef struct token_ 
{
    size_t lineno;
    enum token_type type;
    union
    {
        char *id_val;
        int int_val;
        float real_val;
        char char_val;
        char *str_val;
        enum cmm_type cmm_type_val;
        enum op_rel op_rel_val;
    } value;
    
    struct token_ *prev, *next;
    
} token_t;



#endif
