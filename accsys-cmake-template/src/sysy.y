%{
#include <stdio.h>
#include <ast/ast.h>
#include <string>
#include <vector>
void yyerror(const char *s);
extern int yylex(void);
extern NodePtr root;
%}

/// types
%union {
    int iVal;
    std::string *sVal;
    NodePtr nodeVal;
    std::vector<NodePtr>* vecVal;
    std::vector<int>* iVecVal;
    OpType opVal;
}

%token <iVal> CONSTINT
%token <sVal> IDENTIFIER
%token <opVal> AND OR NOT
%token <opVal> NEQ EQ GEQ LEQ GREAT LESS
%type <nodeVal> FuncDef PrimitiveType Block Stmt StmtElse Exp UnaryExp PrimaryExp Number AddExp MulExp
%type <nodeVal> LOrExp LAndExp EqExp RelExp
%type <nodeVal> Decl BlockItem LVal
%type <nodeVal> VarDecl InitVal VarDef
%type <nodeVal> FuncFParam
%type <vecVal> BlockItems VarDefs
%type <vecVal> FuncFParams DFuncFParams FuncRParams DExps CompUnits
%type <nodeVal> Matched Unmatched
%type <iVecVal> Dimensions Dimensions_funcdef Dimensions_lval


%token  ADD SUB MUL DIV MOD
        ASSIGN ADDEQ SUBEQ MULEQ DIVEQ MODEQ
        IF ELSE FOR WHILE SWITCH CASE DEFAULT CONTINUE BREAK
        RETURN INT VOID
        COMMA SEMI LPAREN RPAREN LBRACKET RBRACKET LBRACE RBRACE

%start CompUnit
%left COMMA //逗号
%left FUNC_CALL_ARG_LIST
%right ASSIGN ADDEQ SUBEQ MULEQ DIVEQ MODEQ
%left OR
%left AND
%left EQ NEQ
%left GEQ GREAT LEQ LESS
%left ADD SUB
%left MUL DIV MOD
%right NOT
%left UMINUS UPLUS


// JY write
%%
CompUnit : 
        CompUnits {  
                auto comp_unit = new CompUnit();       
                for(auto a : *$1) 
                        comp_unit->all.emplace_back(a);
                root = comp_unit;
        };
CompUnits:
        | CompUnits Decl { 
                $1->emplace_back($2);
                $$ = $1; 
        }
        | CompUnits FuncDef { 
                $1->emplace_back($2);
                $$ = $1;  
        }
        | Decl {
                auto decl_units = new std::vector<NodePtr>;
                decl_units->emplace_back($1);
                $$ = decl_units;
        }
        | FuncDef {
                auto funcdef_units = new std::vector<NodePtr>;
                funcdef_units->emplace_back($1);
                $$ = funcdef_units;
        }
        ;
Decl : VarDecl {
                auto decl_unit = new Decl();
                decl_unit->VarDecl = $1;
                $$ = decl_unit;
        }
        ;
// int a;
// int f = 8
// int a, b=5
// int a, b=1,c=3,d=9;
VarDecl : INT VarDef VarDefs SEMI { 
                auto vardecl_unit = new VarDecl();
                vardecl_unit->VarDefs.emplace_back($2);
                if($3 == nullptr)
                        $$ = vardecl_unit;
                else{
                        for(auto i : *$3 )
                                vardecl_unit->VarDefs.emplace_back(i);                       
                        $$ = vardecl_unit;
                } 
        };      
VarDefs : { $$ = nullptr; }
        | VarDefs COMMA VarDef { 
                if($1 == nullptr){
                        auto vardef_vector = new std::vector<NodePtr>;
                        vardef_vector->emplace_back($3);
                        $$ = vardef_vector;
                }else{
                        $1->emplace_back($3);
                        $$ = $1;
                }
        };
// a=5
// a
// a[3]
// a[3][4][5]
VarDef : IDENTIFIER ASSIGN InitVal {
                auto vardef_unit = new VarDef();
                vardef_unit->name = *$1;
                vardef_unit->initialValue = $3;
                vardef_unit->isArray = false;
                $$ = vardef_unit;
        }
        | IDENTIFIER {
                auto vardef_unit = new VarDef();
                vardef_unit->name = *$1;
                vardef_unit->isArray = false;
                $$ = vardef_unit;
        }
        | IDENTIFIER Dimensions {
                auto vardef_unit = new VarDef();
                vardef_unit->name = *$1;
                vardef_unit->isArray = true;
                vardef_unit->dimensions = *$2;
                $$ = vardef_unit;
        };
// only for [3][4]...
Dimensions : LBRACKET CONSTINT RBRACKET {
                $$ = new std::vector<int>;
                $$->emplace_back($2);
        }
        | LBRACKET CONSTINT RBRACKET Dimensions {
                $4->insert($4->begin(), $2);
                $$ = $4;
        };
