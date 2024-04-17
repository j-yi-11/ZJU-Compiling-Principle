#pragma once
#include <cstdint>
#include <type_traits>
#include <string>
#include <vector>
#include <map>

/*
Part 1
变量在使用时未经定义
函数在调用时未经定义。
变量出现重复定义
函数出现重复定义（即同样的函数名出现了不止一次定义）。

struct funcSymbolType {
    enum Type{
        INT, VOID
    } returnType;
    std::vector<varSymbolType> arglist;
    funcSymbolType(){}
};

struct varSymbolType {
    // only int
    bool isArray = false;
    std::vector<int> dimension;
    varSymbolType(){}
};
struct SymbolTable {
    std::vector<std::map<std::string, funcSymbolType>> funcTable;
    std::vector<std::map<std::string, varSymbolType>> varTable;
};


Part 2
赋值号两边的表达式类型不匹配。
    a = f(); VOID

赋值号左边出现一个只有右值的表达式。
    int h(); h = 1;
    int a[2][2]; a = 1;
    int a[2][2]; a[2] = 1; / a[2][2][2] = 1;
    int a[1]; a[100]=1;  ->  运行时错误 ignore

操作数类型不匹配或操作数类型与操作符不匹配（例如整型变量与数组变
量相加减，或数组变量与数组变量相加减）
    int a = 5 * 7 -4 / f(); f is void
    int a = b - 1; int b[7];
    int a = b - c; b[7],c[7]
    int a = b[9][6] - c[5]; b[7],c[7]

return语句的返回类型与函数定义的返回类型不匹配


函数调用时实参与形参的数目或类型不匹配

    int f(int a[][4][7])  // ok --> a.dimension = {-1, 4, 7}
    int b[9][4][7], c[1][4][7];
    f(b) // ok  f(c) // ok
    int b[9][9];
    f(b) f(b[6])// wrong
对非数组型变量使用“[…]”（数组访问）操作符


对普通变量使用“(…)”或“()”（函数调用）操作符


*/

struct funcSymbol {
    enum Type{
        INT, VOID
    } returnType;
    std::vector<varSymbolType> arglist;
    std::string name = "";
    funcSymbol(){}
};
struct funcSymbolTable {
    std::vector<funcSymbolPtr> funcTable;
    funcSymbolTable(){}
};
using funcSymbolPtr = struct funcSymbol*;
using funcSymbolTablePtr = struct funcSymbolTable*;
using varSymbolTypePtr = struct varSymbolType*;

struct varSymbolType {
    // only int
    bool isArray = false;
    std::vector<int> dimension;
    varSymbolType(){}
};
struct SymbolTable {
    struct SymbolTable * highLevelTable = nullptr;
    //std::vector<std::map<std::string, funcSymbolType>> funcTable;
    std::vector<std::map<std::string, varSymbolType>> varTable;
    SymbolTable(){}
};


funcSymbolTablePtr GenerateFuncSymTable(NodePtr root);