/* Copyright (C) 2018 Maciej Hajduk - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the Beerware license, which unfortunately won't
 * be written for another century.
 *
 * You should have received a copy of the Beerware license
 * with this file. If not, please write to m.hajduk(at)pm.me
 */

#include "compiler.hpp"

void __condEq(char* a, char* b, int yylineno) {
    Variable var1 = variables.at(a);
    Variable var2 = variables.at(b);
    initError(var1, a, yylineno);
    initError(var2, b, yylineno);

    resetRegister("G");
    assignRegister("C", var1);
    assignRegister("D", var2);

    insert("COPY", "E", "C");
    insert("SUB", "E", "D");
    insert("JZERO", "E", cmdIndex + 2);
    insert("JUMP", cmdIndex + 6);
    insert("COPY", "E", "D");
    insert("SUB", "E", "C"); 
    insert("JZERO", "E", cmdIndex + 2);
    insert("JUMP", cmdIndex + 2);
    insert("INC", "G");

    createJump();
    insert("JZERO", "G", "$bookmark");

    DEBUG_MSG("Porównano: " << var1.name << " == " << var2.name);
}

void __condNotEq(char* a, char* b, int yylineno) {
    Variable var1 = variables.at(a);
    Variable var2 = variables.at(b);
    initError(var1, a, yylineno);
    initError(var2, b, yylineno);

    resetRegister("G");
    assignRegister("C", var1);
    assignRegister("D", var2);

    insert("COPY", "E", "C");
    insert("SUB", "E", "D");
    insert("JZERO", "E", cmdIndex + 2);
    insert("INC", "G");

    insert("COPY", "E", "D");
    insert("SUB", "E", "C"); 
    insert("JZERO", "E", cmdIndex + 2);
    insert("INC", "G");

    createJump();
    insert("JZERO", "G", "$bookmark");

    DEBUG_MSG("Porównano: " << var1.name << " != " << var2.name);
}

void __condLowEq(char* a, char* b, int yylineno) {
    Variable var1 = variables.at(a);
    Variable var2 = variables.at(b);
    initError(var1, a, yylineno);
    initError(var2, b, yylineno);

    resetRegister("G");
    if (var1.type == "TAB" && var2.type == "TAB") {
        // as long as double tab, we have to switch registers
        assignRegister("D", var1);
        assignRegister("C", var2);
    } else {
        assignRegister("C", var1);
        assignRegister("D", var2);
    }

    insert("COPY", "E", "D");
    insert("SUB", "E", "C");
    insert("JZERO", "E", cmdIndex + 3);
    insert("INC", "G");
    insert("JUMP", cmdIndex + 6);
    insert("COPY", "E", "C");
    insert("SUB", "E", "D"); 
    insert("JZERO", "E", cmdIndex + 2);
    insert("JUMP", cmdIndex + 2);
    insert("INC", "G");

    createJump();
    insert("JZERO", "G", "$bookmark");

    DEBUG_MSG("Porównano: " << var1.name << " <= " << var2.name);
}

void __condGreEq(char* a, char* b, int yylineno) {
    Variable var1 = variables.at(a);
    Variable var2 = variables.at(b);
    initError(var1, a, yylineno);
    initError(var2, b, yylineno);

    resetRegister("G");
    if (var1.type == "TAB" && var2.type == "TAB") {
        // as long as double tab, we have to switch registers
        assignRegister("D", var1);
        assignRegister("C", var2);
    } else {
        assignRegister("C", var1);
        assignRegister("D", var2);
    }

    insert("COPY", "E", "C");
    insert("SUB", "E", "D");
    insert("JZERO", "E", cmdIndex + 3);
    insert("INC", "G");
    insert("JUMP", cmdIndex + 6);
    insert("COPY", "E", "D");
    insert("SUB", "E", "C"); 
    insert("JZERO", "E", cmdIndex + 2);
    insert("JUMP", cmdIndex + 2);
    insert("INC", "G");

    createJump();
    insert("JZERO", "G", "$bookmark");

    DEBUG_MSG("Porównano: " << var1.name << " >= " << var2.name);
}

void __condLow(char* a, char* b, int yylineno) {
    Variable var1 = variables.at(a);
    Variable var2 = variables.at(b);
    initError(var1, a, yylineno);
    initError(var2, b, yylineno);

    resetRegister("G");
    if (var1.type == "TAB" && var2.type == "TAB") {
        // as long as double tab, we have to switch registers
        assignRegister("D", var1);
        assignRegister("C", var2);
    } else {
        assignRegister("C", var1);
        assignRegister("D", var2);
    }

    insert("COPY", "E", "C");
    insert("SUB", "E", "D");
    insert("JZERO", "E", cmdIndex + 2);
    insert("JUMP", cmdIndex + 5);
    insert("COPY", "E", "D");
    insert("SUB", "E", "C");
    insert("JZERO", "E", cmdIndex + 2);
    insert("INC", "G");

    createJump();
    insert("JZERO", "G", "$bookmark");

    DEBUG_MSG("Porównano: " << var1.name << " > " << var2.name);
}

void __condGre(char* a, char* b, int yylineno) {
    Variable var1 = variables.at(a);
    Variable var2 = variables.at(b);
    initError(var1, a, yylineno);
    initError(var2, b, yylineno);

    resetRegister("G");
    if (var1.type == "TAB" && var2.type == "TAB") {
        // as long as double tab, we have to switch registers
        assignRegister("D", var1);
        assignRegister("C", var2);
    } else {
        assignRegister("C", var1);
        assignRegister("D", var2);
    }

    insert("COPY", "E", "D");
    insert("SUB", "E", "C");
    insert("JZERO", "E", cmdIndex + 2);
    insert("JUMP", cmdIndex + 5);
    insert("COPY", "E", "C");
    insert("SUB", "E", "D");
    insert("JZERO", "E", cmdIndex + 2);
    insert("INC", "G");

    createJump();
    insert("JZERO", "G", "$bookmark");

    DEBUG_MSG("Porównano: " << var1.name << " > " << var2.name);
}