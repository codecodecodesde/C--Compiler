//
//  parse.cpp
//  Parser
//

#include "globals.h"
#include <stdio.h>
#include <iostream>
TokenType getToken();
TreeNode* block();
TreeNode * statments();
TreeNode * statement();
TreeNode *term();
TreeNode * expression();
TreeNode * ID_tail(TreeNode * left);
static TokenType token;


//function newNode: creates a new node for syntax tree construction

TreeNode * newNode(TokenType tType) {
    TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if (t==NULL) {
        //cerr << "Out of memory error at line %d\n";
        exit(1);
    }
    else {
        for (i=0;i < MAXCHILDREN;i++) t->child[i] = NULL;
        t->sibling = NULL;
        t->isArray = 0;//not array
        t->arraySize = INIT;
        t->index = INIT;
        t->op = token.TokenClass;
        t->name = " ";
        t->type = 0;//return type or declaration type
    }
    return t;
}

//function next: look ahead one token
static void next(int expected) {
    if (token.TokenClass == expected) token = getToken();
    else {
        cerr << "unexpected token -> " << token.TokenString << endl;
        exit(1);
    }
}

TreeNode * arguments(){
    TreeNode * head = NULL , *tail = NULL;
    while(token.TokenClass != RPAREN){
        TreeNode * p = expression();
        if(p == NULL){
            continue;//this statement is ;
        }
        if(head == NULL){
            head = p;
            tail = p;
        }else{
            tail->sibling = p;
            tail = p;
        }
        if(token.TokenClass == COMM){
            next(COMM);
        }else if(token.TokenClass != RPAREN){
            cerr << "invalid function call argument: invalid element\n" << endl;;
            exit(1);
        }
    }
    
    if(token.TokenClass == EOF){
        cerr << "lack ) in funcation call.\n" << endl;
        exit(1);
    }
    return head;
}

TreeNode * factor(){
    if(token.TokenClass == LPAREN){
        next(LPAREN);
        TreeNode * p = expression();
        next(RPAREN);
        return p;
    }
    TreeNode * p = newNode(token);
    if(token.TokenClass == NUM){
        p->val = atoi(token.TokenString.c_str());
        next(NUM);
        return p;
    }else if(token.TokenClass == INPUT){
        p->name = token.TokenString;
        next(INPUT);
        return p;
    }else if(token.TokenClass == ID){
        p->name = token.TokenString;
        next(ID);
        return ID_tail(p);
    }else{
        cerr << "invalid element in factor.\n" << endl;
    }
    return p;
}

TreeNode *term_tail(TreeNode * left){
    if(token.TokenClass == TIMES || token.TokenClass == DIV){
        TreeNode *p = newNode(token);
        
        next(token.TokenClass);
        p->child[0] = left;
        p->child[1] = factor();
        return term_tail(p);
    }else{
        return left;
    }
}

TreeNode *expr_tail(TreeNode * left){
    if(token.TokenClass == PLUS || token.TokenClass == SUB){
        TreeNode * p = newNode(token);
        next(token.TokenClass);
        p->child[0] = left;
        p->child[1] = term();
        return expr_tail(p);
    }else{
        return left;
    }
}

TreeNode *term(){
    TreeNode *t = factor();
    return term_tail(t);
}

TreeNode * arith_expr(){
    TreeNode * t = term();
    return expr_tail(t);
}

TreeNode * var_tail(TreeNode * left){
    if(token.TokenClass == ASSIGN){
        next(ASSIGN);
        TreeNode *p = newNode(token);
        p->op = ASSIGN;
        p->child[0] = left;
        p->child[1] = expression();
        return p;
    }else if(token.TokenClass == PLUS || token.TokenClass == SUB){
        return expr_tail(left);
    }else if(token.TokenClass == TIMES || token.TokenClass == DIV){
        TreeNode *p = term_tail(left);
        return expr_tail(p);
    }
    return left;
}

TreeNode * ID_tail(TreeNode * left){
    if(token.TokenClass == LSBRACKET){
        next(LSBRACKET);
        left->op = VAR;
        left->isArray = 1;
        left->child[0] = expression();//index
        if(token.TokenClass == RSBRACKET){
            next(RSBRACKET);
        }else{
            cerr << "invalid var: lack ]" << endl;
            exit(1);
        }
    }else if(token.TokenClass == LPAREN){
        left->op = CALL;
        next(LPAREN);
        left->child[0] = arguments();
        next(RPAREN);
    }else{
        left->op = VAR;
    }
    return left;
}

