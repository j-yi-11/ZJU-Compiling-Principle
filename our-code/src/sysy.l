%option noinput
%option nounput
%option noyywrap

%{
#include <ast/ast.h>
#include <string>
#include "sysy.tab.hh"

int lineCount = 0;
%}

digit [1-9][0-9]*|0
blank [ \t\n]
identifier [a-zA-Z_][a-zA-Z0-9_]*

%%

{digit}           { yylval.ival = atoi(yytext); return CONSTINT; }

"+"             { return ADD; }
"-"             { return SUB; }
"*"             { return MUL; }
"/"             { return DIV; }
"%"             { return MOD; }
"!"             { return NOT; }


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

"/*"[^*]*[*]+([^*/][^*]*[*]+)*"/"           { printf(" /**/ "); }
"//".*	        { printf(" // "); }
{identifier}        { 
                         yylval.sval = new std::string(yytext, yyleng);
                         printf(" %s(ID) ", yytext);
                         return IDENTIFIER; 
										}
{blank}         { }
.               { printf("%s\n", yytext); printf("line count = %d",++lineCount); }

%%
