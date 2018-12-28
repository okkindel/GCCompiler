%{
#include "compiler.hpp"

extern int yylex();
extern int yylineno;
extern FILE *yyin;
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

    identifier _assign expression _sem                                  { __cmdAssign($1, yylineno); }
    | _if condition _then commands _endif                               { __end_if(); }
    | _if condition _then commands                                      { __if_else(); } 
        _else commands _endif                                           { __end_if(); } 
    | _while                                                            { __begin_loop(); } 
        condition _do commands _endwhile                                { __end_while(); }
    | _do                                                               { __begin_loop(); } 
        commands _while condition _enddo                                { __end_do(); }
    | _for _identifier _from value _downto value _do                    { __for($2, $4, $6, yylineno); } 
        commands _endfor                                                { __end_down_for(); }
    | _for _identifier _from value _to value _do                        { __for($2, $4, $6, yylineno); } 
        commands _endfor                                                { __end_up_for(); }
    | _read identifier _sem                                             { __cmdRead($2, yylineno); }
    | _write value _sem                                                 { __cmdWrite($2, yylineno); }
    ;

expression:

    value                                           { __expressionVal($1, yylineno); }
    | value _add value                              { __expressionAdd($1, $3); }
    | value _sub value                              { __expressionSub($1, $3); }
    | value _mul value                              { __expressionMul($1, $3); }
    | value _div value                              { __expressionDiv($1, $3); }
    | value _mod value                              { __expressionMod($1, $3); }
    ;

condition:

    value _eq value                                 { __condEq($1, $3, yylineno); }
    | value _ne value                               { __condNotEq($1, $3, yylineno); }
    | value _l value                                { __condLow($1, $3, yylineno); }
    | value _g value                                { __condGre($1, $3, yylineno); }
    | value _le value                               { __condLowEq($1, $3, yylineno); }
    | value _ge value                               { __condGreEq($1, $3, yylineno); }
    ;

value:

    _number                                         { __valueNum($1, yylineno); }
    | identifier
    ;

identifier:

    _identifier                                     { __ideIdetifier($1, yylineno); }
    | _identifier _lb _identifier _rb               { cout << "ide ( ide )" << endl; }
    | _identifier _lb _number _rb                   { cout << "ide ( num )" << endl; }
    ;

%%

int main(int argv, char* argc[]) {
    if( argv != 3 ) {
        cerr << "\e[1m\x1B[31mUSAGE:\e[0m ./compiler input output" << endl;
        return -1;
    }
    yyin = fopen(argc[1], "r");
	yyparse();
    print(argc[2]);
	return 0;
}

int yyerror(string err) {
    cout << "\e[1mBłąd na lini " << yylineno << \
    ", konkretnie: \e[1m\x1B[31m" << err << ".\e[0m\n" << endl;
    exit(1);
}
