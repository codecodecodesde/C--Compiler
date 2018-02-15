//
//  printAST.cpp
//  Parser
//
//  Created by 王璇 on 2017/11/01.
//  Copyright © 2017年 Xuan Wang. All rights reserved.
//

#include <stdio.h>
#include <string>
#include "globals.h"
#include <iostream>
#include <iomanip>
//void printAST(TreeNode * p, int indent);

string convert(int x){
    switch (x) {
        case INT:
            return "int";
        case VOID:
            return "void";
        case LESS:
            return "<";
        case LESSEQUAL:
            return "<=";
        case GREATER:
            return ">";
        case GREATEREQUAL:
            return ">=";
        case DOUBLEEQUAL:
            return "==";
        case NOTEQUAL:
            return "!=";
        case NOT:
            return "!";
        case ASSIGN:
            return "Assign";
        case PLUS:
            return "+";
        case SUB:
            return "-";
        case TIMES:
            return "*";
        case DIV:
            return "/";
        case INPUT:
            return "INPUT";
        default:
            break;
    }
    return "";
}

void printAST(TreeNode * p, int indent){
    if(p->op == V_DEC){
        cout << "Variable declarition: " << convert(p->type) << " " <<  p->name;
        if(p->isArray == 1){
            cout << "[" <<  p->arraySize << "]" << endl;
        }else{
            cout << endl;
        }
        
    }else if(p->op == F_DEC){
        cout << "Function declaration: " << convert(p->type) << " " << p->name << endl;

        TreeNode * t = p->child[0];
        if(t == NULL){
            cout << " Function parameters: None" << endl;
        }
        while(t != NULL){
            cout << " Function parameters: " << convert(t->type) << " " << t->name;
            if(t->isArray == 1){
                cout << "[]" << endl;
            }else{
                cout << endl;
            }
            t = t->sibling;
        }
        printAST(p->child[1], 2);
        
    }else if(p->op == BLOCK){
        cout << string(indent, ' ') << "Block" << endl;
        TreeNode * t = p->child[0];
        if(t == NULL){
            cout <<string(indent + 2, ' ') <<"Local_Var declaration: None" << endl;
        }
        while(t != NULL){
            cout << string(indent + 2, ' ') <<"Local_Var declaration: " << convert(t->type) << " " << t->name;
            if(t->isArray == 1){
                cout << "[" <<  t->arraySize << "]" << endl;
            }else{
                cout << endl;
            }
            t = t->sibling;
        }
        if(p->child[1] == NULL){
            cout << string(indent + 2, ' ') << "Local statements: None" << endl;
        }
        if(p->child[1] != NULL){
            cout << string(indent + 2, ' ') << "Local statements" << endl;
            printAST(p->child[1], indent + 4);
        }
        
    }else if(p->op == IF){
        cout << string(indent, ' ') << "Condition statement" << endl;
        cout << string(indent + 2, ' ') << "if-condition" << endl;
        printAST(p->child[0], indent + 4);
        if(p->child[1] != NULL){
            cout << string(indent + 2, ' ') << "if-statement" << endl;
            printAST(p->child[1], indent + 4);
        }
        if(p->child[2] != NULL){
            cout << string(indent + 2, ' ') << "else-statement" << endl;
            printAST(p->child[2], indent + 4);
        }
        
    }else if(p->op == WHILE){
        cout << string(indent, ' ') << "Repetition statement" << endl;
        cout << string(indent + 2, ' ') << "while-condition" << endl;
        printAST(p->child[0], indent + 4);
        if(p->child[1] != NULL){
            cout << string(indent + 2, ' ') << "while-statement" << endl;
            printAST(p->child[1], indent + 4);
        }
        
    }else if(p->op == RETURN){
        cout << string(indent, ' ') << "Return statement" << endl;
        if(p->child[0] != NULL){
            printAST(p->child[0], indent + 2);
        }
        
    }else if(p->op == DOLLAR){
        cout << string(indent, ' ') << "Out statement" << endl;
        if(p->child[0] != NULL){
            printAST(p->child[0], indent + 2);
        }
        
    }else if(p->op == CALL){
        cout << string(indent, ' ') << "Funcation call: " << p->name << endl;
        TreeNode * t = p->child[0];
        if(t == NULL){
            cout << string(indent + 2, ' ') << "Funcation Arguments: None" << endl;
        }else{
            cout <<string(indent + 2, ' ') <<"Function Argument: "<< endl;
        }
        
        if(t != NULL){
            printAST(t, indent + 4);
        }
    }else if(p->op == VAR){
        cout << string(indent, ' ') << "Variable: " << p->name;
        if(p->isArray == 1){
            cout<< "[]"<< endl;
            cout<< string(indent + 2, ' ')<< "index: "<< endl;
            printAST(p->child[0], indent + 4);
        }else{
            cout << endl;
        }
    }else if(p->op == NUM){
        cout << string(indent, ' ') << "Const: " << p->val << endl;
    }else if(p -> op == BREAK){
        cout << string(indent, ' ') << "Break" << endl;
    }else{
        cout << string(indent, ' ') <<"Op: " << convert(p->op) << endl;
        for(int i = 0; i < MAXCHILDREN ; i++){
            if(p->child[i] != NULL){
                printAST(p->child[i], indent + 2);
            }
        }
    }
    
    if(p->sibling != NULL){
        printAST(p->sibling, indent);
    }
}
