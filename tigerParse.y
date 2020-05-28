%{
     #include <stdlib.h>
     #include <stdio.h>
     #include <string>
     #include <iostream>
     #include <vector>
     #include "ast.h"
     #include "SemanticAnalyzer.h"
     #include "Interpreter.h"
     
     using namespace std;

     extern int yylex();
     extern int yyparse();
     extern FILE* yyin;
     extern int lineNumber;
     int tempLineNumber;

     void yyerror (const char *error);
     node *ast;
%}

%require "2.4.1"

%code requires{
     class node;
}

%start program

%error-verbose

%union{
     int iValue;
     std::string *sValue;
     node *Node;
     std::vector<node*> *listNode;
};

%type <Node> program exp lValue BREAK NIL seqExp negation callExp infixExp arrCreate 
%type <Node> fieldCreate recCreate assignment ifThenElse ifThen whileExp forExp letExp
%type <Node> tyDec funDec dec ty arrTy recTy fieldDec varDec
%type <listNode> exps fieldCreates decs fieldDecs
%type <sValue> ID
%token<iValue> INTLIT
%token<sValue> STRLIT
%token RBRACK LBRACE RBRACE COLON PERIOD COMMA SEMICOLON  RPAREN

%token ARRAY BREAK ELSE END FOR IF IN LET NIL THEN TO VAR WHILE

%nonassoc TYPE FUNCTION
%nonassoc ID
%nonassoc LBRACK
%nonassoc THEN
%nonassoc ELSE DO LPAREN
%left ASSIGNMENT OF
%left OR AND
%nonassoc EQUALS LESSGREATER GREATER LESS GEQUAL LEQUAL
%left SUBTRACT ADD
%left DIVIDE MULTIPLY
%right UNARY

%%

program:
     exp {$$ = new program(lineNumber, $1); ast = $$; }
     ;
     
exps: /* epsilon */ { $$ = new std::vector<node*>();}
     |   exp { $$ = new std::vector<node*>(); $$->push_back($1);}
     |   exps SEMICOLON exp { $$->push_back($3);}
     |   exps COMMA exp { $$->push_back($3); }
     ;
     
exp: 
     lValue {$$ = $1;}
     | NIL {$$ = new NNil(lineNumber);}
     | INTLIT {$$ = new NIntLit(lineNumber, $1);}
     | STRLIT {$$ = new NStrLit(lineNumber, $1);}
     | seqExp {$$ = $1;}
     | negation {$$ = $1;}
     | callExp {$$ = $1;}
     | infixExp {$$ = $1;}
     | arrCreate {$$ = $1;}
     | recCreate{$$ = $1;}
     | assignment {$$ = $1;}
     | ifThenElse {$$ = $1;}
     | ifThen {$$ = $1;}
     | whileExp {$$ = $1;}
     | forExp {$$ = $1;}
     | BREAK {$$ = new NBreak(lineNumber);}
     | letExp {$$ = $1;}
     ;


     
decs: /* one or more */
     dec { $$ = new std::vector<node*>();
          $$->push_back($1);}
     | decs dec {$$->push_back($2);}
     ;
 
dec:
     tyDec {$$ = $1;}
     | varDec {$$ = $1;}
     | funDec {$$ = $1;}
     ;

tyDec:
     TYPE ID EQUALS ty {$$ = new tyDec(lineNumber, new NTyId(lineNumber, $2), $4);}
     ;
    
ty:
     ID {$$ = new refTy(lineNumber, new NTyId(lineNumber, $1));}
     | arrTy {$$ = $1;}
     | recTy {$$ = $1;}
     ;
     
arrTy:
     ARRAY OF ID {$$ = new arrTy(lineNumber, new NTyId(lineNumber, $3));}
     ;
     
recTy:
     LBRACE  fieldDecs RBRACE {$$ = new recTy(lineNumber, $2);}
     ;
     
fieldDecs: /* epsilon */ {$$ = new std::vector<node*>();}
     | fieldDecs fieldDec {$$->push_back($2);}
     | fieldDecs COMMA fieldDec {$$->push_back($3);}
     ;
     
fieldDec:
     ID COLON ID {$$ = new fieldDec(lineNumber, new NId(lineNumber, $1), new NTyId(lineNumber, $3));}
     ;
     
funDec:
     FUNCTION ID LPAREN fieldDecs RPAREN COLON ID EQUALS exp {
          $$ = new funDec(lineNumber, new NId(lineNumber, $2), $4, new NTyId(lineNumber, $7), $9);}
     ;

varDec:
     VAR ID COLON ID ASSIGNMENT exp {$$ = new varDec(lineNumber, new NId(lineNumber, $2), new NTyId(lineNumber, $4), $6);}
     ;
     
lValue:
     ID {$$ = new NId(lineNumber, $1);}
     | lValue PERIOD ID  {$$ = new fieldExp(lineNumber, $1, new NId(lineNumber, $3)); /*field Exp*/}
     | lValue LBRACK exp RBRACK {$$ = new subscript(lineNumber, $1, $3); /* subscript */}
     | ID LBRACK exp RBRACK {$$ = new subscript(lineNumber, new NId(lineNumber, $1), $3); /*subscript to fix shift/reduce */}
     ;

      
seqExp:
     LPAREN exps RPAREN {$$ = new seqExp(lineNumber, $2);}
     ;
     
