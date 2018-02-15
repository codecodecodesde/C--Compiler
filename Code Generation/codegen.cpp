//
//  codegen.cpp
//  Code Generation
//


#include <stdio.h>
#include <iostream>
#include <string>
#include "globals.h"

#define  DADDR_SIZE 1024
#define R0 0
#define R1 1
#define R2 2
#define SP 6
#define FP 5
#define GP 3
#define PC 7
#define ZERO 4


const int size = DADDR_SIZE;
static CodeType codeArray[size];
int globalOffset = 0;
int ICounter = 1;
SymbolTable* global;

SymbolTable* findPos(SymbolTable* current);
SymbolTable* codeGen_block(TreeNode* root, string name, SymbolTable* parent);

void emit(string code, OpCodeType ctype, int oprand1, int oprand2, int oprand3){
    codeArray[ICounter].opcode = code;
    codeArray[ICounter].ctype = ctype;
    codeArray[ICounter].rand1 = oprand1;
    codeArray[ICounter].rand2 = oprand2;
    codeArray[ICounter].rand3 = oprand3;
    ICounter++;
}

void emitWithIdx(string code, OpCodeType ctype, int oprand1, int oprand2, int oprand3, int idx){
    codeArray[idx].opcode = code;
    codeArray[idx].ctype = ctype;
    codeArray[idx].rand1 = oprand1;
    codeArray[idx].rand2 = oprand2;
    codeArray[idx].rand3 = oprand3;
}

int getOffsetLocal(SymbolTable* st, string name){
    if(st->scopeName.compare("global") == 0){
        return -1;
    }
    while(st->table.find(name) == st->table.end()){
        st = st->parent;
        if(st->scopeName.compare("global") == 0){
            return - 1;
        }
    }
    return st->table.at(name).offset;
}

int getOffsetParams(string funcName, string paramName){
    nodeInfo info = global->table.find(funcName)->second;
    int offset = 0;
    params* param = info.paramList;
    while(param != NULL){
        if(param->param_name.compare(paramName) == 0){
            offset = param->offset;
            break;
        }else{
            param = param->sibling;
        }
    }
    return offset;
}

int getOffSetGlobal(string name){
    nodeInfo info = global->table.find(name)->second;
    return info.offset;
}

int getStartIdx(string name){
    nodeInfo info = global->table.find(name)->second;
    return info.Ibegin;
}

int getNumOfParams(string name){
    nodeInfo info = global->table.find(name)->second;
    return info.numOfParams;
}