// // [3]
// // [3][4][5]
// VarDefTail : LBRACKET CONSTINT RBRACKET {       $$ = new std::vector<int>;
//                                                 $$->push_back($2);  // $2 表示匹配的 CONSTINT 值
//                                         }
//         | LBRACKET CONSTINT RBRACKET VarDefTail {
//                                                 $$ = $4;  // $4 表示 VarDefTail 的返回值
//                                                 $$->push_back($2);
//                                         }
//         ;
InitVal : Exp {
                auto val = new InitVal();
                val->Exp = $1;
                $$ = val;
        };

// func def   
// int f(int a, int b[]){...}
FuncDef : FuncType IDENTIFIER LPAREN FuncFParams RPAREN Block {
    auto funcdef_unit = new FuncDef();
    //judge returnType
    if($1 == VOID) funcdef_unit->ReturnType = 0;
    if($1 == INT) funcdef_unit->ReturnType = 1;
    funcdef_unit->name = *($2);
    if($4 != nullptr){
        for(auto i : *$4)
            funcdef_unit->argList.emplace_back(i);
    }
    else{  }
    funcdef_unit->block = $6;
    $$ = funcdef_unit;
  }
  ;
// return type
FuncType : VOID | INT ;
FuncFParams
  : DFuncFParams {
    $$ = $1;
  }
  | { $$ = nullptr; }
  ;
DFuncFParams
  : DFuncFParams ',' FuncFParam {
    $1->emplace_back($3);
    $$ = $1;
  }
  | FuncFParam {
    auto vec = new std::vector<NodePtr>;
    vec->emplace_back($1);
    $$ = vec;
  }
  ;
FuncFParam
  : INT IDENTIFIER {
    auto para = new FuncFParam();
    para->name = *($2);
    $$ = para;
  }
  | INT IDENTIFIER '[' ']' Dimensions_funcdef {
    auto para = new FuncFParam();
    para->name = *($2);
    para->isArray = true;
    para->dimensions = *$3;
    $$ = para;    
  };
Dimensions_funcdef 
  : LBRAKET RBRACKET {
     auto vec = new std::vector<int>;
     vec->emplace_back(-1);// [][2] ==> -1, 2
     $$ = vec;
  }
  | Dimensions_funcdef '[' CONSTINT ']' { 
      auto dims = $1;
      dims->emplace_back($3);
      $$ = dims; 
    }
  ;
// FuncFParams : FuncFParam FuncFParamsTail { 
//         ; }
//         ;
// FuncFParamsTail : /*empty*/
//         | COMMA FuncFParam FuncFParamsTail
//         ;
// int a
// int a[]
// int a[4]
// int a[][3][4]
// FuncFParam : BType IDENTIFIER FuncFParamTail { printf("FuncFParam -> BType IDENTIFIER FuncFParamTail\n"); }
//         ;
// FuncFParamTail : /*empty*/
//         |  LBRACKET RBRACKET
//         |  FuncFParamTail LBRACKET CONSTINT RBRACKET
//         ;

// {......}
Block: LBRACE BlockItems RBRACE 
  : {
    auto block = new Block();
    if($2 == nullptr)// 可能是空块
        $$ = block;
    else{
        for(auto i: *$2){
            block->BlockItems.emplace_back(i);
        }
        $$ = block;
    }
  }
  ;
BlockItems
  : BlockItems BlockItem {
    if($1 == nullptr){
        auto vec = new std::vector<NodePtr>;
        vec->emplace_back($2);
        $$ = vec;
    }
    else{
        $1->emplace_back($2);
        $$ = $1;
    }
  }
  | { $$ = nullptr; }
  ;

BlockItem: Decl {     
        auto item = new BlockItem();
        item->Decl = ($1);
        $$ = item; 
    }
	| Stmt {
        auto item = new BlockItem();
        item->Stmt = ($1);
        $$ = item;  
    };

