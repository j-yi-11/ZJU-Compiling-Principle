//
// Created by jiangyi on 2024/5/30.
//

#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "ast/ast.h"
std::vector<std::string> ircode;
void generate_ircode(NodePtr root);
void translate_BlockItem(NodePtr node);
void translate_Block(NodePtr node);
void translate_VarDecl(NodePtr node, bool isGlobalVar);
void translate_VarDef(NodePtr node, bool isGlobalVar);
void translate_Stmt(NodePtr node);
void translate_FuncDef(NodePtr node);
void translate_FuncFParam(NodePtr node);
void translate_CompUnit(NodePtr node);
std::string translate_exp(NodePtr node);
std::string translate_LVal(NodePtr node);