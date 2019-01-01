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
    Identifier ide = identifiers.at(a);
    DEBUG_MSG("Wyrażenie: wartość zmiennej: " << ide.name << ": " << ide.type);
    if (ide.type == "NUM") {
        setRegister("B", stoll(ide.name));
        removeIde(ide.name);
    }
    else if (ide.type == "VAR") {
        initError(ide, a, yylineno);
        loadRegister("B", ide);
    } else { // TAB
        initError(ide, a, yylineno);
        assignRegister("B", ide);
    }
}

void __expressionAdd (char* a, char* b) {
    Identifier ide1 = identifiers.at(a);
    Identifier ide2 = identifiers.at(b);

    if (ide1.type == "NUM" && ide2.type == "NUM") {
        long long int val = stoll(ide1.name) + stoll(ide2.name);
        setRegister("B", val);
        removeIde(ide1.name);
        removeIde(ide2.name);
    } else if (ide1.type == "NUM" && ide1.name == "1") {
        assignRegister("B", ide2);
        insert("INC", "B");
        removeIde(ide1.name);
    } else if (ide2.type == "NUM" && ide2.name == "1") {
        assignRegister("B", ide1);
        insert("INC", "B");
        removeIde(ide2.name);
    } else {
        assignRegister("B", ide1);
        assignRegister("C", ide2);
        insert("ADD", "B", "C");
        if (ide1.type == "NUM")
            removeIde(ide1.name);
        if (ide2.type == "NUM")
            removeIde(ide2.name);
    }
    DEBUG_MSG("Dodawanie: " << ide1.name << ": " << ide1.type << " + " << ide2.name << ": " << ide2.type);
}

void __expressionSub (char* a, char* b) {
    Identifier ide1 = identifiers.at(a);
    Identifier ide2 = identifiers.at(b);

    if (ide2.type == "NUM" && stoll(ide2.name) == 1) {
        assignRegister("B", ide1);
        insert("DEC", "B");
        removeIde(ide2.name);
    } else if (ide1.type == "NUM" && ide2.type == "NUM") {
        long long int val = stoll(ide1.name) - stoll(ide2.name);
        if (val < 0)
            val = 0;

        setRegister("B", val);
        removeIde(ide1.name);
        removeIde(ide2.name);
    } else if (ide1.type == "TAB" && ide2.type == "TAB") {
        // as long as double tab, we have to switch registers
        assignRegister("C", ide1);
        assignRegister("B", ide2);
        insert("SUB", "B", "C");
    } else {
        assignRegister("B", ide1);
        assignRegister("C", ide2);
        insert("SUB", "B", "C");
        if (ide1.type == "NUM")
            removeIde(ide1.name);
        if (ide2.type == "NUM")
            removeIde(ide2.name);
    }
    DEBUG_MSG("Odejmowanie: " << ide1.name << ": " << ide1.type << " - " << ide2.name << ": " << ide2.type);
}

void __expressionMul (char* a, char* b) {
    Identifier ide1 = identifiers.at(a);
    Identifier ide2 = identifiers.at(b);

    if ((ide1.type == "NUM" && ide1.name == "0") || (ide2.type == "NUM" && ide2.name == "0")) {
        setRegister("B", 0);
        if (ide1.type == "NUM")
            removeIde(ide1.name);
        if (ide2.type == "NUM")
            removeIde(ide2.name);
    } else if (ide1.type == "NUM" && ide2.type == "NUM") {
        long long int val = stoll(ide1.name) * stoll(ide2.name);
        setRegister("B", val);
        removeIde(ide1.name);
        removeIde(ide2.name);
    } else {
        assignRegister("C", ide1);
        assignRegister("D", ide2);
        resetRegister("B");
        insert("JODD", "C", cmdIndex + 2);
        insert("JUMP", cmdIndex + 2);
        insert("ADD", "B", "D");
        insert("ADD", "D", "D");
        insert("HALF", "C");
        insert("JZERO", "C", cmdIndex + 2);
        insert("JUMP", cmdIndex - 6);      
    }
    DEBUG_MSG("Mnożenie: " << ide1.name << ": " << ide1.type << " * " << ide2.name << ": " << ide2.type);
}

void __expressionDiv (char* a, char* b) {
    Identifier ide1 = identifiers.at(a);
    Identifier ide2 = identifiers.at(b);

    if ((ide1.type == "NUM" && ide1.name == "0") || (ide2.type == "NUM" && ide2.name == "0")) {
        setRegister("B", 0);
        if (ide1.type == "NUM")
            removeIde(ide1.name);
        if (ide2.type == "NUM")
            removeIde(ide2.name);
    } else if (ide1.type == "NUM" && ide2.type == "NUM") {
        long long int val = stoll(ide1.name) / stoll(ide2.name);
        setRegister("B", val);
        removeIde(ide1.name);
        removeIde(ide2.name);
    } else if (ide2.type == "NUM" && stoll(ide2.name) == 2) {
        assignRegister("B", ide1);
        insert("HALF", "B");
        removeIde(ide2.name);
    } else {
        if (ide1.type == "TAB" && ide2.type == "TAB") {
            // as long as double tab, we have to switch registers
            assignRegister("C", ide1);
            assignRegister("D", ide2);
        } else {
            assignRegister("D", ide1);
            assignRegister("C", ide2);
        }

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

        if (ide1.type == "NUM")
            removeIde(ide1.name);
        if (ide2.type == "NUM")
            removeIde(ide2.name);
    }
    DEBUG_MSG("Dzielenie: " << ide1.name << ": " << ide1.type << " / " << ide2.name << ": " << ide2.type);
}

void __expressionMod (char* a, char* b) {
    Identifier ide1 = identifiers.at(a);
    Identifier ide2 = identifiers.at(b);

    if ((ide1.type == "NUM" && ide1.name == "0") || (ide2.type == "NUM" && ide2.name == "0")) {
        setRegister("B", 0);
        if (ide1.type == "NUM")
            removeIde(ide1.name);
        if (ide2.type == "NUM")
            removeIde(ide2.name);
    } else if (ide1.type == "NUM" && ide2.type == "NUM") {
        long long int val = stoll(ide1.name) % stoll(ide2.name);
        setRegister("B", val);
        removeIde(ide1.name);
        removeIde(ide2.name);
    } else {
        if (ide1.type == "TAB" && ide2.type == "TAB") {
            // as long as double tab, we have to switch registers
            assignRegister("C", ide1);
            assignRegister("D", ide2);
        } else {
            assignRegister("D", ide1);
            assignRegister("C", ide2);
        }

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

        if (ide1.type == "NUM")
            removeIde(ide1.name);
        if (ide2.type == "NUM")
            removeIde(ide2.name);
    }
    DEBUG_MSG("Dzielenie: " << ide1.name << ": " << ide1.type << " / " << ide2.name << ": " << ide2.type);
}