negation:
     SUBTRACT exp %prec UNARY {$$ = new negation(lineNumber, $2);}
     ;
     
callExp:
     ID LPAREN exps RPAREN {$$ = new callExp(lineNumber, new NId(lineNumber, $1),$3);}
     ;
     
infixExp:
     exp AND exp {$$ = new infixExp(lineNumber, OP_AND, $1, $3);}
     | exp OR exp {$$ = new infixExp(lineNumber, OP_OR, $1, $3);}
     | exp EQUALS exp {$$ = new infixExp(lineNumber, OP_EQ, $1, $3);}
     | exp LESSGREATER exp {$$ = new infixExp(lineNumber, OP_NEQ, $1, $3);}
     | exp GREATER exp {$$ = new infixExp(lineNumber, OP_GT, $1, $3);}
     | exp LESS exp {$$ = new infixExp(lineNumber, OP_LT, $1, $3);}
     | exp GEQUAL exp {$$ = new infixExp(lineNumber, OP_GEQ, $1, $3);}
     | exp LEQUAL exp {$$ = new infixExp(lineNumber, OP_LEQ, $1, $3);}
     | exp ADD exp {$$ = new infixExp(lineNumber, OP_ADD, $1, $3);}
     | exp SUBTRACT exp {$$ = new infixExp(lineNumber, OP_SUBTRACT, $1, $3);}
     | exp MULTIPLY exp {$$ = new infixExp(lineNumber, OP_MULTIPLY, $1, $3);}
     | exp DIVIDE exp {$$ = new infixExp(lineNumber, OP_DIVIDE, $1, $3);}
     ;
     
arrCreate:
     ID LBRACK exp RBRACK OF exp {$$ = new arrCreate(lineNumber, new NTyId(lineNumber, $1), $3, $6);}
     ;
     
recCreate:
     ID LBRACE fieldCreates RBRACE {$$ = new recCreate(lineNumber, new NTyId(lineNumber, $1), $3);}
     ;
     
fieldCreates: /*zero or more */  { $$ = new std::vector<node*>();}
     | fieldCreates fieldCreate {$$->push_back($2);}
     | fieldCreates COMMA fieldCreate  {$$->push_back($3);}
     ;


fieldCreate:
     ID EQUALS exp {$$ = new fieldCreate(lineNumber, new NId(lineNumber, $1), $3);}
     ;
     
assignment:
     lValue ASSIGNMENT exp {$$ = new assignment(lineNumber, $1, $3);}
     ;
     
ifThenElse:
     IF exp THEN exp ELSE exp {$$ = new ifThenElse(lineNumber, $2,$4,$6);} 
     ;
     
ifThen:
     IF exp THEN exp {$$ = new ifThenElse(lineNumber, $2,$4, NULL);}
     ;
     
whileExp:
     WHILE exp DO exp {$$ = new whileExp(lineNumber, $2,$4);}
     ;
     
forExp:
     FOR ID ASSIGNMENT exp TO exp DO exp {$$ = new forExp(lineNumber, new NId(lineNumber, $2), $4, $6, $8);}
     ;
    
letExp:
     LET decs IN exps END {$$ = new letExp(lineNumber, $2, $4);}
     ;
     
%%

/*
     Created by: Braden Luancing
     Major: Computer Science
     Creation Date: 10/2/19
     Due Date: ?/?/19
     Course: CSC425
     Professor: Dr. Schwesinger
     Assignment: #3
     Filename: tigerParse.y
     Purpose: Parses source code for the Tiger language. 
     Returns 0 if parsing was succesful; returns 1 for
     lexical error; returns 2 for parsing error.

*/


#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>

using namespace std;

int main( int argc, char *argv[] )
	{
          //Exit if arguments not found
          if(argc != 2)
          {
               cerr << "ERROR: Insufficient arguments. Please specify a file to scan." << endl; 
               return 1;
          }

          //Try to get the file from the user
          //ifstream file;
          //file.open(argv[1]);
          FILE *file = fopen(argv[1], "r");
          if(!file)
          {
               cerr << "ERROR: Failed to open target file '"<< argv[1] << "'. Confirm that the file exists or the name is correct." << endl;
               return -1;
          }
          
          //Construct a lexer with the file as the input stream
	     yyin = file;
          
          
          //Begin parsing, and grab return code
          int success = yyparse();
          
          //These codes are based off the possible return
          // values from yyparse() based off Bison's manual
          if(success == 0)
          {
               //nodePrinter printer;
               //printer.print(ast);
               
               SemanticAnalyzer* semanticAnalyzer = new SemanticAnalyzer(ast);
               semanticAnalyzer->AnalyzeTree();

               Interpreter* interpreter = new Interpreter(ast);
               interpreter->Interpret();
               return 0;
          }
          else if(success == 1)
          {
               cout << "File lexically invalid." << endl;
               return 1;
          }
          else if(success == 2)
          {
               cout << "File syntactically invalid." << endl;
               return 2;
          }

	}

/*
     Function name: yyerror()
     Description:   Called by Bison when it detects a
                    parsing error. Outputs helpful error message.
     Parameters:    
     
     const char *s: error message from Bison
     
     Return: none

*/
void yyerror(const char *s) {
  cerr << "ERROR: " << lineNumber << ": Parser: " << s << endl;
  exit(2);
}