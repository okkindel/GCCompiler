/* Copyright (C) 2018 Maciej Hajduk - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the Cofeeware license, which unfortunately won't
 * be written for another century.
 *
 * You should have received a copy of the Cofeeware license
 * with this file. If not, please write to m.hajduk(at)pm.me
 */

%{
#include "compiler.hpp"

extern int yylex();
extern int yylineno;
extern FILE *yyin;
int yyerror(const string str);
%}

%define parse.error verbose
%expect 2

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

    _declare declarations                                               { insertTabs(); }
        _in commands _end                                               { insert("HALT"); }
    ;

declarations:

    declarations _identifier _sem                                       { __declareIde($2, yylineno); }
    | declarations _identifier _lb _number _col _number _rb _sem        { __declareTab($2, $4, $6, yylineno); }
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
    | _while                                                            { __begin_while(); } 
        condition _do commands _endwhile                                { __end_while(); }
    | _do                                                               { __begin_while(); } 
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
    | value _add value                              { __expressionAdd($1, $3, yylineno); }
    | value _sub value                              { __expressionSub($1, $3, yylineno); }
    | value _mul value                              { __expressionMul($1, $3, yylineno); }
    | value _div value                              { __expressionDiv($1, $3, yylineno); }
    | value _mod value                              { __expressionMod($1, $3, yylineno); }
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
    | _identifier _lb _identifier _rb               { __ideIdeIde($1, $3, yylineno); }
    | _identifier _lb _number _rb                   { __ideIdeNum($1, $3, yylineno); }
    ;

%%

void printLogo() {
    cout << endl;
    cout << "\033[;36m _____ \033[0m    _      \033[;31m __   \033[0m     ver. 3.0" << endl;
    cout << "\033[;36m|   __|\033[0m___| |_ ___\033[;31m|  |  \033[0m ___ ___ ___ " << endl;
    cout << "\033[;36m|  |  |\033[0m -_| . | .'\033[;31m|  |__\033[0m| .'|   | . |" << endl;
    cout << "\033[;36m|_____|\033[0m___|___|__,\033[;31m|_____\033[0m|__,|_|_|_  |" << endl;
    cout << "        ~                       |___|" << endl;
    cout << endl;
}

int main(int argv, char* argc[]) {
    printLogo();
    if( argv != 3 ) {
        cerr << "\e[1m[ USAGE ]\e[0m compiler input output" << endl;
        return 1;
    }

    string msg =". .. ... ";
    for (const char c: msg) {
        cout << c << flush;
        usleep(80000);
    }

    yyin = fopen(argc[1], "r");
    if (yyin == NULL)
        error(argc[1], 0, "File does not exist:");
    createRegisters();
	yyparse();
    optymize();
    print(argc[2]);
    cout << "Compiled without errors •ᴗ•\n" << endl;
	return 0;
}

int yyerror(string err) {
    cout << "(╯°□°）╯︵ ┻━┻\n\n";
    usleep(500000);
    cout << "\e[1m\x1B[31m[ ERROR ]\e[0m \e[1m[ LINE " << yylineno << " ] \e[1m\x1B[31m" << err << ".\e[0m\n" << endl;
    exit(1);
}
