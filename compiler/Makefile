
all: compiler

bison: bison.y
	bison -t -d bison.y

lexer: lexer.l
	flex lexer.l

compiler: lexer bison
	g++ -std=c++11 -o compiler lex.yy.c bison.tab.c

clean:
	rm -f compiler lex.yy.c bison.tab.c bison.tab.h