CC = g++
CFLAGS = -std=c++11 #-DDEBUG
SRC = ./out/compiler

all: compiler

bison: bison.y
	bison -t -d bison.y

lexer: lexer.l
	flex lexer.l

compiler: lexer bison
	mkdir -p out
	$(CC) $(CFLAGS) -o $(SRC) lex.yy.c bison.tab.c compiler.cpp
	make clean

interpreter:
	cd ../interpreter && make maszyna-rejestrowa
	cd ../interpreter && make clean

debug: compiler
	test -f ../interpreter/maszyna-rejestrowa || make interpreter
	./out/compiler ./programs/alghoritms/binary.in out/out
	../interpreter/maszyna-rejestrowa ./out/out

test: compiler
	test -f ../interpreter/maszyna-rejestrowa || make interpreter
	./out/compiler ./programs/tests/test$(num) out/out
	../interpreter/maszyna-rejestrowa ./out/out

clean:
	rm -f lex.yy.c bison.tab.c bison.tab.h

cleanall: clean
	rm -f ./out/compiler