Stmt: 
		LVal ASSIGN Exp SEMI {
		    auto stmt = new AssignStmt();
		    stmt->LVal = ($1);
		    stmt->Exp = ($3);
		    $$ = stmt;
		}
		| Exp SEMI { // include " ;"
		    auto stmt = new ExpStmt();
		    stmt->Exp = ($1);
		    $$ = stmt;
		 }
		| Block {
		    auto stmt = new BlockStmt();
		    stmt->Block = ($1);
		    $$ = stmt;
		 }
		| IF LPAREN Exp RPAREN Stmt StmtElse {
		    // TODO
		    auto stmt = new IfStmt();
		    stmt->condition = ($3);
		    stmt->then = ($5);
		    stmt->els = ($6);
		    $$ = stmt;
		 }
		| WHILE LPAREN Exp RPAREN Stmt {
		    auto stmt = new WhileStmt();
		    stmt->condition = ($3);
		    stmt->then = ($5);
		    $$ = stmt;
		 }
		| BREAK SEMI {
		    auto stmt = new BreakStmt();
		    $$ = stmt;
		}
		| CONTINUE SEMI {
		    auto stmt = new ContinueStmt();
		    $$ = stmt;
		 }
		| RETURN Exp SEMI {
		    auto stmt = new ReturnStmt();
		    stmt->result = $2;
		    $$ = stmt;
		 }
		| RETURN SEMI {
		    auto stmt = new ReturnStmt();
		    $$ = stmt;
		}
		;
StmtElse : { $$ = nullptr; }
    | ELSE Stmt {
            $$ = $2;
        }
    ;
Exp : LOrExp { 
            auto exp = new Exp();
            exp->LgExp = ($1);
            $$ = exp;
        };

LVal :
	IDENTIFIER {
					auto l_val = new LVal();
					l_val->name = *$1;
					$$ = l_val;
        }
		| IDENTIFIER Dimensions_lval {
			auto l_val = new LVal();
			l_val->name = *$1;
			l_val->isArray = true;
			l_val->position = *$2;
			$$ = l_val;
		}
		;
Dimensions_lval : LBRACKET CONSTINT RBRACKET {
	$$ = new std::vector<int>;
	$$->emplace_back($2); 
}
| LBRACKET CONSTINT RBRACKET Dimensions_lval {
    $4->emplace_back($2);
    $$ = $4;
};
PrimaryExp: LPAREN Exp RPAREN { 
      auto primary = new PrimaryExpr();
			primary->Exp = ($2);
			$$ = primary;
		}
		| LVal { 
			auto primary = new PrimaryExpr();
			primary->LVal = ($1);
			$$ = primary; 
		}
		| Number { 
			auto primary = new PrimaryExpr();
			primary->Number = ($1);
			$$ = primary;	
		};

Number: CONSTINT { 
		auto number = new IntegerLiteral($1);
		$$ = number;
  };

UnaryExp: PrimaryExp { 
		auto unary = new UnaryExpr();
    unary->primaryExp = ($1);
    $$ = unary;
	}
	| IDENTIFIER LPAREN FuncRParams RPAREN { 
		auto unary = new UnaryExpr();
		unary->name = *$1;
		if($3 == nullptr)
			$$ = unary;
		else{
			for(auto i: *$3)
            unary->params.emplace_back(i);
        $$ = unary;
		}
	}
	| UMINUS UnaryExp { 
		auto unary = new UnaryExpr();
    unary->primaryExp = ($1);
		unary->optype = OpType::UMINUS;//todo
    $$ = unary;
	}
	| UPLUS UnaryExp { 
		auto unary = new UnaryExpr();
    unary->primaryExp = ($1);
		unary->optype = OpType::UPLUS;//todo
    $$ = unary;
	}
	| NOT UnaryExp { 
		auto unary = new UnaryExpr();
    unary->primaryExp = ($1);
		unary->optype = OpType::NOT;//todo
    $$ = unary;
	};

// UnaryOp: '+' { printf("UnaryOp -> '+'\n"); }
//         | '-' { printf("UnaryOp -> '-'\n"); }
//         | '!' { printf("UnaryOp -> '!'\n"); }
//         ;

FuncRParams: Exp { 
		auto param = new std::vector<NodePtr>;
    param->emplace_back($1);
    $$ = param;
  }
	| DExps COMMA Exp { // keeping the parameter order
    $1->emplace_back($3);
    $$ = $1;
  }
  | { $$ = nullptr; }
  ;
DExps
  : DExps COMMA Exp {
    $1->emplace_back($3);
    $$ = $1;
  }
  | Exp {
    auto param = new std::vector<NodePtr>;
    param->emplace_back($1);
    $$ = param;
  };

MulExp: UnaryExp { 
		auto mul_exp = new MulExp();
    mul_exp->UnaryExp = ($1);
    $$ = mul_exp;
 }
	| MulExp MUL UnaryExp { 
		auto mul_exp = new MulExp();
    mul_exp->mulExp = ($1);
    mul_exp->optype = OpType::MUL;// todo
    mul_exp->unaryExp = ($3);
    $$ = mul_exp;
	}
	| MulExp DIV UnaryExp { 
		auto mul_exp = new MulExp();
    mul_exp->mulExp = ($1);
    mul_exp->optype = OpType::DIV;// todo
    mul_exp->unaryExp = ($3);
    $$ = mul_exp;
	}
	| MulExp MOD UnaryExp { 
		auto mul_exp = new MulExp();
    mul_exp->mulExp = ($1);
    mul_exp->optype = OpType::MOD;// todo
    mul_exp->unaryExp = ($3);
    $$ = mul_exp;
	};

