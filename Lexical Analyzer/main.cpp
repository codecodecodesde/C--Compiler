//
//  main.cpp
//  Lexical Analyzer
//


#include <iostream>
#include <string>
#include "globals.h"

char c;
typedef enum {
    state_start, state_num, state_id, state_commentstart, state_less, state_greater, state_assign, state_not,state_done, state_error, state_commentbody, state_commentend
} StateType;

bool LayOutCharacter(char c) {
    return ((c == ' ') || (c == '\t') || (c == '\n'));
}

void printToken(TokenType currentToken);

TokenType getToken(){
    TokenType currentToken;
    currentToken.TokenString = "";
    StateType state = state_start;
    
    //ignore all blanks
    while(LayOutCharacter(c)){
        c = cin.get();
    }
    
    if(c == '/'){
        c = cin.get();
        if(c == '*'){
            char temp;
            int commentflag = 0;
            c = cin.get();
            do{
                temp = c;
                if(temp == EOF){
                    break;
                }
                c = cin.get();
                if(c == EOF){
                    break;
                }
                if(temp == '*' && c == '/'){
                    commentflag = 1;
                    c = cin.get();
                }
            }while(commentflag == 0);
        }else{
            cin.putback(c);
            c = '/';
        }
    }
    
    while(LayOutCharacter(c)){
        c = cin.get();
    }

    bool putbackFlag = false;
    while (state != state_done) {
        switch (state) {
            case state_start:
                if (isdigit(c)){
                    state = state_num;
                }
                else if (isalpha(c)) {
                    state = state_id;
                    currentToken.TokenClass = ID;
                }
                else if (c == '<'){
                    state = state_less;
                    currentToken.TokenClass = LESS;
                }
                else if (c == '>'){
                    state = state_greater;
                    currentToken.TokenClass = GREATER;
                }
                else if (c == '='){
                    state = state_assign;
                    currentToken.TokenClass = ASSIGN;
                }
                else if (c == '!'){
                    state = state_not;
                    currentToken.TokenClass = NOT;
                }else {
                    state = state_done;
                    switch (c){
                        case EOF:
                            currentToken.TokenClass = ENDFILE;
                            break;
                        case '+':
                            currentToken.TokenClass = PLUS;
                            break;
                        case '-':
                            currentToken.TokenClass = SUB;
                            break;
                        case '*':
                            currentToken.TokenClass = TIMES;
                            break;
                        case '/':
                            currentToken.TokenClass = DIV;
                            break;
                        case '(':
                            currentToken.TokenClass = LPAREN;
                            break;
                        case ')':
                            currentToken.TokenClass = RPAREN;
                            break;
                        case '[':
                            currentToken.TokenClass = LSBRACKET;
                            break;
                        case ']':
                            currentToken.TokenClass = RSBRACKET;
                            break;
                        case '{':
                            currentToken.TokenClass = LCBRACE;
                            break;
                        case '}':
                            currentToken.TokenClass = RCBRACE;
                            break;
                        case ';':
                            currentToken.TokenClass = SEMI;
                            break;
                        default:
                            currentToken.TokenClass = ERROR;
                            break;
                    }
                }
                break;
            case state_num:
                if (!isdigit(c)){
                    cin.putback(c);
                    putbackFlag = true;
                    state = state_done;
                    currentToken.TokenClass = NUM;
                }
                break;
            case state_id:
                if(!isalnum(c)){
                    cin.putback(c);
                    putbackFlag = true;
                    state = state_done;
                    currentToken.TokenClass = ID;
                }
                break;
            case state_not:// start-->'!'-->'!='
                if(c == '='){
                    state = state_done;
                    currentToken.TokenClass = NOTEQUAL;
                }else{
                    cin.putback(c);
                    putbackFlag = true;
                    state = state_done;
                    currentToken.TokenClass = NOT;
                }
                break;
            case state_assign:// start-->'='-->'=='
                if(c == '='){
                    state = state_done;
                    currentToken.TokenClass = DOUBLEEQUAL;
                }else{
                    cin.putback(c);
                    putbackFlag = true;
                    state = state_done;
                    currentToken.TokenClass = ASSIGN;
                }
                break;
            case state_less://start-->'<'--> '<='
                if(c == '='){
                    state = state_done;
                    currentToken.TokenClass = LESSEQUAL;
                }else{
                    cin.putback(c);
                    putbackFlag = true;
                    state = state_done;
                    currentToken.TokenClass = LESS;
                }
                break;
            case state_greater://start-->'>'--> '>='
                if(c == '='){
                    state = state_done;
                    currentToken.TokenClass = GREATEREQUAL;
                }else{
                    cin.putback(c);
                    putbackFlag = true;
                    state = state_done;
                    currentToken.TokenClass = GREATER;
                }
                break;
            case state_done:
                
            default:
                state = state_done;
                currentToken.TokenClass = ERROR;
                break;
        }
        if(!putbackFlag){
            currentToken.TokenString += c;
        }
        if (state != state_done){
            if(c != EOF){
                c = cin.get();
            }
        }
        
    }
    //if token is ID, continue to check if it is a keyword
    if(currentToken.TokenClass == ID){
        if(currentToken.TokenString == "if"){
            currentToken.TokenClass = IF;
        }else if(currentToken.TokenString == "else"){
            currentToken.TokenClass = ELSE;
        }else if(currentToken.TokenString == "int"){
            currentToken.TokenClass = INT;
        }else if(currentToken.TokenString == "void"){
            currentToken.TokenClass = VOID;
        }else if(currentToken.TokenString == "while"){
            currentToken.TokenClass = WHILE;
        }else if(currentToken.TokenString =="return"){
            currentToken.TokenClass = RETURN;
        }
    }
    
    return currentToken;
}

int main(int argc, const char * argv[]) {
    
    while((c = cin.get()) != EOF){
        TokenType nextToken;
        nextToken.TokenString = "";
        nextToken = getToken();
        
        
        
        printToken(nextToken);
    }
    return 0;
}