void codeGen_expr(TreeNode* root, SymbolTable* current){
    if(root->op == VAR){//retrieve val
        if(root->isArray){
            codeGen_expr(root->child[0], current);//R0
            
            //1. local var
            //2. function parameter
            //3. global var
            int offset = getOffsetLocal(current, root->name);
            if(offset == -1){//not local
                
                //find in function params
                offset = getOffsetParams(current->scopeName, root->name);
                if(offset == 0){
                    //find in global params
                    offset = getOffSetGlobal(root->name);
                    emit("LDA", RM, R1, -offset, GP);
                    emit("SUB", RO, R0, R1, R0);
                }else{//param var
                    emit("LD", RM, R1, offset, FP);
                    emit("ADD", RO, R0, R0, R1);
                }
            }else{//local var
                emit("LDA", RM, R1, offset, FP);//get INTARRAY addr
                emit("ADD", RO, R0, R0, R1);
            }
            
            emit("LD", RM, R0, 0, R0);
            
        }else{
            //load integer in local ST
            int offset = getOffsetLocal(current, root->name);
            if(offset == -1){
                offset = getOffsetParams(current->scopeName, root->name);
                if(offset == 0){
                    offset = getOffSetGlobal(root->name);
                    emit("LD", RM, R0, -offset, GP);
                }else{
                    emit("LD", RM, R0, offset, FP);
                }
                
            }else{
                emit("LD", RM, R0, offset, FP);
            }
        }
    
    }else if(root->op == CALL){
        //TODO:????
        TreeNode *p = root->child[0];
        while(p != NULL){
            codeGen_expr(p, current);//R0
            p = p-> sibling;
            emit("ST", RM, R0, 0, SP);
            emit("LDA", RM, SP, 1, SP);
        }

        //get return Addr and store it
        emit("LDA", RM, R0, 3, PC);
        
        //3 instructions
        emit("ST", RM, R0, 0, SP);
        emit("LDA", RM, SP, 1, SP);
        
        int startInum = getStartIdx(root->name);//find the beginning instruction ICounter
        emit("LDC", RM, PC, startInum, ZERO);//give control to callee
        
        emit("LD", RM, FP, 1, FP);//reset FP to callee
    
    }else if(root->op == PLUS || root->op == SUB || root->op == TIMES || root->op == DIV){
        
        codeGen_expr(root->child[0], current);
        emit("ST", RM, R0, 0, SP);
        emit("LDA", RM, SP, 1, SP);
        
        codeGen_expr(root->child[1], current);//R0
        
        //load the left part
        emit("LD", RM, R1, -1, SP);//R1
        emit("LDA", RM, SP, -1, SP);//SP--
        
        if(root->op == PLUS){
            emit("ADD", RO, R0, R1, R0);
        }else if(root->op == SUB){
            emit("SUB", RO, R0, R1, R0);
        }else if(root->op == TIMES){
            emit("MUL", RO, R0, R1, R0);
        }else{
            emit("DIV", RO, R0, R1, R0);
        }

    }else if(root->op == ASSIGN){
        //left
        if(root->child[0]->isArray){
            codeGen_expr(root->child[0]->child[0], current);//R0 store idx'val
            
            int offset = getOffsetLocal(current, root->child[0]->name);
            if(offset == -1){
                offset = getOffsetParams(current->scopeName, root->child[0]->name);
                if(offset == 0){
                    offset = getOffSetGlobal(root->child[0]->name);
                    emit("LDA", RM, R1, -offset, GP);
                    emit("SUB", RO, R0, R1, R0);
                }else{
                    emit("LD", RM, R1, offset, FP);
                    emit("ADD", RO, R0, R0, R1);
                }
            }else{
                emit("LDA", RM, R1, offset, FP);//get INTARRAY addr
                emit("ADD", RO, R0, R0, R1);
            }
            
            emit("ST", RM, R0, 0, SP);
            emit("LDA", RM, SP, 1, SP);
        }else{
            
            int offset = getOffsetLocal(current, root->child[0]->name);
            if(offset == -1){
                offset = getOffsetParams(current->scopeName, root->child[0]->name);
                if(offset == 0){
                    offset = getOffSetGlobal(root->child[0]->name);
                    emit("LDA", RM, R0, -offset, GP);
                }else{
                    emit("LD", RM, R0, offset, FP);
                }
            }else{
                emit("LDA", RM, R0, offset, FP);//addr
            }
            
            emit("ST", RM, R0, 0, SP);
            emit("LDA", RM, SP, 1, SP);
        }

        //right
        codeGen_expr(root->child[1], current);//val in R0
        
        emit("LD", RM, R1, -1, SP);//R1: left addr
        emit("LDA", RM, SP, -1, SP);//SP--
        emit("ST", RM, R0, 0, R1);//assign: put val in R0 to dMem[val in R1];
    
    }else if(root->op == NUM){//number
        emit("LDC", RM, R0, root->val, R0);
        
    }else if(root->op == INPUT){//user input
        emit("IN", RO, R0, R0, R0);
    }
}

void codeGen_comparison(TreeNode* root, SymbolTable* current){
    if(root->op == NOT){
        codeGen_comparison(root->child[0], current);
    }else if(root->op == LESS || root->op == GREATER
             || root->op == LESSEQUAL || root->op == GREATEREQUAL
             || root->op == DOUBLEEQUAL || root->op == NOTEQUAL){
        
        codeGen_expr(root->child[0], current);
        emit("ST", RM, R0, 0, SP);
        emit("LDA", RM, SP, 1, SP);
        
        codeGen_expr(root->child[1], current);//R0
        
        emit("LD", RM, R1, -1, SP);//R1:left
        emit("LDA", RM, SP, -1, SP);//SP--
        
        emit("SUB", RO, R0, R1, R0);//R0 = R1 - R0
    }
}

