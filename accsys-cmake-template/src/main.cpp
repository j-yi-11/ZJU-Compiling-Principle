#include "ast/ast.h"
#include "semantic/semantic.h"
#include <cstdio>
#include <fmt/core.h>

extern int yyparse();
extern FILE* yyin;
NodePtr root = nullptr;

int main(int argc, char **argv) {
    yyin = fopen(argv[1], "r");
    if(yyin == nullptr){
        fmt::print("file {} not found\n",argv[1]);
        return 1;
    }
    fmt::print("parsing file: {}\n",argv[1]);
    int res = yyparse();
    if(res != 0){
        fmt::print("parse failed with res = {}\n", res);
        return 1;
    }
    fmt::print("parse success\n");
    bool printAST_flag = true;
    if(root == nullptr){
        fmt::print("root is nullptr\n");
    }else{
        //fmt::print("root is not nullptr\n");
        if(printAST_flag)
            printAST(root,"","");
    }
    // semantic anaylsis
//    funcSymbolTablePtr funTablePtr = GenerateFuncSymTable(root);
//
//    symbolTablePtr rootTablePtr = new SymbolTable();
//    GenerateVarSymTable(root, rootTablePtr, funTablePtr,"");
    return 0;
}
