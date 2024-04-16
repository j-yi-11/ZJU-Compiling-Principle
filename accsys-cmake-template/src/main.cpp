#include "ast/ast.h"

#include <fmt/core.h>

extern int yyparse();
extern FILE* yyin;
NodePtr root;

int main(int argc, char **argv) {
    yyin = fopen(argv[1], "r");
    int res = yyparse();
    print_expr((root));
    // fmt::print("Hello, World!\n");
    return 0;
}