AddExp: MulExp { 
		auto add_exp = new AddExp();
    add_exp->mulExp = ($1);
    $$ = add_exp;
 }
	| AddExp ADD MulExp { 
		auto add_exp = new AddExp();
    add_exp->addExp = ($1);
    add_exp->type = OpType::ADD;// todo
    add_exp->mulExp = ($3);
    $$ = add_exp;
	}
	| AddExp SUB MulExp { 
		auto add_exp = new AddExp();
    add_exp->addExp = ($1);
    add_exp->type = OpType::SUB;// todo
    add_exp->mulExp = ($3);
    $$ = add_exp;
	};

RelExp: AddExp { 
		auto rel_exp = new CompExp();
    rel_exp->lhs = ($1);
    $$ = rel_exp;
 }
	| RelExp LESS AddExp { 
		auto rel_exp = new CompExp();
    rel_exp->lhs = ($1);
		rel_exp->optype = OpType::LESS;// todo
		rel_exp->rhs = $3;
    $$ = rel_exp;	
	}
	| RelExp GREAT AddExp { 
		auto rel_exp = new CompExp();
    rel_exp->lhs = ($1);
		rel_exp->optype = OpType::GREAT;// todo
		rel_exp->rhs = $3;
    $$ = rel_exp;	
	}
	| RelExp LEQ AddExp { 
		auto rel_exp = new CompExp();
    rel_exp->lhs = ($1);
		rel_exp->optype = OpType::LEQ;// todo
		rel_exp->rhs = $3;
    $$ = rel_exp;	
	}
	| RelExp GEQ AddExp { 
		auto rel_exp = new CompExp();
    rel_exp->lhs = ($1);
		rel_exp->optype = OpType::GEQ;// todo
		rel_exp->rhs = $3;
    $$ = rel_exp;	
	};

EqExp: RelExp { 
		auto comp_exp = new CompExp();
    comp_exp->lhs = ($1);
    $$ = comp_exp;	
 }
	| EqExp EQ RelExp { 
		auto comp_exp = new CompExp();
    comp_exp->lhs = ($1);
		comp_exp->optype = OpType::EQ;// todo
		comp_exp->rhs = $3;
    $$ = comp_exp;	
	}
	| EqExp NEQ RelExp { 
		auto comp_exp = new CompExp();
    comp_exp->lhs = ($1);
		comp_exp->optype = OpType::NEQ;// todo
		comp_exp->rhs = $3;
    $$ = comp_exp;	
	};

LAndExp: EqExp { 
		auto lg_exp = new LgExp();
    lg_exp->lhs = ($1);
    $$ = lg_exp;
 }
	| LAndExp AND EqExp { 
		auto lg_exp = new LgExp();
    lg_exp->lhs = ($1);
    lg_exp->optype = OpType::AND;// todo
    lg_exp->rhs = ($3);
    $$ = lg_exp;
	 };

LOrExp: LAndExp { 
		auto lg_exp = new LgExp();
    lg_exp->lhs = ($1);
    $$ = lg_exp;
}
  | LOrExp OR LAndExp { 
		auto lg_exp = new LgExp();
    lg_exp->lhs = ($1);
    lg_exp->optype = OpType::OR;// todo
    lg_exp->rhs = ($3);
    $$ = lg_exp;
	};




%%

void yyerror(const char *s) {
    printf("error: %s\n", s);
}












// %parse-param { std::unique_ptr<BaseAST> &ast }

// %union {
//   std::string *str_val;
//   int int_val;
//   BaseAST *ast_val;
//   std::vector<BaseAST*>* vec_val;
// }

// %token INT RETURN CONST VOID IF ELSE WHILE CONTINUE BREAK
// %token <str_val> IDENT
// %token <int_val> UNARY_OP
// %token <int_val> BINARY_OP
// %token <int_val> LOGIC_OP_OR
// %token <int_val> LOGIC_OP_AND
// %token <int_val> COMP_OP_EQ
// %token <int_val> COMP_OP_N

// %token <int_val> INT_CONST

// %type <ast_val> FuncDef PrimitiveType Block Stmt Exp UnaryExp PrimaryExp Number AddExp MulExp
// %type <ast_val> LOrExp LAndExp EqExp RelExp

// %type <ast_val> Decl ConstDecl ConstDef ConstInitVal BlockItem LVal ConstExp
// %type <ast_val> VarDecl InitVal VarDef

// %type <ast_val> FuncFParam

