#include "ast/ast.h"

#include <fmt/core.h>

extern int yyparse();
extern FILE* yyin;
NodePtr root;

int main(int argc, char **argv) {
    yyin = fopen(argv[1], "r");
    yyparse();
    print_expr(static_cast<ExprPtr>(root));
    fmt::print("Hello, World!\n");
    return 0;
}
