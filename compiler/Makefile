CC = g++
CFLAGS = -std=c++11 #-DDEBUG
SRC = ./src
OUT = ../out/compiler
FILES = lex.yy.c parser.tab.c compiler.cpp expressions.cpp conditions.cpp

RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
$(eval $(RUN_ARGS):;@:)

all: compiler

parser: $(SRC)/parser.y
	cd $(SRC) && bison -t -d parser.y

lexer: $(SRC)/lexer.l
	cd $(SRC) && flex lexer.l

compiler: lexer parser
	mkdir -p out
	test -f ../interpreter/maszyna-rejestrowa || make interpreter
	cd $(SRC) && $(CC) $(CFLAGS) -o $(OUT) $(FILES)
	make clean

interpreter:
	cd ../interpreter && make maszyna-rejestrowa
	cd ../interpreter && make clean

debug: compiler
	./out/compiler programs/$(RUN_ARGS) out/out
	../interpreter/maszyna-rejestrowa out/out

test: compiler
	./out/compiler ./programs/tests/test$(RUN_ARGS) out/out
	../interpreter/maszyna-rejestrowa out/out

clean:
	cd $(SRC) && rm -f lex.yy.c parser.tab.c parser.tab.h

cleanall: clean
	cd $(SRC) && rm -f ../out/compiler