// %type <vec_val> BlockItems ConstDefs VarDefs ConstExps
// %type <vec_val> FuncFParams DFuncFParams FuncRParams DExps CompUnits
// %type <ast_val> Matched Unmatched
// %%

// CompUnit
//   : CompUnits {
//     auto comp_unit = make_unique<CompUnitAST>();
//     if($1 == nullptr){
//         cout << "Empty File?" << endl;
//         exit(0);
//     }
//     for(auto i: *$1)
//         comp_unit->globe.push_back(unique_ptr<BaseAST>(i));
//     delete $1;
//     ast = std::move(comp_unit);
//   }
//   ;
// CompUnits
//   : CompUnits FuncDef {
//     $1->push_back($2);
//     $$ = $1;
//   }
//   | CompUnits Decl {
//     $1->push_back($2);
//     $$ = $1;
//   }
//   | Decl {
//       auto units = new vector<BaseAST*>;
//       units->push_back($1);
//       $$ = units;
//   }
//   | FuncDef {
//     auto units = new vector<BaseAST*>;
//     units->push_back($1);
//     $$ = units;
//   }
//   ;
// FuncDef
//   : PrimitiveType IDENT '(' FuncFParams ')' Block {
//     auto ast = new FuncDefAST();
//     ast->func_type = unique_ptr<BaseAST>($1);
//     ast->ident = *unique_ptr<string>($2);
//     if($4 != nullptr){
//         for(auto i : *$4)
//             ast->params.push_back(unique_ptr<BaseAST>(i));
//         delete $4;
//         ast->block = unique_ptr<BaseAST>($6);
//         $$ = ast;
//     }
//     else{
//         ast->block = unique_ptr<BaseAST>($6);
//         $$ = ast;
//     }
//   }
//   ;
// FuncFParams
//   : DFuncFParams {
//     $$ = $1;
//   }
//   | { $$ = nullptr; }
// DFuncFParams
//   : DFuncFParams ',' FuncFParam {
//     $1->push_back($3);
//     $$ = $1;
//   }
//   | FuncFParam {
//     auto vec = new vector<BaseAST*>;
//     vec->push_back($1);
//     $$ = vec;
//   }
//   ;
// FuncFParam
//   : PrimitiveType IDENT {
//     auto para = new FuncFParamAST();
//     para->PrimitiveType = unique_ptr<BaseAST>($1);
//     para->ident = *unique_ptr<string>($2);
//     $$ = para;
//   };
// PrimitiveType
//   : INT {
//     auto ast = new PrimitiveTypeAST();
//     ast->type = string("int");
//     $$ = ast;
//   }
//   | VOID {
//     auto ast = new PrimitiveTypeAST();
//     ast->type = string("void");
//     $$ = ast;
//   }
//   ;
// Stmt
//   : Matched {
//     dynamic_cast<StmtAST*>($1)->matched = true;
//     $$ = $1;
//   }
//   | Unmatched {
//     dynamic_cast<StmtAST*>($1)->matched = false;
//     $$ = $1;
//   }
//   ;
// Matched
//   : RETURN ';' {
//     auto stmt = new StmtAST();
//     stmt->type = StmtAST::RET;
//     $$ = stmt;
//   }
//   | RETURN Exp ';' {
//     auto stmt = new StmtAST();
//     stmt->type = StmtAST::RET;
//     stmt->Exp = unique_ptr<BaseAST>($2);
//     $$ = stmt;
//   }
//   | LVal '=' Exp ';' {
//     auto stmt = new StmtAST();
//     stmt->type = StmtAST::ASSIGN;
//     stmt->LVal = unique_ptr<BaseAST>($1);
//     stmt->Exp = unique_ptr<BaseAST>($3);
//     $$ = stmt;
//   }
//   | Block {
//     auto stmt = new StmtAST();
//     stmt->type = StmtAST::BLOCK;
//     stmt->Block = unique_ptr<BaseAST>($1);
//     $$ = stmt;
//   }
//   | Exp ';' {
//     auto stmt = new StmtAST();
//     stmt->type = StmtAST::EXP;
//     stmt->Exp = unique_ptr<BaseAST>($1);
//     $$ = stmt;
//   }
//   | ';' {
//     auto stmt = new StmtAST();
//     stmt->type = StmtAST::EXP;
//     $$ = stmt;
//   }
//   | IF '(' Exp ')' Matched ELSE Matched {
//     dynamic_cast<StmtAST*>($5)->matched = true;
//     dynamic_cast<StmtAST*>($7)->matched = true;

