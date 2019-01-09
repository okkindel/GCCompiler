/* Copyright (C) 2018 Maciej Hajduk - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the Cofeeware license, which unfortunately won't
 * be written for another century.
 *
 * You should have received a copy of the Cofeeware license
 * with this file. If not, please write to m.hajduk(at)pm.me
 */

#include "compiler.hpp"

void __condEq(char* a, char* b, int yylineno) {
    Variable var1 = variables.at(a);
    Variable var2 = variables.at(b);
    initError(var1, a, yylineno);
    initError(var2, b, yylineno);

    resetRegister("G");
    assignRegister("C", var1, "D", var2);

    insert("COPY", "E", "C");
    insert("SUB", "E", "D");
    insert("JZERO", "E", cmdIndex + 2);
    insert("JUMP", cmdIndex + 5);
    insert("SUB", "D", "C"); 
    insert("JZERO", "D", cmdIndex + 2);
    insert("JUMP", cmdIndex + 2);
    insert("INC", "G");

    createJump();
    insert("JZERO", "G", "$bookmark");
}

void __condNotEq(char* a, char* b, int yylineno) {
    Variable var1 = variables.at(a);
    Variable var2 = variables.at(b);
    initError(var1, a, yylineno);
    initError(var2, b, yylineno);

    resetRegister("G");
    assignRegister("C", var1, "D", var2);

    insert("COPY", "E", "C");
    insert("SUB", "E", "D");
    insert("JZERO", "E", cmdIndex + 2);
    insert("INC", "G");

    insert("SUB", "D", "C"); 
    insert("JZERO", "D", cmdIndex + 2);
    insert("INC", "G");

    createJump();
    insert("JZERO", "G", "$bookmark");
}

void __condLowEq(char* a, char* b, int yylineno) {
    Variable var1 = variables.at(a);
    Variable var2 = variables.at(b);
    initError(var1, a, yylineno);
    initError(var2, b, yylineno);

    resetRegister("G");
    assignRegister("C", var1, "D", var2);

    insert("COPY", "E", "D");
    insert("SUB", "E", "C");
    insert("JZERO", "E", cmdIndex + 3);
    insert("INC", "G");
    insert("JUMP", cmdIndex + 5);
    insert("SUB", "C", "D"); 
    insert("JZERO", "C", cmdIndex + 2);
    insert("JUMP", cmdIndex + 2);
    insert("INC", "G");

    createJump();
    insert("JZERO", "G", "$bookmark");
}

void __condGreEq(char* a, char* b, int yylineno) {
    Variable var1 = variables.at(a);
    Variable var2 = variables.at(b);
    initError(var1, a, yylineno);
    initError(var2, b, yylineno);

    resetRegister("G");
    assignRegister("C", var1, "D", var2);

    insert("COPY", "E", "C");
    insert("SUB", "E", "D");
    insert("JZERO", "E", cmdIndex + 3);
    insert("INC", "G");
    insert("JUMP", cmdIndex + 5);
    insert("SUB", "D", "C"); 
    insert("JZERO", "D", cmdIndex + 2);
    insert("JUMP", cmdIndex + 2);
    insert("INC", "G");

    createJump();
    insert("JZERO", "G", "$bookmark");
}

void __condLow(char* a, char* b, int yylineno) {
    Variable var1 = variables.at(a);
    Variable var2 = variables.at(b);
    initError(var1, a, yylineno);
    initError(var2, b, yylineno);

    resetRegister("G");
    assignRegister("C", var1, "D", var2);

    insert("COPY", "E", "C");
    insert("SUB", "E", "D");
    insert("JZERO", "E", cmdIndex + 2);
    insert("JUMP", cmdIndex + 4);
    insert("SUB", "D", "C");
    insert("JZERO", "D", cmdIndex + 2);
    insert("INC", "G");

    createJump();
    insert("JZERO", "G", "$bookmark");
}

void __condGre(char* a, char* b, int yylineno) {
    Variable var1 = variables.at(a);
    Variable var2 = variables.at(b);
    initError(var1, a, yylineno);
    initError(var2, b, yylineno);

    resetRegister("G");
    assignRegister("C", var1, "D", var2);

    insert("COPY", "E", "D");
    insert("SUB", "E", "C");
    insert("JZERO", "E", cmdIndex + 2);
    insert("JUMP", cmdIndex + 4);
    insert("SUB", "C", "D");
    insert("JZERO", "C", cmdIndex + 2);
    insert("INC", "G");

    createJump();
    insert("JZERO", "G", "$bookmark");
}