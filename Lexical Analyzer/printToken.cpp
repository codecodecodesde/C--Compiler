//
//  printToken.cpp
//  Lexical Analyzer
//


#include <stdio.h>
#include <iomanip>
#include <iostream>
#include "globals.h"
void printToken(TokenType currentToken){
    switch (currentToken.TokenClass) {
        case ID:
            cout << setw(20) << left << "ID" << currentToken.TokenString << endl;
            break;
        case NUM:
            cout << setw(20) << left << "NUM" << currentToken.TokenString << endl;
            break;
        case PLUS:
            cout << setw(20) << left << "PLUS" << currentToken.TokenString << endl;
            break;
        case SUB:
            cout << setw(20) << left << "SUB" << currentToken.TokenString << endl;
            break;
        case TIMES:
            cout << setw(20) << left << "TIMES" << currentToken.TokenString << endl;
            break;
        case DIV:
            cout << setw(20) << left << "DIV" << currentToken.TokenString << endl;
            break;
        case LPAREN:
            cout << setw(20) << left << "LEFT PARAENTHESES" << currentToken.TokenString << endl;
            break;
        case RPAREN:
            cout << setw(20) << left << "RIGHT PARENTHESES" << currentToken.TokenString << endl;
            break;
        case LSBRACKET:
            cout << setw(20) << left << "LEFT BRACKET" << currentToken.TokenString << endl;
            break;
        case RSBRACKET:
            cout << setw(20) << left << "RIGHT BRACKET" << currentToken.TokenString << endl;
            break;
        case LCBRACE:
            cout << setw(20) << left << "LEFT BRACE" << currentToken.TokenString << endl;
            break;
        case RCBRACE:
            cout << setw(20) << left << "RIGHT BRACE" << currentToken.TokenString << endl;
            break;
        case LESS:
            cout << setw(20) << left << "LESS" << currentToken.TokenString << endl;
            break;
        case LESSEQUAL:
            cout << setw(20) << left << "LESSEQUAL" << currentToken.TokenString << endl;
            break;
        case GREATER:
            cout << setw(20) << left << "GREATER" << currentToken.TokenString << endl;
            break;
        case GREATEREQUAL:
            cout << setw(20) << left << "GREATEREQUAL" << currentToken.TokenString << endl;
            break;
        case ASSIGN:
            cout << setw(20) << left << "ASSIGN" << currentToken.TokenString << endl;
            break;
        case DOUBLEEQUAL:
            cout << setw(20) << left << "DOUBLE EQUAL" << currentToken.TokenString << endl;
            break;
        case NOT:
            cout << setw(20) << left << "NOT" << currentToken.TokenString << endl;
            break;
        case NOTEQUAL:
            cout << setw(20) << left << "NOT EQUAL" << currentToken.TokenString << endl;
            break;
        case SEMI:
            cout << setw(20) << left << "SEMICOLON" << currentToken.TokenString << endl;
            break;
        case ERROR:
            cout << setw(20) << left << "ERROR" << "illegal token" << endl;
            break;
        case INT:
            cout << setw(20) << left << "INT" << currentToken.TokenString << endl;
            break;
        case VOID:
            cout << setw(20) << left << "VOID" << currentToken.TokenString << endl;
            break;
        case IF:
            cout << setw(20) << left << "IF" << currentToken.TokenString << endl;
            break;
        case ELSE:
            cout << setw(20) << left << "ELSE" << currentToken.TokenString << endl;
            break;
        case WHILE:
            cout << setw(20) << left << "WHILE" << currentToken.TokenString << endl;
            break;
        case RETURN:
            cout << setw(20) << left << "RETURN" << currentToken.TokenString << endl;
            break;
        case ENDFILE:
            cout << setw(20) << left << "EOF" << "eof" << endl;
            break;
        default:
            break;
    }
}
