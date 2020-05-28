/*
     Created by: Braden Luancing
     Major: Computer Science
     Creation Date: 9/20/19
     Due Date: 9/??/19
     Course: CSC425
     Professor: Dr. Schwesinger
     Assignment: #1
     Filename: tigerLex.l
     Purpose: Creates a scanner/lexer for the Tiger language.
     
     Updated on: 10/2/19
     Changes:  Rewrote part of it to allow multiline comments,
               fixed the pattern for string literals, and allowed it to 
               cooperate with the Bison parsing tool.

*/

%option noyywrap

%{
     #include <stdio.h>
     #include <stdlib.h>
     #include <cstdio>
     #include <iostream>
     #include <set>
     #include <vector>
     #include "tigerParse.tab.h" //Created by YACC with -d flag
     #include "ast.h"
     
     using namespace std;
     
     int lineNumber = 1;
     int charNumber = 1;
     int commentLineStart;
     
%}

WS	     [ \t]+

ALPHA     [A-Za-z]
DIGIT     [0-9]+
ID	     [A-Za-z][_A-Za-z0-9]*
TYPE      "int"|"string"|[A-Za-z][_A-Za-z0-9]*

COMMENT   "/*"([^*]|(\*+[^*/]))*\*+\/

STRLIT    \"(\\.|[^"\\])*\"

NEWLINE   (\r\n)|(\n)

%x COMMENT

%% 




{WS}           ;

{NEWLINE}             { //printf("<End line #%d>(new line)\n", lineNumber); 
               lineNumber++;
               }

"/*"           {commentLineStart = lineNumber; BEGIN COMMENT;}     

\"[^"]*  {std::cout << "ERROR: " << lineNumber << ": Lexer: Unterminated string \n";
               exit(1);}

"("            {return LPAREN;}
")"            {return RPAREN;}
"["            {return LBRACK;}
"]"            {return RBRACK;}
"{"            {return LBRACE;}
"}"            {return RBRACE;}
":"            {return COLON;}
":="           {return ASSIGNMENT;}
"."            {return PERIOD;}
","            {return COMMA;}
";"            {return SEMICOLON;}

"*"            {return MULTIPLY;}
"/"            {return DIVIDE;}
"+"            {return ADD;}
"-"            {return SUBTRACT;}

"="            {return EQUALS;}
"<>"           {return LESSGREATER;}
">"            {return GREATER;}
"<"            {return LESS;}
">="           {return GEQUAL;}
"<="           {return LEQUAL;}

"&"            {return AND;}
"|"            {return OR;}

"array"        {return ARRAY;}
"break"        {return BREAK;}
"do"           {return DO;}
"else"         {return ELSE;}
"end"          {return END;}
"for"          {return FOR;}
"function"     {return FUNCTION;}
"if"           {return IF;}
"in"           {return IN;}
"let"          {return LET;}
"nil"          {return NIL;}
"of"           {return OF;}
"then"         {return THEN;}
"to"           {return TO;}
"type"         {return TYPE;}
"var"          {return VAR;}
"while"        {return WHILE;}

{DIGIT}+       { yylval.iValue = atoi(yytext);
                         return INTLIT;}
               
{ID}           { yylval.sValue = new string(yytext);
                         return ID;}

{STRLIT}       {yylval.sValue = new string(yytext); return STRLIT;};

.              { cout << "ERROR: " << lineNumber << ": Lexer: Unexpected token (" << yytext << ") \n";
               exit(1);}

<COMMENT>"*/"  {BEGIN (0);}
<COMMENT>\n    {lineNumber++;}
<COMMENT>.     {}


<<EOF>>        {if(YY_START == COMMENT)
{
     std::cout << "ERROR: " << lineNumber << ": Lexer: Unterminated comment starting at line " << commentLineStart << ".\n";
               exit(1);
}
else
return 0;}








%%