void codeGen_statement(TreeNode* root, string curScopeName, SymbolTable* current){
    
    if(root->op == IF){
        codeGen_comparison(root->child[0], current);//R0
        int jumpElse = 0;
        if(root->child[1] != NULL){//if-statement
            int Inum = ICounter;
            ICounter++;//leave a gap
            codeGen_statement(root->child[1], curScopeName, current);
            
            jumpElse = ICounter;//idx
            int jumpEndI = ICounter;
            if(root->child[2] != NULL){
                jumpEndI++;
            }
            //jump w/o not
            if(root->child[0]->op == LESS){
                emitWithIdx("JGE", RM, R0, jumpEndI, ZERO, Inum);
            }else if(root->child[0]->op == GREATER){
                emitWithIdx("JLE", RM, R0, jumpEndI, ZERO, Inum);
            }else if(root->child[0]->op == LESSEQUAL){
                emitWithIdx("JGT", RM, R0, jumpEndI, ZERO, Inum);
            }else if(root->child[0]->op == GREATEREQUAL){
                emitWithIdx("JLT", RM, R0, jumpEndI, ZERO, Inum);
            }else if(root->child[0]->op == DOUBLEEQUAL){
                emitWithIdx("JNE", RM, R0, jumpEndI, ZERO, Inum);
            }else if(root->child[0]->op == NOTEQUAL){
                emitWithIdx("JEQ", RM, R0, jumpEndI, ZERO, Inum);
            }
        }
        
        if(root->child[2] != NULL){//else-statement
            ICounter++;
            codeGen_statement(root->child[2], curScopeName, current);
            //jump ElSE
            emitWithIdx("LDC", RM, PC, ICounter, R0, jumpElse);
        }else{
            emitWithIdx("LDC", RM, PC, ICounter, R0, jumpElse);
        }
        
    }else if(root->op == WHILE){
        int jumpTrue = ICounter;
        codeGen_comparison(root->child[0], current);//R0
        int jumpNot = ICounter;
        ICounter++;
        if(root->child[1] != NULL){
            codeGen_statement(root->child[1], curScopeName, current);
        }
        
        //jump true
        emit("LDC", RM, PC, jumpTrue, R0);
        
        //jump w/o not
        if(root->child[0]->op == LESS){
            emitWithIdx("JGE", RM, R0, ICounter, ZERO, jumpNot);
        }else if(root->child[0]->op == GREATER){
            emitWithIdx("JLE", RM, R0, ICounter, ZERO, jumpNot);
        }else if(root->child[0]->op == LESSEQUAL){
            emitWithIdx("JGT", RM, R0, ICounter, ZERO, jumpNot);
        }else if(root->child[0]->op == GREATEREQUAL){
            emitWithIdx("JLT", RM, R0, ICounter, ZERO, jumpNot);
        }else if(root->child[0]->op == DOUBLEEQUAL){
            emitWithIdx("JNE", RM, R0, ICounter, ZERO, jumpNot);
        }else if(root->child[0]->op == NOTEQUAL){
            emitWithIdx("JEQ", RM, R0, ICounter, ZERO, jumpNot);
        }
        
    }else if(root->op == RETURN){

        codeGen_expr(root->child[0], current);//R0
        
    }else if(root->op == DOLLAR){
        codeGen_expr(root->child[0], current);
        
        emit("OUT", RO, R0, R0, R0);

    }else if(root->op == BLOCK){
        
        if(current->child == NULL){
            current->child = codeGen_block(root, curScopeName, current);
        }else{
            SymbolTable* p = findPos(current);
            p->sibling = codeGen_block(root, curScopeName, current);
        }
        
    }else if(root->op == BREAK){
        //jump out while block
        //TODO:
        
        
    }else{
        codeGen_expr(root, current);
    }
}

