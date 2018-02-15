//
//  analyze.cpp
//  Semantic Analysis
//
//  Created by 王璇 on 2017/11/15.
//  Copyright © 2017年 Xuan Wang. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <set>
#include "globals.h"
SymbolTable* analyze_block(TreeNode* root, string name, SymbolTable* parent);

nodeInfo* getNodeInfo(SymbolTable* current, string key){
    while(current->table.find(key) == current->table.end()){
        current = current->parent;
        if(current == NULL){
            return 0;
        }
    }
    return &current->table.at(key);
}

int analyze_expr(TreeNode* root, SymbolTable* current){
    if(root->op == VAR){
        //find var declaration
        nodeInfo* info = getNodeInfo(current, root->name);
        if(info == 0){
            cerr << root->name << " is undeclared before use" << endl;
            exit(1);
        }
        if(info->isArray != root->isArray){
            cerr << root->name << " is undeclared before use" << endl;
            cerr << root->name << " may be a previous defined array" << endl;
            exit(1);
        }else{
            if(info->isArray == true){
                if(analyze_expr(root->child[0], current) != INT){
                    cerr << "index error in variable " << root->name << endl;
                    exit(1);
                }
            }
        }
        return info->type;
    }else if(root->op == CALL){
        //find function declararion
        nodeInfo* info = getNodeInfo(current, root->name);
        if(info == 0){
            cerr << root->name << " is undeclared before use" << endl;
            exit(1);
        }
        TreeNode* t = root->child[0];//arguments
        params* p = info->paramList;//parameters
        
        //f(void)
        if(p->type == VOID && p->param_name == ""){
            if(t != NULL){
                cerr << "too many arguments in function call: " << root->name << endl;
                exit(1);
            }
        }else{
            //f(int x, int y[])
            while(t != NULL && p != NULL){
                if(p->isArray){
                    if(t->op == VAR && t->child[0] == NULL){
                        nodeInfo* info = getNodeInfo(current, t->name);
                        if(info == 0){
                            cerr << t->name << "is undeclared before use" << endl;
                            exit(1);
                        }
                        if(info->isArray == false){
                            cerr << "invalid argument: " << t->name << endl;
                            cerr << "unexpected type arguments in function call" <<endl;
                            exit(1);
                        }
                    }else{
                        cerr << "unexpected type arguments in function call" <<endl;
                        exit(1);
                    }
                }else if(p->type != analyze_expr(t, current)){
                    cerr << "invalid argument type in function: " << root->name << endl;
                    cerr << t->name << " has different data type from function declaration" << endl;
                    exit(1);
                }
                p = p->sibling;
                t = t->sibling;
            }
            if(t == NULL && p != NULL){
                cerr << "too few arguments to function call: " << root->name << endl;
                exit(1);
            }
            if(p == NULL && t != NULL){
                cerr << "too many arguments to function call: " << root->name << endl;
                exit(1);
            }
        }
        return info->type;
    }else if(root->op == PLUS || root->op == SUB || root->op == TIMES || root->op == DIV){
        //type check
        int type1 = analyze_expr(root->child[0], current);
        int type2 = analyze_expr(root->child[1], current);
        if(type1 != type2 || type1 == VOID || type1 == VOID){
            cerr << "void variable cannot be in arithmatic expression" << endl;
            exit(1);
        }
        return INT;
    }else if(root->op == ASSIGN){
        //update symbol table
        if(root->child[0]->op != VAR){
            cerr << "expression is not assignable" << endl;
            exit(1);
        }
        int varType = analyze_expr(root->child[0], current);
        int exprType = analyze_expr(root->child[1], current);
        if(varType != exprType){
            cerr << "variable cannot be assigned with different type data" << endl;
            exit(1);
        }
        return varType;
    }else{
        return INT;
    }
}

void analyze_comparison(TreeNode* root, SymbolTable* current){
    if(root->op == NOT){
        analyze_comparison(root->child[0], current);
    }else if(root->op == LESS || root->op == GREATER
             || root->op == LESSEQUAL || root->op == GREATEREQUAL
             || root->op == DOUBLEEQUAL || root->op == NOTEQUAL){
        analyze_expr(root->child[0], current);
        analyze_expr(root->child[1], current);
    }
}

SymbolTable* findPos(SymbolTable* current){
    SymbolTable* st = current->child;
    while(st->sibling != NULL){
        st = st->sibling;
    }
    return st;
}

