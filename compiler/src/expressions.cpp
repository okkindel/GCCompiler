/* Copyright (C) 2018 Maciej Hajduk - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the Cofeeware license, which unfortunately won't
 * be written for another century.
 *
 * You should have received a copy of the Cofeeware license
 * with this file. If not, please write to m.hajduk(at)pm.me
 */

#include "compiler.hpp"

void __expressionVal(char* a, int yylineno) {
    Variable var = variables.at(a);
    if (var.type == "NUM") {
        setRegister("B", stoll(var.name));
        removeIde(var.name);
    }
    else {
        initError(var, a, yylineno);
        assignRegister("B", var);
    }
}

void __expressionAdd (char* a, char* b, int yylineno) {
    Variable var1 = variables.at(a);
    Variable var2 = variables.at(b);
    initError(var1, a, yylineno);
    initError(var2, b, yylineno);
    expireRegisters();

    if (var1.type == "NUM" && var1.name == "1") {
        assignRegister("B", var2);
        insert("INC", "B");
    } else if (var2.type == "NUM" && var2.name == "1") {
        assignRegister("B", var1);
        insert("INC", "B");
    } else if (var1.type == "NUM" && var1.name == "0") {
        assignRegister("B", var2);
    } else if (var2.type == "NUM" && var2.name == "0") {
        assignRegister("B", var1);
    } else if (var1.type == "NUM" && var2.type == "NUM" && stoll(var1.name) < LLONG_MAX / 2 && stoll(var2.name) < LLONG_MAX / 2) {
        long long int val = stoll(var1.name) + stoll(var2.name);
        setRegister("B", val);
    } else {
        assignRegister("B", var1, "C", var2);
        insert("ADD", "B", "C");
    }
    if (var1.type == "NUM")
        removeIde(var1.name);
    if (var2.type == "NUM")
        removeIde(var2.name);
    expireRegisters();
}

void __expressionSub (char* a, char* b, int yylineno) {
    Variable var1 = variables.at(a);
    Variable var2 = variables.at(b);
    initError(var1, a, yylineno);
    initError(var2, b, yylineno);
    expireRegisters();

    if (var2.type == "NUM" && stoll(var2.name) == 1) {
        assignRegister("B", var1);
        insert("DEC", "B");
    } else if (var1.type == "NUM" && var2.type == "NUM") {
        long long int val = stoll(var1.name) - stoll(var2.name);
        if (val < 0)
            val = 0;
        setRegister("B", val);
    } else {
        assignRegister("B", var1, "C", var2);
        insert("SUB", "B", "C");
    }
    if (var1.type == "NUM")
        removeIde(var1.name);
    if (var2.type == "NUM")
        removeIde(var2.name);
    expireRegisters();
}

void __expressionMul (char* a, char* b, int yylineno) {
    Variable var1 = variables.at(a);
    Variable var2 = variables.at(b);
    initError(var1, a, yylineno);
    initError(var2, b, yylineno);
    expireRegisters();

    if ((var1.type == "NUM" && var1.name == "0") || (var2.type == "NUM" && var2.name == "0")) {
        setRegister("B", 0);
    } else if (var1.type == "NUM" && var2.type == "NUM" && stoll(var1.name) < sqrt(LLONG_MAX) && stoll(var2.name) < sqrt(LLONG_MAX)) {
        long long int val = stoll(var1.name) * stoll(var2.name);
        setRegister("B", val);
    // if power of two
    } else if (var1.type == "NUM" && ((stoll(var1.name) & (stoll(var1.name) - 1)) == 0)) {
        assignRegister("B", var2);
        for (int i = 0; i < log2(stoll(var1.name)); ++i)
            insert("ADD", "B", "B");
    // if power of two
    } else if (var2.type == "NUM" && ((stoll(var2.name) & (stoll(var2.name) - 1)) == 0)) {
        assignRegister("B", var1);
        for (int i = 0; i < log2(stoll(var2.name)); ++i)
            insert("ADD", "B", "B");
    } else {
        assignRegister("C", var1, "D", var2);
        resetRegister("B");
        insert("JODD", "C", cmdIndex + 2);
        insert("JUMP", cmdIndex + 2);
        insert("ADD", "B", "D");
        insert("ADD", "D", "D");
        insert("HALF", "C");
        insert("JZERO", "C", cmdIndex + 2);
        insert("JUMP", cmdIndex - 6);
    }
    if (var1.type == "NUM")
        removeIde(var1.name);
    if (var2.type == "NUM")
        removeIde(var2.name);
    expireRegisters();
}

