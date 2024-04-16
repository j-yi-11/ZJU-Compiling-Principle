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
// calculation
binary_operator [*/%]
unary_operator [!+-]
%%

{digit}         { yylval.ival = atoi(yytext); return CONSTINT; }
"+"             { return ADD; }
"+="            { return ADDEQ; }
"-"             { return SUB; }

"-="            { return SUBEQ; }
"*"             { return MUL; }
"*="            { return MULEQ; }
"/"             { return DIV; }
"/="            { return DIVEQ; }
"%"             { return MOD; }
"%="            { return MODEQ; }
"="             { return ASSIGN; }
"=="            { return EQ; }
"!="            { return NEQ; }
">="            { return GEQ; }
"<="            { return LEQ; }
">"             { return GREAT; }
"<"             { return LESS; }
"!"             { return NOT; }
"&&"            { return AND; }
"||"            { return OR; }

"if"			{ return IF; }
"else"			{ return ELSE; }
"for"			{ return FOR; }
"while"			{ return WHILE; }
"switch"		{ return SWITCH; }
"case"			{ return CASE; }
"default"		{ return DEFAULT; }
"continue"		{ return CONTINUE; }
"return"		{ return RETURN; }
"break"			{ return BREAK; }
"int"			{ return INT; }
"void"			{ return VOID; }

","             { return COMMA; }
":"             { return COLON; }
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
                         yylval.sVal = new std::string(yytext, yyleng);
                         return IDENTIFIER; 
				} 
{blank}         { }
.               { printf("ERROR(%s)\n", yytext); }

%%
