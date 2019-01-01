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
    Identifier ide1 = identifiers.at(a);
    Identifier ide2 = identifiers.at(b);
    initError(ide1, a, yylineno);
    initError(ide2, b, yylineno);

    resetRegister("G");
    assignRegister("C", ide1);
    assignRegister("D", ide2);

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

    DEBUG_MSG("Porównano: " << ide1.name << " == " << ide2.name);
}

void __condNotEq(char* a, char* b, int yylineno) {
    Identifier ide1 = identifiers.at(a);
    Identifier ide2 = identifiers.at(b);
    initError(ide1, a, yylineno);
    initError(ide2, b, yylineno);

    resetRegister("G");
    assignRegister("C", ide1);
    assignRegister("D", ide2);
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

    DEBUG_MSG("Porównano: " << ide1.name << " != " << ide2.name);
}

void __condLowEq(char* a, char* b, int yylineno) {
    Identifier ide1 = identifiers.at(a);
    Identifier ide2 = identifiers.at(b);
    initError(ide1, a, yylineno);
    initError(ide2, b, yylineno);

    resetRegister("G");
    assignRegister("C", ide1);
    assignRegister("D", ide2);
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

    DEBUG_MSG("Porównano: " << ide1.name << " <= " << ide2.name);
}

void __condGreEq(char* a, char* b, int yylineno) {
    Identifier ide1 = identifiers.at(a);
    Identifier ide2 = identifiers.at(b);
    initError(ide1, a, yylineno);
    initError(ide2, b, yylineno);

    resetRegister("G");
    assignRegister("C", ide1);
    assignRegister("D", ide2);
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

    DEBUG_MSG("Porównano: " << ide1.name << " >= " << ide2.name);
}

void __condLow(char* a, char* b, int yylineno) {
    Identifier ide1 = identifiers.at(a);
    Identifier ide2 = identifiers.at(b);
    initError(ide1, a, yylineno);
    initError(ide2, b, yylineno);

    resetRegister("G");
    assignRegister("C", ide1);
    assignRegister("D", ide2);
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

    DEBUG_MSG("Porównano: " << ide1.name << " > " << ide2.name);
}

void __condGre(char* a, char* b, int yylineno) {
    Identifier ide1 = identifiers.at(a);
    Identifier ide2 = identifiers.at(b);
    initError(ide1, a, yylineno);
    initError(ide2, b, yylineno);

    resetRegister("G");
    assignRegister("C", ide1);
    assignRegister("D", ide2);
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

    DEBUG_MSG("Porównano: " << ide1.name << " > " << ide2.name);
}