void analyze_statement(TreeNode* root, string curScopeName, SymbolTable* current){
    if(root->op == IF){
        analyze_comparison(root->child[0], current);
        if(root->child[1] != NULL){
            analyze_statement(root->child[1], curScopeName + ".if-stmt", current);
        }
        if(root->child[2] != NULL){
            analyze_statement(root->child[1], curScopeName + ".else-stmt", current);
        }
    }else if(root->op == WHILE){
        current->table.insert(pair<string, nodeInfo>("loop", *(new nodeInfo)));
        analyze_comparison(root->child[0], current);
        if(root->child[1] != NULL){
            analyze_statement(root->child[1], curScopeName + ".while-stmt", current);
        }
        current->table.erase("loop");
    }else if(root->op == RETURN){
        if(root->child[0] != NULL){
            analyze_expr(root->child[0], current);
        }
    }else if(root->op == DOLLAR){
        analyze_expr(root->child[0], current);
    }else if(root->op == BLOCK){
        if(current->child == NULL){
            current->child = analyze_block(root, curScopeName, current);
        }else{
            SymbolTable* p = findPos(current);
            p->sibling = analyze_block(root, curScopeName, current);
        }
    }else if(root->op == BREAK){
        //search break in previous tables
        if(getNodeInfo(current, "loop") == NULL){
            cerr << "break statement must occur in while-stmt" << endl;
            exit(1);
        }
    }else{
        analyze_expr(root, current);
    }
}


SymbolTable* analyze_block(TreeNode* root, string name, SymbolTable* parent){
    SymbolTable* st = new SymbolTable();
    st->scopeName = name;
    st->parent = parent;
    
    //local var declaration
    TreeNode* p = root->child[0];
    while(p != NULL){
        if(st->table.find(p->name) == st->table.end()){
            nodeInfo* info = new nodeInfo();
            info->type = p->type;
            info->decType = LOCAL_V_DEC;
            if(p->isArray){
                info->isArray = true;
                info->arraySize = p->arraySize;
            }
            st->table.insert(pair<string, nodeInfo>(p->name, *info));
        }else{
            cerr << "redefinition of local variable " << p->name << " in" << st->scopeName << endl;
            exit(1);
        }
        p = p->sibling;
    }
    
    p = root->child[1];
    while(p != NULL){
        analyze_statement(p, st->scopeName, st);
        p = p->sibling;
    }
    return st;
}

void analyze_params(TreeNode* head, nodeInfo* info){
    set<string> *parameters = new set<string>();
    params* h = NULL, *t = NULL;
    while(head != NULL){
        params* p = new params();
        p->type = head->type;
        p->isArray = head->isArray;
        p->param_name = head->name;
        if(parameters->count(head->name) == 1){
            cerr << "redefinition of function parameter " << head->name << endl;
            exit(1);
        }
        parameters->insert(head->name);
        if(h == NULL){
            h = p;
            t = p;
        }else{
            t->sibling = p;
            t = p;
        }
        head = head->sibling;
    }
    info->paramList = h;
}

SymbolTable* analyze(TreeNode * root){
    SymbolTable* st = new SymbolTable();
    st->scopeName = "global";
    while(root != NULL){
        if(root -> op == V_DEC){
            if(st->table.find(root->name) == st->table.end()){
                nodeInfo* info = new nodeInfo();
                info->type = root->type;
                info->decType = V_DEC;
                if(root->isArray){
                    info->isArray = true;
                    info->arraySize = root->arraySize;
                }
                st->table.insert(pair<string, nodeInfo>(root->name, *info));
            }else{
                cerr << "Error in " << st->scopeName << ":";
                cerr << " redefinition of variable " << root->name << endl;
                exit(1);
            }
        }else if(root->op == F_DEC){
            if(st->table.count(root->name) == 0){
                nodeInfo *info = new nodeInfo();
                info->type = root->type;
                info->decType = F_DEC;
                analyze_params(root->child[0], info);
                st->table.insert(pair<string, nodeInfo>(root->name, *info));
                if(st->child == NULL){
                    st->child = analyze_block(root->child[1], root->name, st);
                }else{
                    SymbolTable* p = findPos(st);
                    p->sibling = analyze_block(root->child[1], root->name, st);
                }
            }else{
                cerr << "Error in " << st->scopeName << ":";
                cerr << " redefinition of function " << root->name << endl;
                exit(1);
            }
        }
        root = root->sibling;
    }
    return st;
}
