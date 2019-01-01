/* Copyright (C) 2018 Maciej Hajduk - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the Beerware license, which unfortunately won't
 * be written for another century.
 *
 * You should have received a copy of the Beerware license
 * with this file. If not, please write to m.hajduk(at)pm.me
 */

%option noyywrap
%option yylineno
%{
#include <iostream>
#include <string>
#include <vector>

#include "parser.tab.h"

%}

%x COMMENT
%option noyywrap

%%

"["             { BEGIN(COMMENT); }
<COMMENT>"]"       { BEGIN(INITIAL); }
<COMMENT>(\n|.)
<COMMENT><<EOF>>   { printf("Error: infinite comment.\n"); yyterminate(); }

"DECLARE"		return _declare;
"IN"		    return _in;
"END"		    return _end;

":="		    return _assign;

"IF"		    return _if;
"THEN"		    return _then;
"ELSE"		    return _else;
"ENDIF"		    return _endif;

"WHILE"		    return _while;
"DO"		    return _do;
"ENDWHILE"	    return _endwhile;
"ENDDO"	        return _enddo;

"FOR"		    return _for;
"FROM"		    return _from;
"TO"		    return _to;
"DOWNTO"	    return _downto;
"ENDFOR"	    return _endfor;

"WRITE"		    return _write;
"READ"		    return _read;

"="             return _eq;
"<"             return _l;
">"             return _g;
"<="            return _le;
">="            return _ge;
"!="            return _ne;

"+"             return _add;
"-"             return _sub;
"*"             return _mul;
"/"             return _div;
"%"             return _mod;
"("             return _lb;
")"             return _rb;

[0-9]+          { yylval.str = (char *)strdup(yytext); return _number; }
[_a-z]+         { yylval.str = (char *)strdup(yytext); return _identifier; }

";"             { return _sem; }
":"             { return _col; }

[ \t\r\n]+

%%