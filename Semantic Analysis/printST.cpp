//
//  printST.cpp
//  Semantic Analysis
//
//  Created by 王璇 on 2017/11/25.
//  Copyright © 2017年 Xuan Wang. All rights reserved.
//
#include <iostream>
#include "globals.h"
#include <iterator>
#include <iomanip>

string convertType(int type){
    if(type == INT){
        return "int";
    }else{
        return "void";
    }
}

void printST(SymbolTable* st){
    if(st == NULL){
        return;
    }
    if(st->table.size() == 0){
        printST(st->child);
        printST(st->sibling);
        return;
    }
    cout << string(7, ' ') << st->scopeName << endl;
    cout << left << setw(10) << "name" << "semantic info" << endl;
    map<string, nodeInfo>::iterator pos;
    for(pos = st->table.begin(); pos != st->table.end(); pos++){
        cout << left << setw(10) << pos->first;
        nodeInfo info = pos->second;
        if(info.decType == F_DEC){
            cout << convertType(pos->second.type);
            cout << " " << pos->first;
            cout << "(";
            while(info.paramList != NULL){
                cout << convertType(info.paramList->type);
                if(info.paramList->param_name.compare("")){
                    cout << " " << info.paramList->param_name;
                }
                if(info.paramList->isArray){
                    cout << "[]";
                }
                info.paramList = info.paramList->sibling;
                if(info.paramList != NULL){
                    cout << ", ";
                }
            }
            cout << ")" << endl;
        }else if(info.decType == V_DEC || info.decType == LOCAL_V_DEC){
            cout << convertType(pos->second.type) <<" "<< pos->first;
            if(info.isArray){
                cout << "[" << info.arraySize << "]";
            }
            cout << endl;
        }
    }
    
    printST(st->child);
    printST(st->sibling);
    
}
