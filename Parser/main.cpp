//
//  main.cpp
//  Parser
//
//  Created by 王璇 on 2017/11/12.
//  Copyright © 2017年 Xuan Wang. All rights reserved.
//

#include <stdio.h>
#include "globals.h"
TreeNode * declarations();
void printAST(TreeNode * p, int indent);


int main(int argc, char* argv[]){
    TreeNode * program = declarations();
    if(program == NULL){
        printf("no top-level expression.\n");
    }else{
        printAST(program, 0);
    }
    return 0;
}