//     auto stmt = new StmtAST();
//     stmt->type = StmtAST::IF;
//     stmt->Exp = unique_ptr<BaseAST>($3);
//     stmt->if_stmt = unique_ptr<BaseAST>($5);
//     stmt->else_stmt = unique_ptr<BaseAST>($7);
//     $$ = stmt;
//   }
//   | BREAK ';' {
//     auto stmt = new StmtAST();
//     stmt->type = StmtAST::BREAK;
//     $$ = stmt;
//   }
//   | CONTINUE ';' {
//     auto stmt = new StmtAST();
//     stmt->type = StmtAST::CONTINUE;
//     $$ = stmt;
//   }
//   | WHILE '(' Exp ')' Stmt {
//     auto stmt = new StmtAST();
//     stmt->type = StmtAST::WHILE;
//     stmt->Exp = unique_ptr<BaseAST>($3);
//     stmt->if_stmt = unique_ptr<BaseAST>($5);
//     $$ = stmt;
//   }
//   ;
// Unmatched
//   : IF '(' Exp ')' Stmt {
//     auto stmt = new StmtAST();
//     stmt->type = StmtAST::IF;
//     stmt->Exp = unique_ptr<BaseAST>($3);
//     stmt->if_stmt = unique_ptr<BaseAST>($5);
//     $$ = stmt;
//   }
//   | IF '(' Exp ')' Matched ELSE Unmatched {
//     dynamic_cast<StmtAST*>($5)->matched = true;
//     dynamic_cast<StmtAST*>($7)->matched = false;

//     auto stmt = new StmtAST();
//     stmt->type = StmtAST::IF;
//     stmt->Exp = unique_ptr<BaseAST>($3);
//     stmt->if_stmt = unique_ptr<BaseAST>($5);
//     stmt->else_stmt = unique_ptr<BaseAST>($7);
//     $$ = stmt;
//   };
// Exp
//   : LOrExp {
//     auto exp = new ExpAST();
//     exp->LgExp = unique_ptr<BaseAST>($1);
//     $$ = exp;
//   }
//   ;
// UnaryExp
//   : PrimaryExp {
//     auto unary = new UnaryExpAST();
//     unary->PrimaryExp = unique_ptr<BaseAST>($1);
//     $$ = unary;
//   }
//   | UNARY_OP UnaryExp {
//     auto unary = new UnaryExpAST();
//     unary->UnaryExp = unique_ptr<BaseAST>($2);
//     unary->OpType = ( $1 == '+' ? PLUS : $1 == '-' ? MINUS : COMPLEMENT );
//     $$ = unary;
//   }
//   | IDENT '(' FuncRParams ')' {
//     auto unary = new UnaryExpAST();
//     unary->ident = *unique_ptr<string>($1);
//     if($3 == nullptr)
//         $$ = unary;
//     else{
//         for(auto i: *$3)
//             unary->params.push_back(unique_ptr<BaseAST>(i));
//         delete $3;
//         $$ = unary;
//     }
//   }
//   ;
// FuncRParams
//   : Exp {
//     auto param = new vector<BaseAST*>;
//     param->push_back($1);
//     $$ = param;
//   }
//   | DExps ',' Exp { // keeping the parameter order is important
//     $1->push_back($3);
//     $$ = $1;
//   }
//   | { $$ = nullptr; }
//   ;
// DExps
//   : DExps ',' Exp {
//     $1->push_back($3);
//     $$ = $1;
//   }
//   | Exp {
//     auto param = new vector<BaseAST*>;
//     param->push_back($1);
//     $$ = param;
//   };

