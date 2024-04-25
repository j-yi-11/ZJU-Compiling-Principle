%option noinput
%option nounput
%option noyywrap

%{
#include <ast/ast.h>
#include <string>
#include "sysy.tab.hh"
%}

digit [1-9][0-9]*|0
blank [ \t\n]
identifier [a-zA-Z_][a-zA-Z0-9_]*

binary_operator [\*/%]
unary_operator [!+-]
%%

{digit}           { yylval.ival = atoi(yytext); return CONSTINT; }
{unary_operator}  { yylval.ival = (int)yytext[0]; return UNARYOP; }
{binary_operator} { yylval.ival = (int)yytext[0]; return BINARYOP; }

"="             { return ASSIGN; }
"=="            { return EQ; }
"!="            { return NEQ; }
">="            { return GEQ; }
"<="            { return LEQ; }
">"             { return GREAT; }
"<"             { return LESS; }
"&&"            { return AND; }
"||"            { return OR; }

"if"			{ return IF; }
"else"			{ return ELSE; }
"while"			{ return WHILE; }
"continue"		{ return CONTINUE; }
"return"		{ return RETURN; }
"break"			{ return BREAK; }
"int"			{ return INT; }
"void"			{ return VOID; }

","       { return COMMA; }
";"				{ return SEMI; }
"("				{ return LPAREN; }
")"				{ return RPAREN; }
"["				{ return LBRACKET; }
"]"				{ return RBRACKET; }
"{"				{ return LBRACE; }
"}"				{ return RBRACE; }

"/*"[^*]*[*]+([^*/][^*]*[*]+)*"/"           { ; }
"//".*	        { ; }
{identifier}        { 
                         yylval.sval = new std::string(yytext, yyleng);
                         return IDENTIFIER; 
										}
{blank}         { }
.               { /*printf("ERROR(%s)\n", yytext);*/ }

%%