SymbolTable* codeGen_block(TreeNode* root, string name, SymbolTable* parent){
    
    SymbolTable* st = new SymbolTable();
    st->scopeName = name;
    st->parent = parent;
    st->offset = parent->offset;
    
    //local var declaration
    TreeNode* p = root->child[0];
    while(p != NULL){
        nodeInfo* info = &st->table.find(p->name)->second;
        info->type = p->type;
        info->decType = LOCAL_V_DEC;
        info->offset = st->offset;
        
        if(p->isArray){
            info->isArray = true;
            info->arraySize = p->arraySize;
            st->offset += p->arraySize;
            emit("LDA", RM, SP, p->arraySize, SP);
        }else{
            emit("LDA", RM, SP, 1, SP);
            st->offset++;
        }
        st->table.insert(pair<string, nodeInfo>(p->name, *info));
        p = p->sibling;
    }
    
    //statements
    p = root->child[1];
    while(p != NULL){
        codeGen_statement(p, st->scopeName, st);
        p = p->sibling;
    }
    
    if(st->scopeName.compare("main") != 0){
    //roll back SP to SP - ParasNum
        int numOfParams = getNumOfParams(st->scopeName);
        emit("LDA", RM, SP, -numOfParams, FP);
        //return control to caller
        emit("LD", RM, PC, 0, FP);
    }
    return st;
}


void codeGen_params(TreeNode* head, nodeInfo* info){
    int offset = 0;
    int num = 0;
    params* h = NULL, *t = NULL;
    
    while(head != NULL){
        params* p = new params();
        num++;
        p->type = head->type;
        p->param_name = head->name;
        p->offset = offset;
        p->isArray = head->isArray;
        offset++;
        
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
    info->numOfParams = num;//gap between param with FP: offset - numOfParams
    
    params* first = info->paramList;
    while(first != NULL){
        first->offset = first->offset - info->numOfParams;
        first = first->sibling;
        
    }
}

void codeGen(TreeNode *root){
    int newVar = 1;
    SymbolTable* st = new SymbolTable();
    st->offset = 2;
    global = st;
    st->scopeName = "global";
    while(root != NULL){
        if(root -> op == V_DEC){//global var
            
            if(newVar == 1){
                emit("LDC", RM, GP, 1023, R0);
                newVar = 0;
            }

            nodeInfo* info = new nodeInfo();
            info->type = root->type;
            info->decType = V_DEC;
            info->offset = globalOffset;
            if(root->isArray){
                info->isArray = true;
                info->arraySize = root->arraySize;
                globalOffset += root->arraySize;
            }else{
                globalOffset++;
            }
            st->table.insert(pair<string, nodeInfo>(root->name, *info));
            
            emit("LDA", RM, GP, -(info->offset), GP);
            
        }else if(root->op == F_DEC){

            nodeInfo *info = new nodeInfo();
            info->type = root->type;
            info->decType = F_DEC;
            info->Ibegin = ICounter;
            
            if(root->name.compare("main") == 0){
                int mainStart = ICounter;

                emit("LDC", RM, FP, 0, R0);
                emit("LDC", RM, SP, 2, R0);
                emitWithIdx("LDC", RM, PC, mainStart, R0, 0);
                
                //store argument's info
                codeGen_params(root->child[0], info);
                st->table.insert(pair<string, nodeInfo>("main", *info));
                
                if(st->child == NULL){
                    st->child = codeGen_block(root->child[1], "main", st);
                }else{
                    SymbolTable* p = findPos(st);
                    p->sibling = codeGen_block(root->child[1], "main", st);
                }

            }else{

                //save FP of caller
                emit("ST", RM, FP, 0, SP);
                
                //set new FP
                emit("LDA", RM, FP, -1, SP);
                emit("LDA", RM, SP, 1, SP);
                
                //store argument info
                codeGen_params(root->child[0], info);
                st->table.insert(pair<string, nodeInfo>(root->name, *info));
                
                if(st->child == NULL){
                    st->child = codeGen_block(root->child[1], root->name, st);
                }else{
                    SymbolTable* p = findPos(st);
                    p->sibling = codeGen_block(root->child[1], root->name, st);
                }
            }
        }
        root = root->sibling;
    }
}

void printCode(void){
    for(int i = 0; i < ICounter; i++){
        cout << i << ": "
        << codeArray[i].opcode << " "
        << codeArray[i].rand1 << ","
        << codeArray[i].rand2
        << (codeArray[i].ctype == RO? "," : "(")
        << codeArray[i].rand3;
        if (codeArray[i].ctype == RM) cout << ")";
        cout << endl;
    }
}




