%{
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
using namespace std;

// STRUKTURY

int yylex();
extern int yylineno;
int yyerror(const string str);

// insert single command
void insert(string str);
// insert single command with registry
void insert(string str, string reg);
// insert double command with registry and number
void insert(string str, string reg, long long int num);


%}

%define parse.error verbose
%define parse.lac full

%union {
    char* str;
    long long int num;
}

%token <str> _number
%token <str> _declare _in _end _if _then _else _endif
%token <str> _while _do _endwhile _enddo _for _from _endfor
%token <str> _col _write _read _identifier _sem _to _downto
%token <str> _lb _rb _assign _eq _l _g _le _ge _ne _add _sub _mul _div _mod

%type <str> value
%type <str> identifier

%%
program:

    _declare declarations _in commands _end { insert("HALT"); }
    ;

declarations:

    declarations _identifier _sem                                 { cout << "declaration" << endl; }
    | declarations _identifier _lb _number _col _number _rb _sem  { cout << "tab ( num : num )" << endl; }
    |
    ;

commands:

    commands command
    | command
    ;

command:

    identifier _assign expression _sem              { cout << "asign" << endl; }
    | _if condition _then commands if               
    | _while condition _do commands _endwhile       { cout << "while" << endl; }
    | _do commands _while condition _enddo          { cout << "do" << endl; }
    | _for _identifier _from value for
    | _read identifier _sem                         { cout << "read" << endl; }
    | _write value _sem                             { cout << "write" << endl; }
    ;

if:

    _else commands _endif                           { cout << "if 1" << endl; }
    | _endif                                        { cout << "if 2" << endl; }
    ;

for:

    _downto value _do commands _endfor              { cout << "for1" << endl; }
    | _to value _do commands _endfor                { cout << "for2" << endl; }
    ;

expression:

    value                                           { cout << "value" << endl; }
    | value _add value                              { cout << "add" << endl; }
    | value _sub value                              { cout << "sub" << endl; }
    | value _mul value                              { cout << "mul" << endl; }
    | value _div value                              { cout << "div" << endl; }
    | value _mod value                              { cout << "mod" << endl; }
    ;

condition:

    value _eq value                                 { cout << "eq" << endl; }
    | value _ne value                               { cout << "ne" << endl; }
    | value _l value                                { cout << "low" << endl; }
    | value _g value                                { cout << "gre" << endl; }
    | value _le value                               { cout << "le" << endl; }
    | value _ge value                               { cout << "ge" << endl; }
    ;

value:

    _number                                         { cout << "num" << endl; }
    | identifier
    ;

identifier:

    _identifier                                     { cout << "ide" << endl; }
    | _identifier _lb _identifier _rb               { cout << "ide ( ide )" << endl; }
    | _identifier _lb _number _rb                   { cout << "ide ( num )" << endl; }
    ;

%%

void insert(string str) {
    cout << str << endl;
}
void insert(string str, string reg) {
    cout << str << reg << endl;
}

void insert(string str, string reg, long long int num) {
    cout << str << reg << num << endl;
}

void parser(long long int argv, char* argc[]) {
	yyparse();
}

int main(int argv, char* argc[]) {
	parser(argv, argc);
	return 0;
}

int yyerror(string err) {
    cout << "\e[1mGeneralnie, to zjebałeś na całej lini " << yylineno << \
    ", konkretnie: \e[1m\x1B[31m" << err << ".\e[0m\n" << endl;
    exit(1);
}
