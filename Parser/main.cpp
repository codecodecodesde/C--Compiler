//
//  main.cpp
//  Parser
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
