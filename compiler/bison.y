%{
#include "compiler.hpp"

extern int yylex();
extern int yylineno;
int yyerror(const string str);

%}

%define parse.error verbose

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

    _declare declarations _in commands _end                             { insert("HALT"); }
    ;

declarations:

    declarations _identifier _sem                                       { __declareIde($2, yylineno); }
    | declarations _identifier _lb _number _col _number _rb _sem        { cout << "tab ( num : num )" << endl; }
    |
    ;

commands:

    commands command                                                    { }
    | command                                                           { }
    ;

command:

    identifier _assign expression _sem              { __assign($1, yylineno); }
    | _if condition _then commands if                       
    | _while condition _do commands _endwhile       { cout << "while" << endl; }
    | _do commands _while condition _enddo          { cout << "do" << endl; }
    | _for _identifier _from value for              
    | _read identifier _sem                         { __read($2, yylineno); }
    | _write value _sem                             { __write($2, yylineno); }
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

    value                                           { __expressionVal($1, yylineno); }
    | value _add value                              { __add($1, $3); }
    | value _sub value                              { __sub($1, $3); }
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

    _number                                         { __valueNum($1, yylineno); }
    | identifier
    ;

identifier:

    _identifier                                     { __ide($1, yylineno); }
    | _identifier _lb _identifier _rb               { cout << "ide ( ide )" << endl; }
    | _identifier _lb _number _rb                   { cout << "ide ( num )" << endl; }
    ;

%%

int main(int argv, char* argc[]) {
	yyparse();
    print();
	return 0;
}

int yyerror(string err) {
    cout << "\e[1mBłąd na lini " << yylineno << \
    ", konkretnie: \e[1m\x1B[31m" << err << ".\e[0m\n" << endl;
    exit(1);
}
