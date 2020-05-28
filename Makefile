# Created by Braden Luancing
# Purpose: Used to generate a lexer/parser for
#          the Tiger language
COMP=/opt/gcc6/bin/g++


all: tigerc clean

tigerc: tigerParse.tab.c tigerParse.tab.h lex.yy.c ast.o SemanticAnalyzer.o SymbolTable.o Interpreter.o
	$(COMP) -std=c++11 -ggdb -c lex.yy.c
	$(COMP) -std=c++11 -ggdb -c tigerParse.tab.c 
	$(COMP) -std=c++11 -ggdb lex.yy.o tigerParse.tab.o ast.o SemanticAnalyzer.o SymbolTable.o Interpreter.o -lfl -o tigerc

tigerParse.tab.c tigerParse.tab.h: tigerParse.y
	bison -d --verbose tigerParse.y

lex.yy.c : tigerLex.l
	flex tigerLex.l

ast.o: ast.h ast.cpp
	$(COMP) -std=c++11 -ggdb -c ast.cpp

SemanticAnalyzer.o: SemanticAnalyzer.h SemanticAnalyzer.cpp
	$(COMP) -std=c++11 -ggdb -c SemanticAnalyzer.cpp

SymbolTable.o: SymbolTable.h SymbolTable.cpp
	$(COMP) -std=c++11 -ggdb -c SymbolTable.cpp

Interpreter.o: Interpreter.h Interpreter.cpp
	$(COMP) -std=c++11 -ggdb -c Interpreter.cpp

clean:
	rm lex.yy.* tigerParse.tab.* ast.o SemanticAnalyzer.o SymbolTable.o

test:
	/opt/anaconda3/bin/python test_runner.py