TreeNode * expression(){
    TreeNode * p;
    if(token.TokenClass == ID){
        TreeNode * t = newNode(token);
        t->name = token.TokenString;
        next(ID);
        p = ID_tail(t);
        return var_tail(p);
    }else{
        p = arith_expr();
    }
    return p;
}

TreeNode * expr_stmt(){
    if(token.TokenClass == SEMI){
        next(SEMI);
        return NULL;
    }else{
        TreeNode * p = expression();
        if(token.TokenClass != SEMI){
            cerr << "lack ; after expression" << endl;
        }
        next(SEMI);
        return p;
    }
}

TreeNode * compare_tail(TreeNode * left){
    if(token.TokenClass == LESS || token.TokenClass == GREATER
       || token.TokenClass == LESSEQUAL || token.TokenClass == GREATEREQUAL
       || token.TokenClass == DOUBLEEQUAL || token.TokenClass == NOTEQUAL){
        TreeNode * p = newNode(token);
        next(token.TokenClass);
        p->child[0] = left;
        p->child[1] = arith_expr();
        return p;
    }else{
        cerr << "invalid relop.\n" << endl;
        exit(1);
    }
}
TreeNode * jumpout(){
    TreeNode * p = newNode(token);
    next(BREAK);
    return p;
}

TreeNode * dollar(){
    TreeNode * p = newNode(token);
    next(DOLLAR);
    p->child[0] = expr_stmt();
    return p;
}

TreeNode * return_stmt(){
    TreeNode * p = newNode(token);
    next(RETURN);
    p->child[0] = expr_stmt();
    return p;
}

TreeNode * comparison(){
    if(token.TokenClass == NOT){
        TreeNode * p = newNode(token);
        next(NOT);
        next(LPAREN);
        p->child[0] = comparison();
        next(RPAREN);
        return p;
    }
    TreeNode * p = arith_expr();
    return compare_tail(p);
}

TreeNode * repetition(){
    TreeNode * p = newNode(token);
    next(WHILE);
    next(LPAREN);
    p->child[0] = comparison();
    next(RPAREN);
    p->child[1] = statement();
    return p;
}

TreeNode * selection(){
    TreeNode * p = newNode(token);
    next(IF);
    next(LPAREN);
    p->child[0] = comparison();
    next(RPAREN);
    p->child[1] = statement();
    if(token.TokenClass == ELSE){
        next(ELSE);
        p->child[2] = statement();
    }
    return p;
}

TreeNode * statement(){
    TreeNode *p;
    if(token.TokenClass == LCBRACE){
        next(LCBRACE);
        p = block();
        next(RCBRACE);
    }else if(token.TokenClass == IF){
        p = selection();
    }else if(token.TokenClass == WHILE){
        p = repetition();
    }else if(token.TokenClass == RETURN){
        p = return_stmt();
    }else if(token.TokenClass == DOLLAR){
        p = dollar();
    }else if(token.TokenClass == BREAK){
        p = jumpout();
    }else{
        p = expr_stmt();
    }
    return p;
}

TreeNode * statments(){
    TreeNode *head = NULL, *tail = NULL;
    while(token.TokenClass != RCBRACE){
        TreeNode *p = statement();
        if(p == NULL){
            continue;//this statement is ;
        }
        if(head == NULL){
            head = p;
            tail = p;
        }else{
            tail->sibling = p;
            tail = p;
        }
    }
    
    if(token.TokenClass == EOF){
        cerr << "lack } in function block.\n" << endl;
        exit(1);
    }
    return head;
}

TreeNode * local_var(){
    TreeNode *head = NULL, *tail = NULL;
    while(token.TokenClass == INT || token.TokenClass == VOID){
        TreeNode * p = newNode(token);
        if (head == NULL){
            head = tail = p;
        }else {
            tail->sibling= p;
            tail = p;
        }
        
        p->type = token.TokenClass;//local var declaration type
        p->op = LOCAL_V_DEC;
        next(token.TokenClass);
        if(token.TokenClass == ID){
            p->name = token.TokenString;
            next(ID);
            if(token.TokenClass == SEMI){
                next(SEMI);
            }else if(token.TokenClass == LSBRACKET){
                next(LSBRACKET);
                p->isArray = 1;
                if(token.TokenClass == NUM){
                    p->arraySize = atoi(token.TokenString.c_str());
                    next(NUM);
                    if(token.TokenClass == RSBRACKET){
                        next(RSBRACKET);
                        if(token.TokenClass == SEMI){
                            next(SEMI);
                        }else{
                            cerr << "invalid local variable declaration: lack ;\n"<< endl;
                            exit(1);
                        }
                    }else{
                        cerr << "invalid local variable declaration: lack ]\n" << endl;
                        exit(1);
                    }
                }else{
                    cerr << "invalid local variable declaration: invalid array size\n"<< endl;
                    exit(1);
                }
            }else{
                cerr << "invalid local variable declaration: invalid element\n" << token.TokenString << endl;
                exit(1);
            }
        }else{
            cerr << "invalid local variable declaration: var_name error\n" << token.TokenString << endl;
        }
    }
    return head;
}

