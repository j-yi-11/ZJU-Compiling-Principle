%{
#include <stdio.h>
#include <stdlib.h>
#include <ast/ast.h>
#include <string>
#include <iostream>
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

%token <ival> CONSTINT 
// %token <ival> UNARYOP BINARYOP
%token <sval> IDENTIFIER
// jy comment 5.14
// %token <opVal> AND OR NOT
// %token <opVal> NEQ EQ GEQ LEQ GREAT LESS
%type <nodeVal> FuncDef Block Stmt  Exp 
// %type <nodeVal> UnaryExp PrimaryExp Number AddExp MulExp
// %type <nodeVal> LOrExp LAndExp EqExp RelExp
%type <nodeVal> Decl BlockItem LVal
%type <nodeVal> VarDecl VarDef
// %type <nodeVal> InitVal
%type <nodeVal> FuncFParam
%type <nodeVal> Matched Unmatched
%type <vecVal> BlockItems VarDefs
%type <vecVal> FuncFParams DFuncFParams FuncRParams DExps CompUnits
%type <iVecVal> Dimensions Dimensions_funcdef 
%type <vecVal> Dimensions_lval
// jy add 5.14 ADD SUB MUL DIV MOD NOT
%token  ASSIGN
				ADD SUB MUL DIV MOD NOT
        IF ELSE WHILE CONTINUE BREAK
        RETURN INT VOID
        COMMA SEMI LPAREN RPAREN LBRACKET RBRACKET LBRACE RBRACE

%start CompUnit

// jy add 5.14
%right ASSIGN
%left OR
%left AND
%left EQ NEQ
%left GEQ LEQ GREAT LESS
%left ADD SUB
%left MUL DIV MOD
%right NOT

// JY write
%%
CompUnit : CompUnits {
                std::cout<<"CompUnit";
                auto comp_unit = new CompUnit();       
                for(auto a : *$1) 
                        comp_unit->all.emplace_back(a);
                root = comp_unit;
        };
CompUnits: //{}|
        CompUnits Decl {
                $1->emplace_back($2);
                $$ = $1;
                // printf("CompUnits Decl\n");
        }
        | CompUnits FuncDef { 
                $1->emplace_back($2);
                $$ = $1;
                // printf("CompUnits FuncDef\n");
        }
        | Decl {
                auto decl_units = new std::vector<NodePtr>;
                decl_units->emplace_back($1);
                $$ = decl_units;
                // printf("Decl\n");
        }
        | FuncDef {
                auto funcdef_units = new std::vector<NodePtr>;
                funcdef_units->emplace_back($1);
                $$ = funcdef_units;
                // printf("FuncDef\n");
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
VarDef : IDENTIFIER ASSIGN Exp { // IDENTIFIER ASSIGN InitVal
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

// func def   
// int f(int a, int b[]){...}
// FuncType
FuncDef : INT IDENTIFIER LPAREN FuncFParams RPAREN Block {
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
FuncFParams
  : DFuncFParams {
    $$ = $1;
  }
  | {
    $$ = nullptr;
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
    }else{
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
				$$ = $1;
		}
  | Unmatched {
				$$ = $1;
 };
Matched:
		LVal ASSIGN Exp SEMI {
		    auto stmt = new AssignStmt();
		    stmt->LVal = $1;
		    stmt->matched = true;
		    stmt->Exp = $3;
		    $$ = stmt;
		}
		| Exp SEMI {
		    auto stmt = new ExpStmt();
		    stmt->matched = true;
		    stmt->Exp = $1;
		    $$ = stmt;
		}
		| SEMI { // include " ;"
		    auto stmt = new ExpStmt();
		    stmt->matched = true;
		    $$ = stmt;
		}
		| Block {
		    auto stmt = new BlockStmt();
		    stmt->Block = $1;
		    stmt->matched = true;
		    $$ = stmt;
		 }
		| IF LPAREN Exp RPAREN Matched ELSE Matched {
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
		    auto stmt = new IfStmt();
		    stmt->condition = ($3);
		    stmt->matched = false;
		    stmt->then = ($5);
		    stmt->els = ($7);
		    $$ = stmt;
		}
		;
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
Dimensions_lval :
		LBRACKET RBRACKET {
			std::cout<<"Dimensions_lval";
			// $$ = new std::vector<int>;
			$$ = new std::vector<NodePtr>;
			$$->emplace_back(nullptr);
		}
		|
		LBRACKET Exp RBRACKET { //  CONSTINT
			// $$ = new std::vector<int>;
			$$ = new std::vector<NodePtr>;
			$$->emplace_back($2);
		}
		| Dimensions_lval LBRACKET Exp RBRACKET  { // CONSTINT
		    $1->emplace_back($3);
		    $$ = $1;
		};

FuncRParams: Exp { 
		auto param = new std::vector<NodePtr>;
    param->emplace_back($1);
    $$ = param;
  }
	| DExps COMMA Exp {
	  std::cout<<"FuncRParams";
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

// jy want to change

Exp:
	Exp ADD Exp {
		auto exp = new addExp();
		exp->lhs = $1;
		exp->rhs = $3;
		$$ = exp;
	}
	| Exp SUB Exp {
		auto exp = new subExp();
		exp->lhs = $1;
		exp->rhs = $3;
		$$ = exp;
	}
	| Exp MUL Exp {
		auto exp = new mulExp();
		exp->lhs = $1;
		exp->rhs = $3;
		$$ = exp;
	}
	| Exp DIV Exp {
		auto exp = new divExp();
		exp->lhs = $1;
		exp->rhs = $3;
		$$ = exp;
	}
	| Exp MOD Exp {
		auto exp = new modExp();
		exp->lhs = $1;
		exp->rhs = $3;
		$$ = exp;
	}
	| Exp AND Exp {
		auto exp = new andExp();
		exp->lhs = $1;
		exp->rhs = $3;
		$$ = exp;
	}
	| Exp OR Exp {
		auto exp = new orExp();
		exp->lhs = $1;
		exp->rhs = $3;
		$$ = exp;
	}
	| Exp EQ Exp {
		auto exp = new eqExp();
		exp->lhs = $1;
		exp->rhs = $3;
		$$ = exp;
	}
	| Exp NEQ Exp {
		auto exp = new neqExp();
		exp->lhs = $1;
		exp->rhs = $3;
		$$ = exp;
	}
	| Exp GEQ Exp {
		auto exp = new geqExp();
		exp->lhs = $1;
		exp->rhs = $3;
		$$ = exp;
	}
	| Exp LEQ Exp {
		auto exp = new leqExp();
		exp->lhs = $1;
		exp->rhs = $3;
		$$ = exp;
	}
	| Exp GREAT Exp {
		auto exp = new greatExp();
		exp->lhs = $1;
		exp->rhs = $3;
		$$ = exp;
	}
	| Exp LESS Exp {
		auto exp = new lessExp();
		exp->lhs = $1;
		exp->rhs = $3;
		$$ = exp;
	}
	| NOT Exp {
		auto exp = new notExp();
		exp->lhs = $2;
		$$ = exp;
	}
	| ADD Exp %prec NOT {
		auto exp = new posExp();
		exp->lhs = $2;
		$$ = exp;
	}
	| SUB Exp %prec NOT {
		auto exp = new negExp();
		exp->lhs = $2;
		$$ = exp;
	}
	| LPAREN Exp RPAREN {
		$$ = $2;
	}
	| CONSTINT {
		auto exp = new intExp();
		exp->value = $1;
		$$ = exp;
	}
	| IDENTIFIER LPAREN FuncRParams RPAREN {
		auto exp = new funcallExp();
		exp->name = *($1);
		if($3 == nullptr)
			$$ = exp;
		else{
			for(auto i: *$3)
				exp->params.emplace_back(i);
			$$ = exp;
		}
	}
	| LVal {
		$$ = $1;
	}
//	| LVal ASSIGN Exp {
//		auto exp = new assignExp();
//		exp->lhs = $1;
//		exp->rhs = $3;
//		$$ = exp;
//	}
	;

// previous



// PrimaryExp: LPAREN Exp RPAREN {  // ok
//       auto primary = new PrimaryExpr();
// 			primary->Exp = ($2);
// 			$$ = primary;
// 		}
// 		| LVal { //ok
// 			auto primary = new PrimaryExpr();
// 			primary->LVal = ($1);
// 			$$ = primary;
// 		}
// 		| CONSTINT { // ok
// 			auto primary = new PrimaryExpr();
// 			primary->Number = new IntegerLiteral($1);
// 			$$ = primary;
// 		};

// UnaryExp: PrimaryExp { // ok
// 		auto unary = new UnaryExpr();
//     unary->primaryExp = ($1);
//     $$ = unary;
// 	}
// 	| IDENTIFIER LPAREN FuncRParams RPAREN { //ok
// 		auto unary = new UnaryExpr();
// 		unary->name = *$1;
// 		unary->isFunCall = true;
// 		if($3 == nullptr)
// 			$$ = unary;
// 		else{
// 			for(auto i: *$3)
//             unary->params.emplace_back(i);
//         $$ = unary;
// 		}
// 	}
// 	| UNARYOP UnaryExp { //ok
// 		auto unary = new UnaryExpr();
//     unary->unaryExp = $2;
//     if($1 == '+'){
//         unary->opType = OpType::OP_Add;
//     }else if($1 == '-'){
//         unary->opType = OpType::OP_Neg;
//     }else if($1 == '!'){
// 		     unary->opType = OpType::OP_Lnot;
// 		}
//     $$ = unary;
// 	};


// MulExp: UnaryExp { //ok
// 		auto mul_exp = new MulExp();
//     mul_exp->unaryExp = ($1);
//     $$ = mul_exp;
//  }
// 	| MulExp BINARYOP UnaryExp { //ok
// 		auto mul_exp = new MulExp();
//     mul_exp->mulExp = ($1);
//     if($2 == '*'){
// 		    mul_exp->optype = OpType::OP_Mul;
//     }else if($2 == '/'){
//         mul_exp->optype = OpType::OP_Div;
//     }else if($2 == '%'){
//         mul_exp->optype = OpType::OP_Mod;
//     }
//     mul_exp->unaryExp = $3;
//     $$ = mul_exp;
// 	};

// AddExp: MulExp { //ok
// 		auto add_exp = new AddExp();
//     add_exp->mulExp = ($1);
//     $$ = add_exp;
//  }
// 	| AddExp UNARYOP MulExp { //ok
// 		auto add_exp = new AddExp();
//     add_exp->addExp = $1;
//     if($2 == '+'){
//         add_exp->optype = OpType::OP_Add;
//     }else if($2 == '-'){
//         add_exp->optype = OpType::OP_Sub;
//     }
//     add_exp->mulExp = $3;
//     $$ = add_exp;
// 	};

// RelExp: AddExp { //ok
// 		auto rel_exp = new CompExp();
//     rel_exp->lhs = $1;
//     $$ = rel_exp;
//  }
// 	| RelExp LESS AddExp { //ok
// 		auto rel_exp = new CompExp();
//     rel_exp->lhs = ($1);
// 		rel_exp->optype = OpType::OP_Lt;
// 		rel_exp->rhs = $3;
//     $$ = rel_exp;	
// 	}
// 	| RelExp GREAT AddExp { //ok
// 		auto rel_exp = new CompExp();
//     rel_exp->lhs = $1;
// 		rel_exp->optype = OpType::OP_Gt;
// 		rel_exp->rhs = $3;
//     $$ = rel_exp;	
// 	}
// 	| RelExp LEQ AddExp { //ok
// 		auto rel_exp = new CompExp();
//     rel_exp->lhs = $1;
// 		rel_exp->optype = OpType::OP_Le;
// 		rel_exp->rhs = $3;
//     $$ = rel_exp;	
// 	}
// 	| RelExp GEQ AddExp { //ok
// 		auto rel_exp = new CompExp();
//     rel_exp->lhs = $1;
// 		rel_exp->optype = OpType::OP_Ge;
// 		rel_exp->rhs = $3;
//     $$ = rel_exp;	
// 	};


// EqExp: RelExp { //ok
// 		auto comp_exp = new CompExp();
//     comp_exp->lhs = $1;
//     $$ = comp_exp;	
//  }
// 	| EqExp EQ RelExp { //ok
// 		auto comp_exp = new CompExp();
//     comp_exp->lhs = $1;
// 		comp_exp->optype = OpType::OP_Eq;
// 		comp_exp->rhs = $3;
//     $$ = comp_exp;	
// 	}
// 	| EqExp NEQ RelExp { //ok
// 		auto comp_exp = new CompExp();
//     comp_exp->lhs = $1;
// 		comp_exp->optype = OpType::OP_Ne;
// 		comp_exp->rhs = $3;
//     $$ = comp_exp;	
// 	};

// LAndExp: EqExp { //ok
// 		auto lg_exp = new LgExp();
//     lg_exp->lhs = ($1);
//     $$ = lg_exp;
//  }
// 	| LAndExp AND EqExp { //ok
// 		auto lg_exp = new LgExp();
//     lg_exp->lhs = $1;
//     lg_exp->optype = OpType::OP_Land;
//     lg_exp->rhs = $3;
//     $$ = lg_exp;
// 	 };

// LOrExp: LAndExp { // ok
// 		auto lg_exp = new LgExp();
//     lg_exp->lhs = ($1);
//     $$ = lg_exp;
// }
//   | LOrExp OR LAndExp { // ok
// 		auto lg_exp = new LgExp();
//     lg_exp->lhs = $1;
//     lg_exp->optype = OpType::OP_Lor;
//     lg_exp->rhs = $3;
//     $$ = lg_exp;
// 	};

// Exp : LOrExp { // ok
//            auto exp = new Expr();
//            exp->LgExp = ($1);
//            $$ = exp;
//        };


%%

void yyerror(const char *s) {
    printf("error: %s\n", s);
}