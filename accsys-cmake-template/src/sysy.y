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
%type <nodeVal> Matched Unmatched
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
	Matched {
				dynamic_cast<NodePtr>($1)->matched = true;
				$$ = $1;
		}
  | Unmatched {
				dynamic_cast<NodePtr>($1)->matched = false;
				$$ = $1;
 };
Matched:
		LVal ASSIGN Exp SEMI {
		    auto stmt = new AssignStmt();
		    stmt->LVal = ($1);
		    stmt->Exp = ($3);
		    $$ = stmt;
		}
		| Exp SEMI {
		    auto stmt = new ExpStmt();
		    stmt->Exp = ($1);
		    $$ = stmt;
		}
		| SEMI { // include " ;"
		    auto stmt = new ExpStmt();
		    $$ = stmt;
		}
		| Block {
		    auto stmt = new BlockStmt();
		    stmt->Block = ($1);
		    $$ = stmt;
		 }
		| IF LPAREN Exp RPAREN Matched ELSE Matched {
		    dynamic_cast<IfStmt*>($5)->matched = true;
		    dynamic_cast<IfStmt*>($7)->matched = true;
		    auto stmt = new IfStmt();
		    stmt->condition = ($3);
		    stmt->then = ($5);
		    stmt->else_stmt = ($7);
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
Unmatched:
		IF LPAREN Exp RPAREN Stmt {
		    auto stmt = new IfStmt();
		    stmt->condition = ($3);
		    stmt->then = ($5);
		    $$ = stmt;
		}
		| IF LPAREN Exp RPAREN Matched ELSE Unmatched {
		    dynamic_cast<IfStmt*>($5)->matched = true;
		    dynamic_cast<IfStmt*>($7)->matched = false;
		    auto stmt = new IfStmt();
		    stmt->condition = ($3);
		    stmt->then = ($5);
		    stmt->else_stmt = ($7);
		    $$ = stmt;
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