CC = g++
CFLAGS = -std=c++11 #-DDEBUG
SRC = ./src
OUT = ../out/compiler
FILES = lex.yy.c parser.tab.c compiler.cpp expressions.cpp conditions.cpp

all: compiler

parser: $(SRC)/parser.y
	cd $(SRC) && bison -t -d parser.y

lexer: $(SRC)/lexer.l
	cd $(SRC) && flex lexer.l

compiler: lexer parser
	mkdir -p out
	cd $(SRC) && $(CC) $(CFLAGS) -o $(OUT) $(FILES)
	make clean

interpreter:
	cd ../interpreter && make maszyna-rejestrowa
	cd ../interpreter && make clean

debug: compiler
	test -f ../interpreter/maszyna-rejestrowa || make interpreter
	./out/compiler programs/alghoritms/8-for.imp out/out
	../interpreter/maszyna-rejestrowa out/out

test: compiler
	test -f ../interpreter/maszyna-rejestrowa || make interpreter
	./out/compiler ./programs/tests/test$(num) out/out
	../interpreter/maszyna-rejestrowa out/out

clean:
	cd $(SRC) && rm -f lex.yy.c parser.tab.c parser.tab.h

cleanall: clean
	cd $(SRC) && rm -f ./out/compiler