void __expressionDiv (char* a, char* b, int yylineno) {
    Variable var1 = variables.at(a);
    Variable var2 = variables.at(b);
    initError(var1, a, yylineno);
    initError(var2, b, yylineno);
    expireRegisters();

    if ((var1.type == "NUM" && var1.name == "0") || (var2.type == "NUM" && var2.name == "0")) {
        setRegister("B", 0);
    } else if (var1.type == "NUM" && var2.type == "NUM") {
        long long int val = stoll(var1.name) / stoll(var2.name);
        setRegister("B", val);
    // if power of two
    } else if (var2.type == "NUM" && ((stoll(var2.name) & (stoll(var2.name) - 1)) == 0)) {
        assignRegister("B", var1);
        for (int i = 0; i < log2(stoll(var2.name)); ++i)
            insert("HALF", "B");
    } else {
        assignRegister("D", var1, "C", var2);
        resetRegister("B");
        insert("JZERO", "C", cmdIndex + 22);
        insert("COPY", "E", "C");
        insert("COPY", "B", "E");
        insert("SUB", "B", "D");
        insert("JZERO", "B", cmdIndex + 2);
        insert("JUMP", cmdIndex + 3);
        insert("ADD", "E", "E");
        insert("JUMP", cmdIndex - 5);
        resetRegister("B");
        insert("COPY", "F", "E");
        insert("SUB", "F", "D");
        insert("JZERO", "F", cmdIndex + 4);
        insert("ADD", "B", "B");
        insert("HALF", "E");
        insert("JUMP", cmdIndex + 5);
        insert("ADD", "B", "B");
        insert("INC", "B");
        insert("SUB", "D", "E");
        insert("HALF", "E");
        insert("COPY", "F", "C");
        insert("SUB", "F", "E");
        insert("JZERO", "F", cmdIndex - 12);
    }
    if (var1.type == "NUM")
        removeIde(var1.name);
    if (var2.type == "NUM")
        removeIde(var2.name);
    expireRegisters();
}

void __expressionMod (char* a, char* b, int yylineno) {
    Variable var1 = variables.at(a);
    Variable var2 = variables.at(b);
    initError(var1, a, yylineno);
    initError(var2, b, yylineno);
    expireRegisters();

    if ((var1.type == "NUM" && var1.name == "0") || (var2.type == "NUM" && var2.name == "0")) {
        setRegister("B", 0);
    } else if (var1.type == "NUM" && var2.type == "NUM") {
        long long int val = stoll(var1.name) % stoll(var2.name);
        setRegister("B", val);
    } else if (var2.type == "NUM" && stoll(var2.name) == 2) {
        assignRegister("C", var1);
        resetRegister("B");
        insert("JODD", "C", cmdIndex + 2);
        insert("JUMP", cmdIndex + 2);
        insert("INC", "B");
    } else {
        assignRegister("D", var1, "C", var2);
        insert("JZERO", "C", cmdIndex + 24);
        insert("JZERO", "C", cmdIndex + 22);
        insert("COPY", "E", "C");
        insert("COPY", "B", "E");
        insert("SUB", "B", "D");
        insert("JZERO", "B", cmdIndex + 2);
        insert("JUMP", cmdIndex + 3);
        insert("ADD", "E", "E");
        insert("JUMP", cmdIndex - 5);
        resetRegister("B");
        insert("COPY", "F", "E");
        insert("SUB", "F", "D");
        insert("JZERO", "F", cmdIndex + 4);
        insert("ADD", "B", "B");
        insert("HALF", "E");
        insert("JUMP", cmdIndex + 5);
        insert("ADD", "B", "B");
        insert("INC", "B");
        insert("SUB", "D", "E");
        insert("HALF", "E");
        insert("COPY", "F", "C");
        insert("SUB", "F", "E");
        insert("JZERO", "F", cmdIndex - 12);
        insert("JUMP", cmdIndex + 2);
        resetRegister("D");
        insert("COPY", "B", "D");
    }
    if (var1.type == "NUM")
        removeIde(var1.name);
    if (var2.type == "NUM")
        removeIde(var2.name);
    expireRegisters();
}