// MulExp
//   : UnaryExp {
//     auto mul_exp = new MulExpAST();
//     mul_exp->UnaryExp = unique_ptr<BaseAST>($1);
//     $$ = mul_exp;
//   }
//   | MulExp BINARY_OP UnaryExp {
//     auto mul_exp = new MulExpAST();
//     mul_exp->MulExp = unique_ptr<BaseAST>($1);
//     mul_exp->type = $2 == '*' ? MUL : $2 == '/' ? DIV : MOD;
//     mul_exp->UnaryExp = unique_ptr<BaseAST>($3);
//     $$ = mul_exp;
//   }
//   ;
// AddExp
//   : MulExp {
//     auto add_exp = new AddExpAST();
//     add_exp->MulExp = unique_ptr<BaseAST>($1);
//     $$ = add_exp;
//   }
//   | AddExp UNARY_OP MulExp {
//     auto add_exp = new AddExpAST();
//     add_exp->AddExp = unique_ptr<BaseAST>($1);
//     add_exp->type = $2 == '+' ? PLUS : $2 == '-' ? MINUS : COMPLEMENT ;
//     add_exp->MulExp = unique_ptr<BaseAST>($3);
//     $$ = add_exp;
//   }
//   ;
// LOrExp
//   : LAndExp {
//     auto lg_exp = new LgExpAST();
//     lg_exp->LHS = unique_ptr<BaseAST>($1);
//     $$ = lg_exp;
//   }
//   | LOrExp LOGIC_OP_OR LAndExp {
//     auto lg_exp = new LgExpAST();
//     lg_exp->LHS = unique_ptr<BaseAST>($1);
//     lg_exp->type = static_cast<LogicOp>($2);
//     lg_exp->RHS = unique_ptr<BaseAST>($3);
//     $$ = lg_exp;
//   }
//   ;
// LAndExp
//   : EqExp {
//     auto lg_exp = new LgExpAST();
//     lg_exp->LHS = unique_ptr<BaseAST>($1);
//     $$ = lg_exp;
//   }
//   | LAndExp LOGIC_OP_AND EqExp {
//     auto lg_exp = new LgExpAST();
//     lg_exp->LHS = unique_ptr<BaseAST>($1);
//     lg_exp->type = static_cast<LogicOp>($2);
//     lg_exp->RHS = unique_ptr<BaseAST>($3);
//     $$ = lg_exp;
//   }
//   ;
// EqExp
//   : RelExp {
//     auto eq_exp = new CompExpAST();
//     eq_exp->LHS = unique_ptr<BaseAST>($1);
//     $$ = eq_exp;
//   }
//   | EqExp COMP_OP_EQ RelExp {
//     auto eq_exp = new CompExpAST();
//     eq_exp->LHS = unique_ptr<BaseAST>($1);
//     eq_exp->type = static_cast<CompOp>($2);
//     eq_exp->RHS = unique_ptr<BaseAST>($3);
//     $$ = eq_exp;
//   }
//   ;
// RelExp
//   : AddExp {
//     auto rel_exp = new CompExpAST();
//     rel_exp->LHS = unique_ptr<BaseAST>($1);
//     $$ = rel_exp;
//   }
//   | RelExp COMP_OP_N AddExp {
//     auto rel_exp = new CompExpAST();
//     rel_exp->LHS = unique_ptr<BaseAST>($1);
//     rel_exp->type = static_cast<CompOp>($2);
//     rel_exp->RHS = unique_ptr<BaseAST>($3);
//     $$ = rel_exp;
//   }
//   ;

