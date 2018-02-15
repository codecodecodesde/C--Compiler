//
//  globals.h
//  Lexical Analyzer
//

#include <stdlib.h>
#include <string>
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
#define COMM 14
#define LESS 15
#define GREATER 16
#define LESSEQUAL 17
#define GREATEREQUAL 18
#define ASSIGN  19
#define DOUBLEEQUAL   20
#define NOT 21
#define NOTEQUAL 22
#define ERROR   23
#define ENDFILE 24
#define INT 25
#define VOID 26
#define IF 27
#define ELSE 28
#define WHILE 29
#define RETURN 30
#define DOLLAR 31
#define BREAK 32
#define V_DEC 33
#define F_DEC 34
#define F_PARAM 35
#define BLOCK 36
#define LOCAL_V_DEC 37
#define VAR 38
#define CALL 39
#define INPUT 40
#define INIT -1
using namespace std;

typedef struct {
    int TokenClass;  /* one of the above */
    string TokenString;
} TokenType;

#define MAXCHILDREN 3

typedef struct TreeNode {
    struct TreeNode * child[MAXCHILDREN];
    struct TreeNode * sibling;
    string name;
    int type;//function return type or var dec type
    int op;//fun_dec|var_dec|param|var|assign|compare...
    int val;//num
    int isArray;//1: yes 0 : no
    int arraySize;//
    int index;//index or size;
} TreeNode;

#endif /* globals_h */
