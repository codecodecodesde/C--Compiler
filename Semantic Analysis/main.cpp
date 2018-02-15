//
//  main.cpp
//  Semantic Analysis
//
//  Created by 王璇 on 2017/11/14.
//  Copyright © 2017年 Xuan Wang. All rights reserved.
//

#include <iostream>
#include "globals.h"
#include <iterator>
#include <iomanip>

TreeNode * declarations();
SymbolTable* analyze(TreeNode * p);
void printST(SymbolTable *st);

int main(int argc, const char * argv[]) {
    TreeNode * program = declarations();
    if (program == NULL) {
        cerr << "no top-level expression\n";
        exit(1);
    }
    SymbolTable* st = analyze(program);
    printST(st);
}