TreeNode* block(){
    TreeNode * p = newNode(token);
    p->op = BLOCK;
    if(token.TokenClass == INT || token.TokenClass == VOID){
        //exist local variable declaration
        p->child[0] = local_var();
        p->child[1] = statments();
    }else{
        p->child[1] = statments();
    }
    return p;
}

TreeNode* parameters(){
    TreeNode *head = NULL, *tail = NULL;
    if(token.TokenClass == RPAREN){
        cerr << "function parameters error.\n" << endl;
        exit(1);
    }
    while (token.TokenClass != RPAREN) {
        TreeNode * p = newNode(token);
        if (head == NULL){
            head = tail = p;
        }else {
            tail->sibling= p;
            tail = p;
        }
        if(token.TokenClass == VOID || token.TokenClass == INT){
            p->type = token.TokenClass;
            p->op = F_PARAM;
            next(token.TokenClass);
            if(token.TokenClass == ID){
                p->name = token.TokenString;
                next(ID);
                if(token.TokenClass == COMM){
                    next(COMM);
                }else if(token.TokenClass == LSBRACKET){
                    p->isArray = 1;
                    next(LSBRACKET);
                    if(token.TokenClass == RSBRACKET){
                        next(RSBRACKET);
                        if(token.TokenClass == COMM){
                            next(COMM);
                        }else if(token.TokenClass != RPAREN){
                            cerr << "invalid func parameter: invalid element\n";
                            exit(1);
                        }
                    }else{
                        cerr << "invalid func parameter: lack ]\n";
                        exit(1);
                    }
                }else if(token.TokenClass != RPAREN){
                    cerr << "invalid func parameter: invalid element\n";
                    exit(1);
                }
            }else if (!(token.TokenClass == RPAREN && p->type == VOID)){
                cerr << "invalid func parameter: name error\n";
                exit(1);
            }
        }else{
            cerr << "invalid func parameter: type error\n";
            exit(1);
        }
    }
    return head;
}

TreeNode * declarations() {
    TreeNode *head = NULL, *tail = NULL;
    token = getToken();
    while (token.TokenClass != ENDFILE) {
        TreeNode * p = newNode(token);
        if (head == NULL){
            head = tail = p;
        }else {
            tail->sibling= p;
            tail = p;
        }
        
        if(token.TokenClass == VOID || token.TokenClass == INT){
            p->type = token.TokenClass;
            next(token.TokenClass);//type id dec-tail
            if(token.TokenClass == ID){
                p->name = token.TokenString;
                next(token.TokenClass);//;|[|(
                if(token.TokenClass == SEMI){
                    //var-dec: type id;
                    p->op = V_DEC;
                    next(SEMI);
                }else if(token.TokenClass == LSBRACKET){
                    //var-dec: type id[N];
                    p->op = V_DEC;
                    p->isArray = 1;
                    next(LSBRACKET);
                    if(token.TokenClass == NUM){
                        p->arraySize = atoi(token.TokenString.c_str());
                        next(NUM);
                        if(token.TokenClass == RSBRACKET){
                            next(RSBRACKET);
                            if(token.TokenClass != SEMI){
                                cerr << "invalid array variable declaration: semicolon error" << p->name << endl;
                                exit(1);
                            }else{
                                next(SEMI);
                            }
                        }else{
                            cerr << "invalid array variable declaration: ] error" << p->name << endl;
                            exit(1);
                        }
                    }else{
                        cerr << "invalid array variable declaration: array size error" << p->name << endl;
                        exit(1);
                    }
                }else if(token.TokenClass == LPAREN){
                    //func-dec: type id () or type id (type id, type id[], ....)
                    p->op = F_DEC;
                    next(LPAREN);
                    p->child[0] = parameters();
                    next(RPAREN);
                    if(token.TokenClass == LCBRACE){
                        next(LCBRACE);
                        p->child[1] = block();
                        next(RCBRACE);
                    }else{
                        cerr << "invalid function declaration: lack }\n"<< endl;
                        exit(1);
                    }
                }else{
                    cerr << "invalid variable or function declaration: invlaid element"<< endl;
                    exit(1);
                }
            }else{
                cerr << "invalid declaration: name error\n";
                exit(1);
            }
            
            
        }else{
            cerr << "invalid declaration: type error\n";
            exit(1);
        }
    }
    return head;
}
