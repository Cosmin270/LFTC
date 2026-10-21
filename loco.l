%{
#include <stdio.h>
#include "SymbolTable.h"
#include "Pif.h"

int error_count = 0;
%}

%option noyywrap yylineno nounput noinput

DIGIT   [0-9]
LETTER  [a-zA-Z]
ID      {LETTER}({LETTER}|{DIGIT}|_)*
INTCT   {DIGIT}+
REALCT  {DIGIT}+"."{DIGIT}+
STRCHAR [a-zA-Z0-9_\-+.,:;!?()/%#=<> ]
STRCT   \"{STRCHAR}*\"

%%

"type"            |
"record"          |
"end"             |
"var"             |
"int"             |
"real"            |
"bool"            |
"string"          |
"input"           |
"read"            |
"send"            |
"log"             |
"wait"            |
"if"              |
"else"            |
"while"           |
"for"             |
"in"              |
"when"            |
"true"            |
"false"           |
"traction"        |
"direction"       |
"forward"         |
"reverse"         |
"neutral"         |
"cruise"          |
"brake"           |
"direct"          |
"primary"         |
"release"         |
"running"         |
"service1"        |
"service2"        |
"service3"        |
"emergency"       |
"emergency_stop"  |
"regen"           |
"sand"            |
"pantograph"      |
"up"              |
"down"            |
"breaker"         |
"open"            |
"close"           |
"compressor"      |
"start"           |
"stop"            |
"horn"            |
"high"            |
"low"             |
"lights"          |
"back"            |
"off"             |
"alarm"           |
"DSV"             |
"PZB"             |
"Fire"            |
"WheelSlip"       |
"U"               |
"O"               |
"M"               { add_pif(yytext, -1); }

"=="              |
"!="              |
"<="              |
">="              |
"||"              |
"&&"              |
".."              |
"="               |
"<"               |
">"               |
"!"               |
"+"               |
"-"               |
"*"               |
"/"               |
"%"               { add_pif(yytext, -1); }

"("               |
")"               |
"["               |
"]"               |
":"               |
";"               |
","               |
"."               { add_pif(yytext, -1); }

{ID}              { add_pif("IDENTIFIER", st_insert(yytext)); }

{REALCT}          |
{INTCT}           |
{STRCT}           { add_pif("CONSTANT", st_insert(yytext)); }

{DIGIT}+({LETTER}|_)({LETTER}|{DIGIT}|_)* {
                     printf("Lexical error at line %d: invalid identifier '%s'\n", yylineno, yytext);
                     error_count++;
                 }

\"[^"\n]*\"       {
                     printf("Lexical error at line %d: invalid character in string %s\n", yylineno, yytext);
                     error_count++;
                 }

\"[^"\n]*         {
                     printf("Lexical error at line %d: unterminated string %s\n", yylineno, yytext);
                     error_count++;
                 }

[ \t\r\n]+        { /* skip whitespace */ }

.                 {
                     printf("Lexical error at line %d: illegal character '%s'\n", yylineno, yytext);
                     error_count++;
                 }

%%

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <source file>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        printf("Cannot open %s\n", argv[1]);
        return 1;
    }

    yylex();
    fclose(yyin);

    FILE *pif_out = fopen("PIF.out", "w");
    pif_print(pif_out);
    fclose(pif_out);

    FILE *st_out = fopen("ST.out", "w");
    st_print(st_out);
    fclose(st_out);

    if (error_count == 0)
        printf("Lexically correct\n");
    else
        printf("%d lexical error(s) found\n", error_count);

    pif_free();
    st_free();
    return 0;
}
