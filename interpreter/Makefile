FLAGS = -W -pedantic -std=c++17 -O3

.PHONY = all clean cleanall

all: maszyna-rejestrowa

maszyna-rejestrowa: lexer.o parser.o mr.o main.o
	$(CXX) $^ -o $@
	strip $@

%.o: %.cc
	$(CXX) $(FLAGS) -c $^

lexer.cc: lexer.l parser.hh
	flex -o $@ $<

parser.cc parser.hh: parser.y
	bison -Wall -d -o parser.cc $^

clean:
	rm -f *.o parser.cc parser.hh lexer.cc

cleanall: clean
	rm -f maszyna-rejestrowa maszyna-rejestrowa-cln
