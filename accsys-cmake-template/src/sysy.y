%{
#include <stdio.h>
#include <stdlib.h>
#include <ast/ast.h>
#include <string>
#include <vector>
void yyerror(const char *s);
extern int yylex(void);
extern NodePtr root;
%}

/// types
%union {
    int ival;
    std::string *sval;
    NodePtr nodeVal;
    std::vector<NodePtr>* vecVal;
    std::vector<int>* iVecVal;
    OpType opVal;
}

%token <ival> CONSTINT UNARYOP BINARYOP
%token <sval> IDENTIFIER
%token <opVal> AND OR NOT
%token <opVal> NEQ EQ GEQ LEQ GREAT LESS
%type <nodeVal> FuncDef FuncType Block Stmt  Exp UnaryExp PrimaryExp Number AddExp MulExp
%type <nodeVal> LOrExp LAndExp EqExp RelExp
%type <nodeVal> Decl BlockItem LVal
%type <nodeVal> VarDecl InitVal VarDef
%type <nodeVal> FuncFParam
%type <nodeVal> Matched Unmatched
%type <vecVal> BlockItems VarDefs
%type <vecVal> FuncFParams DFuncFParams FuncRParams DExps CompUnits
%type <iVecVal> Dimensions Dimensions_funcdef Dimensions_lval


%token  ASSIGN ADDEQ SUBEQ MULEQ DIVEQ MODEQ
        IF ELSE FOR WHILE SWITCH CASE DEFAULT CONTINUE BREAK
        RETURN INT VOID
        COMMA SEMI LPAREN RPAREN LBRACKET RBRACKET LBRACE RBRACE

%start CompUnit

// JY write
%%
CompUnit : CompUnits {
                auto comp_unit = new CompUnit();       
                for(auto a : *$1) 
                        comp_unit->all.emplace_back(a);
                root = comp_unit;
                printf("root is initialized\n");
        };