// Number
//   : INT_CONST {
//     auto number = new NumberAST();
//     number->value = $1;
//     $$ = number;
//   }
//   ;
// Decl
//   : ConstDecl {
//     auto decl = new DeclAST();
//     decl->ConstDecl = unique_ptr<BaseAST>($1);
//     $$ = decl;
//   }
//   | VarDecl {
//     auto decl = new DeclAST();
//     decl->VarDecl = unique_ptr<BaseAST>($1);
//     $$ = decl;
//   }
//   ;
// ConstDecl
//   : CONST PrimitiveType ConstDef ConstDefs ';' {
//     auto decl = new ConstDeclAST();
//     decl->PrimitiveType = unique_ptr<BaseAST>($2);
//     decl->ConstDefs.push_back(unique_ptr<BaseAST>($3));
//     if($4 == nullptr)
//         $$ = decl;
//     else{
//         for(auto i:*($4)){
//             decl->ConstDefs.push_back(unique_ptr<BaseAST>(i));
//         }
//         delete $4;
//         $$ = decl;
//     }
//   }
//   ;
// ConstDefs
//   : ConstDefs ',' ConstDef {
//       if($1 == nullptr){
//         auto vec = new vector<BaseAST*>;
//         vec->push_back($3);
//         $$ = vec;
//       }
//       else{
//         $1->push_back($3);
//         $$ = $1;
//       }
//   }
//   | { $$ = nullptr; }
//   ;
// ConstDef
//   : IDENT '=' ConstInitVal {
//     auto def = new ConstDefAST();
//     def->ident = *unique_ptr<string>($1);
//     def->ConstInitVal = unique_ptr<BaseAST>($3);
//     $$ = def;
//   }
//   | IDENT '[' ConstExp ']' '=' ConstInitVal {
//     auto def = new ConstDefAST();
//     def->ident = *unique_ptr<string>($1);
//     def->ConstExp = unique_ptr<BaseAST>($3);
//     def->ConstInitVal = unique_ptr<BaseAST>($6);
//     $$ = def;
//   }
//   ;
// ConstInitVal
//   : ConstExp {
//     auto val = new ConstInitValAST();
//     val->ConstExp = unique_ptr<BaseAST>($1);
//     $$ = val;
//   }
//   | '{' ConstExps '}' {
//     auto val = new ConstInitValAST();
//     for(auto i: *$2){
//         val->vals.push_back(unique_ptr<BaseAST>(i));
//     }
//     delete $2;
//     $$ = val;
//   }
//   ;
// ConstExps
//  : ConstExps ',' ConstExp {
//     $1->push_back($3);
//     $$ = $1;
//  }
//  | ConstExp {
//     auto vec = new vector<BaseAST*>;
//     vec->push_back($1);
//     $$ = vec;
//  }
//   ;
// ConstExp
//   : Exp {
//     auto exp = new ConstExpAST();
//     exp->Exp = unique_ptr<BaseAST>($1);
//     $$ = exp;
//   }
//   ;
// LVal
//   : IDENT {
//     auto l_val = new LValAST();
//     l_val->ident = *unique_ptr<string>($1);
//     $$ = l_val;
//   }
//   | IDENT '[' Exp ']' {
//     auto l_val = new LValAST();
//     l_val->ident = *unique_ptr<string>($1);
//     l_val->Exp = unique_ptr<BaseAST>($3);
//     $$ = l_val;
//   }
//   ;
// BlockItem
//   : Decl {
//     auto item = new BlockItemAST();
//     item->Decl = unique_ptr<BaseAST>($1);
//     $$ = item;
//   }
//   | Stmt {
//     auto item = new BlockItemAST();
//     item->Stmt = unique_ptr<BaseAST>($1);
//     $$ = item;
//   }
//   ;
// PrimaryExp
//   : '(' Exp ')' {
//     auto primary = new PrimaryExpAST();
//     primary->Exp = unique_ptr<BaseAST>($2);
//     $$ = primary;
//   }
//   | LVal {
//     auto primary = new PrimaryExpAST();
//     primary->LVal = unique_ptr<BaseAST>($1);
//     $$ = primary;
//   }
//   | Number {
//     auto primary = new PrimaryExpAST();
//     primary->Number = unique_ptr<BaseAST>($1);
//     $$ = primary;
//   }
//   ;
// Block
//   : '{' BlockItems '}' {
//     auto block = new BlockAST();
//     // 可能是空块
//     if($2 == nullptr)
//         $$ = block;
//     else{
//         for(auto i: *$2){
//             block->BlockItems.push_back(unique_ptr<BaseAST>(i));
//         }
//         delete $2;
//         $$ = block;
//     }
//   }
//   ;
// BlockItems
//   : BlockItems BlockItem {
//     if($1 == nullptr){
//         auto vec = new vector<BaseAST*>;
//         vec->push_back($2);
//         $$ = vec;
//     }
//     else{
//         $1->push_back($2);
//         $$ = $1;
//     }
//   }
//   | { $$ = nullptr; }
//   ;
// InitVal
//   : Exp {
//     auto val = new InitValAST();
//     val->Exp = unique_ptr<BaseAST>($1);
//     $$ = val;
//   }
//   //
//   | '{' DExps '}' {
//     auto val = new ExpAST();
//     for(auto i: *$2){
//         val->vals.push_back(unique_ptr<BaseAST>(i));
//     }
//     delete $2;
//     $$ = val;
//   }
//   ;
// VarDecl
//   : PrimitiveType VarDef VarDefs ';' {
//     auto var = new VarDeclAST();
//     var->PrimitiveType = unique_ptr<BaseAST>($1);
//     var->VarDefs.push_back(unique_ptr<BaseAST>($2));
//     if($3 == nullptr)
//         $$ = var;
//     else{
//         for(auto i:*($3)){
//             var->VarDefs.push_back(unique_ptr<BaseAST>(i));
//         }
//         delete $3;
//         $$ = var;
//     }
//   }
//   ;
// VarDefs
//   : VarDefs ',' VarDef {
//     if($1 == nullptr){
//         auto vec = new vector<BaseAST*>;
//         vec->push_back($3);
//         $$ = vec;
//     }
//     else{
//         $1->push_back($3);
//         $$ = $1;
//     }
//   }
//   | { $$ = nullptr; }
//   ;
// VarDef
//   : IDENT '=' InitVal {
//     auto var = new VarDefAST();
//     var->ident = *unique_ptr<string>($1);
//     var->InitVal = unique_ptr<BaseAST>($3);
//     $$ = var;
//   }
//   | IDENT '[' ConstExp ']' '=' InitVal {
//     auto def = new VarDefAST();
//     def->ident = *unique_ptr<string>($1);
//     def->ConstExp = unique_ptr<BaseAST>($3);
//     def->InitVal = unique_ptr<BaseAST>($6);
//     $$ = def;
//   }
//   | IDENT {
//     auto var = new VarDefAST();
//     var->ident = *unique_ptr<string>($1);
//     $$ = var;
//   }
//   | IDENT '[' ConstExp ']'{
//     auto var = new VarDefAST();
//     var->ident = *unique_ptr<string>($1);
//     var->ConstExp = unique_ptr<BaseAST>($3);
//     $$ = var;
//   }
//   ;