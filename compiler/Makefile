
all: compiler

bison: bison.y
	bison -t -d bison.y

lexer: lexer.l
	flex lexer.l

compiler: lexer bison
	g++ -std=c++11 -lm -o compiler lex.yy.c bison.tab.c compiler.cpp
	make clean

interpreter:
	cd ../interpreter && make maszyna-rejestrowa
	cd ../interpreter && make clean

debug: compiler
	# test -f compiler || make compiler
	test -f ../interpreter/maszyna-rejestrowa || make interpreter
	./compiler < programs/in
	../interpreter/maszyna-rejestrowa ./programs/out

clean:
	rm -f lex.yy.c bison.tab.c bison.tab.h

cleanall:
	rm -f compiler lex.yy.c bison.tab.c bison.tab.h