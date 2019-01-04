/* Copyright (C) 2018 Maciej Hajduk - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the Beerware license, which unfortunately won't
 * be written for another century.
 *
 * You should have received a copy of the Beerware license
 * with this file. If not, please write to m.hajduk(at)pm.me
 */

#include "compiler.hpp"

void __expressionVal(char* a, int yylineno) {
    Variable var = variables.at(a);
    DEBUG_MSG("Wyrażenie: wartość zmiennej: " << var.name << ": " << var.type);
    if (var.type == "NUM") {
        setRegister("B", stoll(var.name));
        removeIde(var.name);
    }
    else {
        initError(var, a, yylineno);
        assignRegister("B", var);
    }
}

void __expressionAdd (char* a, char* b) {
    Variable var1 = variables.at(a);
    Variable var2 = variables.at(b);

    if (var1.type == "NUM" && var2.type == "NUM") {
        long long int val = stoll(var1.name) + stoll(var2.name);
        setRegister("B", val);
        removeIde(var1.name);
        removeIde(var2.name);
    } else if (var1.type == "NUM" && var1.name == "1") {
        assignRegister("B", var2);
        insert("INC", "B");
        removeIde(var1.name);
    } else if (var2.type == "NUM" && var2.name == "1") {
        assignRegister("B", var1);
        insert("INC", "B");
        removeIde(var2.name);
    } else {
        assignRegister("B", var1, "C", var2);
        insert("ADD", "B", "C");
        if (var1.type == "NUM")
            removeIde(var1.name);
        if (var2.type == "NUM")
            removeIde(var2.name);
    }
    DEBUG_MSG("Dodawanie: " << var1.name << ": " << var1.type << " + " << var2.name << ": " << var2.type);
}

void __expressionSub (char* a, char* b) {
    Variable var1 = variables.at(a);
    Variable var2 = variables.at(b);

    if (var2.type == "NUM" && stoll(var2.name) == 1) {
        assignRegister("B", var1);
        insert("DEC", "B");
        removeIde(var2.name);
    } else if (var1.type == "NUM" && var2.type == "NUM") {
        long long int val = stoll(var1.name) - stoll(var2.name);
        if (val < 0)
            val = 0;
        setRegister("B", val);
        removeIde(var1.name);
        removeIde(var2.name);
    } else {
        assignRegister("B", var1, "C", var2);
        insert("SUB", "B", "C");
        if (var1.type == "NUM")
            removeIde(var1.name);
        if (var2.type == "NUM")
            removeIde(var2.name);
    }
    DEBUG_MSG("Odejmowanie: " << var1.name << ": " << var1.type << " - " << var2.name << ": " << var2.type);
}

void __expressionMul (char* a, char* b) {
    Variable var1 = variables.at(a);
    Variable var2 = variables.at(b);

    if ((var1.type == "NUM" && var1.name == "0") || (var2.type == "NUM" && var2.name == "0")) {
        setRegister("B", 0);
        if (var1.type == "NUM")
            removeIde(var1.name);
        if (var2.type == "NUM")
            removeIde(var2.name);
    } else if (var1.type == "NUM" && var2.type == "NUM") {
        long long int val = stoll(var1.name) * stoll(var2.name);
        setRegister("B", val);
        removeIde(var1.name);
        removeIde(var2.name);
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
    DEBUG_MSG("Mnożenie: " << var1.name << ": " << var1.type << " * " << var2.name << ": " << var2.type);
}

void __expressionDiv (char* a, char* b) {
    Variable var1 = variables.at(a);
    Variable var2 = variables.at(b);

    if ((var1.type == "NUM" && var1.name == "0") || (var2.type == "NUM" && var2.name == "0")) {
        setRegister("B", 0);
        if (var1.type == "NUM")
            removeIde(var1.name);
        if (var2.type == "NUM")
            removeIde(var2.name);
    } else if (var1.type == "NUM" && var2.type == "NUM") {
        long long int val = stoll(var1.name) / stoll(var2.name);
        setRegister("B", val);
        removeIde(var1.name);
        removeIde(var2.name);
    } else if (var2.type == "NUM" && stoll(var2.name) == 2) {
        assignRegister("B", var1);
        insert("HALF", "B");
        removeIde(var2.name);
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

        if (var1.type == "NUM")
            removeIde(var1.name);
        if (var2.type == "NUM")
            removeIde(var2.name);
    }
    DEBUG_MSG("Dzielenie: " << var1.name << ": " << var1.type << " / " << var2.name << ": " << var2.type);
}

void __expressionMod (char* a, char* b) {
    Variable var1 = variables.at(a);
    Variable var2 = variables.at(b);

    if ((var1.type == "NUM" && var1.name == "0") || (var2.type == "NUM" && var2.name == "0")) {
        setRegister("B", 0);
        if (var1.type == "NUM")
            removeIde(var1.name);
        if (var2.type == "NUM")
            removeIde(var2.name);
    } else if (var1.type == "NUM" && var2.type == "NUM") {
        long long int val = stoll(var1.name) % stoll(var2.name);
        setRegister("B", val);
        removeIde(var1.name);
        removeIde(var2.name);
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

        if (var1.type == "NUM")
            removeIde(var1.name);
        if (var2.type == "NUM")
            removeIde(var2.name);
    }
    DEBUG_MSG("Modulo: " << var1.name << ": " << var1.type << " % " << var2.name << ": " << var2.type);
}