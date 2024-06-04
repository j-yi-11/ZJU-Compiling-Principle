#include "ast/ast.h"
#include "semantic/semantic.h"
#include <cstdio>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <fmt/core.h>
#include <fmt/color.h>

extern int yyparse();
extern FILE* yyin;
extern std::vector<std::string> ircode;
extern void generate_ircode(NodePtr root);
NodePtr root = nullptr;

int main(int argc, char **argv) {
    yyin = fopen(argv[1], "r");
    if(yyin == nullptr){
        fmt::print(fmt::emphasis::bold | fg(fmt::color::red), "source file {} not found\n",argv[1]);
        return 1;
    }
    fmt::print(fmt::emphasis::bold | fg(fmt::color::green), "parsing file: {} read success\n",argv[1]);
    int res = yyparse();
    if(res != 0){
        fmt::print(fmt::emphasis::bold | fg(fmt::color::red),"parse failed with res = {}\n", res);
        return 1;
    }
    fmt::print(fmt::emphasis::bold | fg(fmt::color::green), "lexer and parser success\n");
    bool printAST_flag = false;
    if(root == nullptr){
        fmt::print(fmt::emphasis::bold | fg(fmt::color::red), "AST root is nullptr\n");
    }else{
        //fmt::print("root is not nullptr\n");
        if(printAST_flag) {
            printAST(root, "", "");
            fmt::print(fmt::emphasis::bold | fg(fmt::color::green), "AST print success\n");
        }
    }
    // semantic anaylsis
    funcSymbolTablePtr funTablePtr = GenerateFuncSymTable(root);

    symbolTablePtr rootTablePtr = new SymbolTable();
    GenerateVarSymTable(root, rootTablePtr, funTablePtr,"");
    fmt::print(fmt::emphasis::bold | fg(fmt::color::green), "semantic analysis success\n");
    // generate ir code
    generate_ircode(root);
    // save ir code into file named "ircode.txt"
    std::ofstream irfile("ircode.txt");
    // clear all contents in original file ircode.txt
    irfile.clear();
    for(auto &line: ircode){
        irfile << line << "\n";
    }
    return 0;
}
