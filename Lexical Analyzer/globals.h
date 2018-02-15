//
//  globals.h
//  Lexical Analyzer
//

#include <stdlib.h>
#ifndef globals_h
#define globals_h

#define ID 1
#define NUM 2
#define PLUS    3
#define SUB     4
#define TIMES   5
#define DIV     6
#define LPAREN  7
#define RPAREN  8
#define LSBRACKET 9
#define RSBRACKET 10
#define LCBRACE 11
#define RCBRACE 12
#define SEMI 13
#define LESS 14
#define GREATER 15
#define LESSEQUAL 16
#define GREATEREQUAL 17
#define ASSIGN  18
#define DOUBLEEQUAL   19
#define NOT 20
#define NOTEQUAL 21
#define ERROR   22
#define ENDFILE 23
#define INT 24
#define VOID 25
#define IF 26
#define ELSE 27
#define WHILE 28
#define RETURN 29
#define COMMENT 30

using namespace std;


typedef struct {
    int TokenClass;  /* one of the above */
    string TokenString;
} TokenType;

#endif /* globals_h */