CompUnits: {}
        | CompUnits Decl { 
                $1->emplace_back($2);
                $$ = $1;
                printf("CompUnits Decl\n");
        }
        | CompUnits FuncDef { 
                $1->emplace_back($2);
                $$ = $1;
                printf("CompUnits FuncDef\n");
        }
        | Decl {
                auto decl_units = new std::vector<NodePtr>;
                decl_units->emplace_back($1);
                $$ = decl_units;
                printf("Decl\n");
        }
        | FuncDef {
                auto funcdef_units = new std::vector<NodePtr>;
                funcdef_units->emplace_back($1);
                $$ = funcdef_units;
                printf("FuncDef\n");
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

InitVal : Exp {
                auto val = new InitVal();
                val->Exp = $1;
                $$ = val;
        };

// func def   
// int f(int a, int b[]){...}
// FuncType
FuncDef : INT IDENTIFIER LPAREN FuncFParams RPAREN Block {
		printf("int FuncDef\n");
    auto funcdef_unit = new FuncDef();
    funcdef_unit->ReturnType = FuncDef::Type::INT;
    funcdef_unit->name = *($2);
    if($4 != nullptr){
        for(auto i : *$4)
            funcdef_unit->argList.emplace_back(i);
    }
    else{  }
    funcdef_unit->block = $6;
    $$ = funcdef_unit;
  }
  | VOID IDENTIFIER LPAREN FuncFParams RPAREN Block {
  printf("void FuncDef\n");
		auto funcdef_unit = new FuncDef();
		funcdef_unit->ReturnType = FuncDef::Type::VOID;
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
// FuncType :
//	VOID{
//		auto node = new FuncType(FuncType::Type::VOID);
//		$$ = node;
//  }
//	| INT {
//		auto node = new FuncType(FuncType::Type::INT);
//		$$ = node;
//	}
//	;
FuncFParams
  : DFuncFParams {
    $$ = $1;
  }
  | {
    $$ = nullptr;
    printf("FuncFParams is void\n");
  }
  ;
DFuncFParams
  : DFuncFParams COMMA FuncFParam {
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
  | INT IDENTIFIER Dimensions_funcdef { // LBRACKET RBRACKET
    auto para = new FuncFParam();
    para->name = *($2);
    para->isArray = true;
    para->dimensions = *$3;
    $$ = para;    
  };
Dimensions_funcdef 
  : LBRACKET RBRACKET {
     auto vec = new std::vector<int>;
     vec->emplace_back(-1);// [][2] ==> -1, 2
     $$ = vec;
  }
  | Dimensions_funcdef LBRACKET CONSTINT RBRACKET {
      auto dims = $1;
      dims->emplace_back($3);
      $$ = dims; 
    }
  ;

// {......}
Block: LBRACE BlockItems RBRACE 
  {
    auto block = new Block();
    if($2 == nullptr){
		    $$ = block;
		    printf("Block is empty\n");
    }else{
        for(auto i: *$2){
            block->BlockItems.emplace_back(i);
        }
        printf("BlockItem.size = %d\n", block->BlockItems.size());
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
				//dynamic_cast<NodePtr>($1)->matched = true;
				$$ = $1;
		}
  | Unmatched {
				//dynamic_cast<NodePtr>($1)->matched = false;
				$$ = $1;
 };
Matched:
		LVal ASSIGN Exp SEMI {
		    auto stmt = new AssignStmt();
		    stmt->LVal = ($1);
		    stmt->matched = true;
		    stmt->Exp = ($3);
		    $$ = stmt;
		}
		| Exp SEMI {
		    auto stmt = new ExpStmt();
		    stmt->matched = true;
		    stmt->Exp = ($1);
		    $$ = stmt;
		}
		| SEMI { // include " ;"
		    auto stmt = new ExpStmt();
		    stmt->matched = true;
		    $$ = stmt;
		}
		| Block {
		    auto stmt = new BlockStmt();
		    stmt->Block = ($1);
		    stmt->matched = true;
		    $$ = stmt;
		 }
		| IF LPAREN Exp RPAREN Matched ELSE Matched {
		    //dynamic_cast<IfStmt*>($5)->matched = true;
		    //dynamic_cast<IfStmt*>($7)->matched = true;
		    auto stmt = new IfStmt();
		    stmt->condition = ($3);
		    stmt->matched = true;
		    stmt->then = ($5);
		    stmt->els = ($7);
		    $$ = stmt;
		 }
		| WHILE LPAREN Exp RPAREN Stmt {
		    auto stmt = new WhileStmt();
		    stmt->condition = ($3);
		    stmt->matched = true;
		    stmt->then = ($5);
		    $$ = stmt;
		 }
		| BREAK SEMI {
		    auto stmt = new BreakStmt();
		    stmt->matched = true;
		    $$ = stmt;
		}
		| CONTINUE SEMI {
		    auto stmt = new ContinueStmt();
		    stmt->matched = true;
		    $$ = stmt;
		 }
		| RETURN Exp SEMI {
		    auto stmt = new ReturnStmt();
		    stmt->result = $2;
		    stmt->matched = true;
		    $$ = stmt;
		 }
		| RETURN SEMI {
		    auto stmt = new ReturnStmt();
		    stmt->matched = true;
		    $$ = stmt;
		}
		;
Unmatched:
		IF LPAREN Exp RPAREN Stmt {
		    auto stmt = new IfStmt();
		    stmt->condition = ($3);
		    stmt->matched = false;
		    stmt->then = ($5);
		    $$ = stmt;
		}
		| IF LPAREN Exp RPAREN Matched ELSE Unmatched {
		    //dynamic_cast<IfStmt*>($5)->matched = true;
		    //dynamic_cast<IfStmt*>($7)->matched = false;
		    auto stmt = new IfStmt();
		    stmt->condition = ($3);
		    stmt->matched = false;
		    stmt->then = ($5);
		    stmt->els = ($7);
		    $$ = stmt;
		}
		;

Exp : LOrExp { 
            auto exp = new Expr();
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
	| UNARYOP UnaryExp {
		auto unary = new UnaryExpr();
    unary->unaryExp = $2;
    if($1 == '+'){
        unary->opType = OpType::OP_Add;
    }else if($1 == '-'){
        unary->opType = OpType::OP_Neg;
    }else if($1 == '!'){
		     unary->opType = OpType::OP_Lnot;
		}
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
    mul_exp->unaryExp = ($1);
    $$ = mul_exp;
 }
	| MulExp BINARYOP UnaryExp {
		auto mul_exp = new MulExp();
    mul_exp->mulExp = ($1);
    if($2 == '*'){
		    mul_exp->optype = OpType::OP_Mul;
    }else if($2 == '/'){
        mul_exp->optype = OpType::OP_Div;
    }else if($2 == '%'){
        mul_exp->optype = OpType::OP_Mod;
    }
    mul_exp->unaryExp = $3;
    $$ = mul_exp;
	};

AddExp: MulExp { 
		auto add_exp = new AddExp();
    add_exp->mulExp = ($1);
    $$ = add_exp;
 }
	| AddExp UNARYOP MulExp {
		auto add_exp = new AddExp();
    add_exp->addExp = $1;
    if($2 == '+'){
        add_exp->optype = OpType::OP_Add;
    }else if($2 == '-'){
        add_exp->optype = OpType::OP_Sub;
    }
    add_exp->mulExp = $3;
    $$ = add_exp;
	};

RelExp: AddExp { 
		auto rel_exp = new CompExp();
    rel_exp->lhs = $1;
    $$ = rel_exp;
 }
	| RelExp LESS AddExp { 
		auto rel_exp = new CompExp();
    rel_exp->lhs = ($1);
		rel_exp->optype = OpType::OP_Lt;
		rel_exp->rhs = $3;
    $$ = rel_exp;	
	}
	| RelExp GREAT AddExp { 
		auto rel_exp = new CompExp();
    rel_exp->lhs = $1;
		rel_exp->optype = OpType::OP_Gt;
		rel_exp->rhs = $3;
    $$ = rel_exp;	
	}
	| RelExp LEQ AddExp { 
		auto rel_exp = new CompExp();
    rel_exp->lhs = $1;
		rel_exp->optype = OpType::OP_Le;
		rel_exp->rhs = $3;
    $$ = rel_exp;	
	}
	| RelExp GEQ AddExp { 
		auto rel_exp = new CompExp();
    rel_exp->lhs = $1;
		rel_exp->optype = OpType::OP_Ge;
		rel_exp->rhs = $3;
    $$ = rel_exp;	
	};

EqExp: RelExp { 
		auto comp_exp = new CompExp();
    comp_exp->lhs = $1;
    $$ = comp_exp;	
 }
	| EqExp EQ RelExp { 
		auto comp_exp = new CompExp();
    comp_exp->lhs = $1;
		comp_exp->optype = OpType::OP_Eq;
		comp_exp->rhs = $3;
    $$ = comp_exp;	
	}
	| EqExp NEQ RelExp { 
		auto comp_exp = new CompExp();
    comp_exp->lhs = $1;
		comp_exp->optype = OpType::OP_Ne;
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
    lg_exp->lhs = $1;
    lg_exp->optype = OpType::OP_Land;
    lg_exp->rhs = $3;
    $$ = lg_exp;
	 };

LOrExp: LAndExp { 
		auto lg_exp = new LgExp();
    lg_exp->lhs = ($1);
    $$ = lg_exp;
}
  | LOrExp OR LAndExp { 
		auto lg_exp = new LgExp();
    lg_exp->lhs = $1;
    lg_exp->optype = OpType::OP_Lor;
    lg_exp->rhs = $3;
    $$ = lg_exp;
	};




%%

void yyerror(const char *s) {
    printf("error: %s\n", s);
}