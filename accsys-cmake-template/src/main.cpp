#include "ast/ast.h"
#include "semantic/semantic.h"

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
    int res = yyparse();
    if(res != 0){
        fmt::print("parse failed\n");
        fmt::print("res: {}\n", res);
        return 1;
    }
    if(root == nullptr){
        fmt::print("root is nullptr\n");
    }else{
        fmt::print("root is not nullptr\n");
        print_expr((root));
    }
    // semantic anaylsis
    funcSymbolTablePtr funTablePtr = GenerateFuncSymTable(root);
    return 0;
}
