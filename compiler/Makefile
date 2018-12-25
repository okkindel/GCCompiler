
all: compiler

bison: bison.y
	bison -t -d bison.y

lexer: lexer.l
	flex lexer.l

compiler: lexer bison
	g++ -std=c++11 -lm -o compiler lex.yy.c bison.tab.c compiler.cpp
	rm -f lex.yy.c bison.tab.c bison.tab.h

clean:
	rm -f compiler lex.yy.c bison.tab.c bison.tab.h