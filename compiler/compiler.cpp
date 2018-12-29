/* Copyright (C) 2018 Maciej Hajduk - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the Beerware license, which unfortunately won't
 * be written for another century.
 *
 * You should have received a copy of the Beerware license
 * with this file. If not, please write to m.hajduk(at)pm.me
 */

#include "compiler.hpp"

//////////////////////////////////
//          Variables           //
//////////////////////////////////

int memIndex = 0;
int cmdIndex = 0;
int loopIndex = 0;
vector<string> commands;
map<string, Identifier> identifiers;
stack<Condition> conditions;
map<int, Loop> loops;
stack<Array> arrays;
stack<int> hooks;

//////////////////////////////////
//      Token functions         //
//////////////////////////////////

void __declareIde (char* a, int yylineno) { 
    DEBUG_MSG("Zadeklarowano zmienną: " << a);     
    if (identifiers.find(a) != identifiers.end())
        error(a, yylineno, "Kolejna deklaracja zmiennej:");
    else {
        Identifier ide;
        createIde(&ide, a, "VAR");
        insertIde(a, ide);
    }
}

void __declareTab (char* a, char* b, char* c, int yylineno) {
    DEBUG_MSG("Zadeklarowano tablicę: " << a);     
    if (identifiers.find(a) != identifiers.end())
        error(a, yylineno, "Kolejna deklaracja zmiennej:");
    else if (stoll(b) > stoll(c))
        error(a, yylineno, "Tablica o ujemnej pojemności:");
    else {
        Identifier ide;
        int size  = stoll(c) - stoll(b);
        createIde(&ide, a, "TAB", stoll(b), size);
        insertIde(a, ide);
    }
}

void __cmdAssign(char* a, int yylineno) {
    Identifier ide = identifiers.at(a);
    if (ide.type == "ITE")
        error(a, yylineno, "Modyfikacja iteratora pętli:");
    identifiers.at(a).initialized = true;
    storeRegister("B", ide);
    DEBUG_MSG("Przyporządkowano klucz do zmiennej: " << ide.name << " na miejscu: " << ide.memory << " i jest zainicjowany: " << ide.initialized);
}

void __if_else() {
    replace(commands.at(conditions.top().index), "$bookmark", to_string(cmdIndex));
    assignRegister("G", conditions.top().value);

    // negation
    setRegister("H", 1);
    insert("SUB", "H", "G");

    removeCond();
    createCond();
    insert("JZERO", "H", "$bookmark");
    DEBUG_MSG("Rozpoczęto procedurę else");
}

void __end_if() {
    replace(commands.at(conditions.top().index), "$bookmark", to_string(cmdIndex));
    removeCond();
    DEBUG_MSG("Zakończono warunek if");
}

void __begin_loop() {
    hooks.push(cmdIndex);
}

void __end_while() {
    insert("JUMP", hooks.top());
    replace(commands.at(conditions.top().index), "$bookmark", to_string(cmdIndex));
    assignRegister("G", conditions.top().value);
    insert("JZERO", "G", cmdIndex + 2);
    insert("JUMP", to_string(conditions.top().index + 1));
    removeCond();
    hooks.pop();
    DEBUG_MSG("Zakończono warunek if");
}

void __end_do() {
    replace(commands.at(conditions.top().index), "$bookmark", to_string(cmdIndex));
    assignRegister("G", conditions.top().value);
    
    insert("JZERO", "G", cmdIndex + 2);
    insert("JUMP", hooks.top());

    removeCond();
    hooks.pop();
    DEBUG_MSG("Zakończono warunek do");
}

void __for(char* i, char* a, char* b, int yylineno) {
    
    DEBUG_MSG("Zadeklarowano zmienną: " << a);     
    if (identifiers.find(i) != identifiers.end())
        error(i, yylineno, "kolejna deklaracja zmiennej");

    Identifier iterator;
    createIde(&iterator, i, "ITE");
    insertIde(i, iterator);
    identifiers.at(i).initialized = true;

    Identifier condition;
    createIde(&condition, "C", "VAR");
    insertIde("C", condition);
    identifiers.at("C").initialized = true;

    Identifier start = identifiers.at(a);
    Identifier finish = identifiers.at(b);

    assignRegister("F", start);
    storeRegister("F", iterator);
    assignRegister("F", finish);
    storeRegister("F", condition);

    Loop loop;
    createLoop(&loop, iterator, condition, cmdIndex);
    insertLoop(loop);
}

void __end_down_for() {

    Loop loop = loops.at(loopIndex - 1);

    assignRegister("G", loop.condition);
    loadRegister("H", loop.iterator);

    insert("COPY", "F", "H");
    insert("SUB", "F", "G");
    insert("JZERO", "F", cmdIndex + loop.iterator.memory + 5);
    insert("DEC H");
    storeRegister("H", loop.iterator);
    insert("JUMP", loop.index);
    removeLoop();
}

void __end_up_for() {

    Loop loop = loops.at(loopIndex - 1);

    assignRegister("G", loop.condition);
    loadRegister("H", loop.iterator);

    insert("COPY", "F", "G");
    insert("SUB", "F", "H");
    insert("JZERO", "F", cmdIndex + loop.iterator.memory + 5);
    insert("INC H");
    storeRegister("H", loop.iterator);
    insert("JUMP", loop.index);
    removeLoop();
}

void __cmdWrite(char* a, int yylineno) {
    Identifier ide = identifiers.at(a);
    initError(ide, a, yylineno);    
    assignRegister("B", ide);
    insert("PUT", "B");
    DEBUG_MSG("Wczytywanie klucza: " << ide.name << " z miejsca w pamięci: " << ide.memory);
}

void __cmdRead(char* a, int yylineno) {
    Identifier ide = identifiers.at(a);
    identifiers.at(a).initialized = true;
    insert("GET", "B");
    storeRegister("B", ide);
    DEBUG_MSG("Zapisywanie klucza: " << ide.name << " do miejsca w pamięci: " << ide.memory);
}

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
    } else if (ide1.type == "NUM" && ide2.type == "VAR") {
        loadRegister("C", ide2);
        resetRegister("B");
        for (long long int i = 0; i < stoll(ide1.name); ++i) {
            insert("ADD", "B", "C");
        }
        removeIde(ide1.name);
    } else if (ide1.type == "VAR" && ide2.type == "NUM") {
        loadRegister("C", ide1);
        resetRegister("B");
        for (long long int i = 0; i < stoll(ide2.name); ++i) {
            insert("ADD", "B", "C");
        }
        removeIde(ide2.name);
    } else if (ide1.type == "VAR" && ide2.type == "VAR") {
        loadRegister("B", ide1);
        loadRegister("C", ide2);
        insert("JZERO", "C", cmdIndex + 7);
        insert("COPY", "D", "B");
        insert("DEC", "C");
        insert("ADD", "B", "D");
        insert("DEC", "C");
        insert("JZERO", "C", cmdIndex + 2);
        insert("JUMP", cmdIndex - 3);        
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
        assignRegister("B", ide1);
        assignRegister("C", ide2);
        // counter
        resetRegister("D");

        // if b == 0
        insert("JZERO", "C", cmdIndex + 9);
        
        insert("COPY", "E", "B");
        insert("SUB", "B", "C");

        // if a <= b
        insert("JZERO", "B", cmdIndex + 4);
        insert("INC", "D");
        insert("JZERO", "B", cmdIndex + 4);
        insert("JUMP", cmdIndex - 5);

        // if a >= b
        insert("SUB", "C", "E");
        insert("JZERO", "C", cmdIndex + 3);

        insert("COPY", "B", "D");
        insert("JUMP", cmdIndex + 3);

        insert("INC", "D");
        insert("JUMP", cmdIndex - 3);

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
        assignRegister("B", ide1);
        assignRegister("C", ide2);
        // if b == 0
        insert("JZERO", "C", cmdIndex + 6);
        
        insert("COPY", "D", "B");
        insert("SUB", "B", "C");

        insert("JZERO", "B", cmdIndex + 2);
        insert("JUMP", cmdIndex - 3);

        // if a == b
        insert("SUB", "C", "D");
        insert("JZERO", "C", cmdIndex + 3);
        insert("COPY", "B", "D");
        insert("JUMP", cmdIndex + 2);
        insert("COPY", "B", "C");

        if (ide1.type == "NUM")
            removeIde(ide1.name);
        if (ide2.type == "NUM")
            removeIde(ide2.name);
    }
    DEBUG_MSG("Dzielenie: " << ide1.name << ": " << ide1.type << " / " << ide2.name << ": " << ide2.type);
}

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

    createCond();
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

    createCond();
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

    createCond();
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

    createCond();
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

    createCond();
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

    createCond();
    insert("JZERO", "G", "$bookmark");

    DEBUG_MSG("Porównano: " << ide1.name << " > " << ide2.name);
}

void __valueNum(char* a, int yylineno) {
    DEBUG_MSG("Znaleziono stałą o wartości: " << a);
    Identifier ide;
    createIde(&ide, a, "NUM");
    insertIde(a, ide);
}

void __ideIdetifier(char* a, int yylineno) {
    if (identifiers.find(a) == identifiers.end())
        error(a, yylineno, "Zmienna nie została zadeklarowana:");
    if (identifiers.at(a).type == "TAB")
        error(a, yylineno, "Brak odwołania do elementu tablicy:");
    DEBUG_MSG("Znaleziono klucz: " << identifiers.at(a).name << " i jest zainicjowany: " << identifiers.at(a).initialized);
}

void __ideIdeIde(char* a, char* b, int yylineno) {
    if (identifiers.find(a) == identifiers.end())
        error(a, yylineno, "Zmienna nie została zadeklarowana:");
    if (identifiers.find(b) == identifiers.end())
        error(a, yylineno, "Zmienna nie została zadeklarowana:");

    Identifier ide = identifiers.at(a);
    Identifier var = identifiers.at(b);

    if (ide.type != "TAB")
        error(a, yylineno, "Niewłaściwe odwołanie do zmiennej:");
    
    Array arr;
    arr.value = ide;
    arr.index = var;
    arrays.push(arr);
    DEBUG_MSG("Znaleziono klucz tablicy: " << identifiers.at(a).name << " i jest zainicjowany: " << identifiers.at(a).initialized);
}

void __ideIdeNum(char* a, char* b, int yylineno) {
    if (identifiers.find(a) == identifiers.end())
        error(a, yylineno, "Zmienna nie została zadeklarowana:");

    Identifier ide = identifiers.at(a);
    Identifier num;
    createIde(&num, b, "NUM");
    insertIde(b, num);

    if (ide.type != "TAB")
        error(a, yylineno, "Niewłaściwe odwołanie do zmiennej:");
    if (stoll(b) < ide.begin || stoll(b) > ide.begin + ide.size)
        error(a, yylineno, "Przekroczony zakres tablicy:");
    
    Array arr;
    arr.value = ide;
    arr.index = num;
    arrays.push(arr);

    removeIde(num.name);
    DEBUG_MSG("Znaleziono klucz tablicy: " << identifiers.at(a).name << " i jest zainicjowany: " << identifiers.at(a).initialized);
}

//////////////////////////////////
//      Register functions      //
//////////////////////////////////

void setRegister(string reg, long long int num) {
    resetRegister(reg);

    // some optimization here, we can also increment in loop
    if (num < 10) {
    	for (long long int i = 0; i < num; ++i) {
            insert("INC", reg);
        }
    } else {
        for (long long int i = 0; i < 10; ++i) {
            insert("INC", reg);
        }

        long long int counter = 10;
        while (2 * counter < num) {
            insert("ADD", reg, reg);
            counter *= 2;
        }
        if (num - counter < 2 * counter - num) {
            for (long long int i = 0; i < num - counter; ++i) {
                insert("INC", reg);
            }
        } else {
            insert("ADD", reg, reg);
            for (long long int i = 0; i < 2 * counter - num; ++i) {
                insert("DEC", reg);
            }
        }
    }    
}

void storeRegister(string reg, Identifier i) {
    assignMemory(i);
    insert("STORE", reg);
}

void loadRegister(string reg, Identifier i) {
    assignMemory(i);
    insert("LOAD", reg);
}

void assignMemory(Identifier i) {
    if (i.type == "TAB") {
        assignRegister("F", arrays.top().index);
        setRegister("A", i.memory);
        insert("ADD", "A", "F");
        arrays.pop();
    } else
        setRegister("A", i.memory);
}

void assignRegister(string r, Identifier i) {
    if (i.type == "NUM")
        setRegister(r, stoll(i.name));
    else if (i.type == "VAR" || i.type == "ITE" || i.type == "TAB")
        loadRegister(r, i);
}

void resetRegister(string reg) {
    insert("SUB", reg, reg);
}

//////////////////////////////////
//    Identifiers functions     //
//////////////////////////////////

void createIde(Identifier *ide, string name, string type) {
    ide->name = name;
    ide->memory = memIndex;
    ide->type = type;
    ide->initialized = false;
    ide->size = 1;
}

void createIde(Identifier *ide, string name, string type, int begin, int size) {
    ide->name = name;
    ide->memory = memIndex;
    ide->type = type;
    ide->initialized = false;
    ide->begin = begin;
    ide->size = size;
}

void insertIde(string key, Identifier ide) {
    DEBUG_MSG("Dodano do pamięci klucz: " << key << ", typu: " << ide.type << ", na miejscu: " << memIndex);
    if (identifiers.count(key) == 0) {
        identifiers.insert(make_pair(key, ide));
        identifiers.at(key).counter = 0;
        memIndex += ide.size;
    }
    else {
        identifiers.at(key).counter = identifiers.at(key).counter + 1;
    }
}

void removeIde(string key) {
    if (identifiers.count(key) > 0) {
        if (identifiers.at(key).counter > 0) {
            identifiers.at(key).counter = identifiers.at(key).counter - 1;
        }
        else {
            identifiers.erase(key);
            memIndex--;
        }
    }
    DEBUG_MSG("Usunięto z pamięci klucz: " << key);
}

//////////////////////////////////
//        Loop functions        //
//////////////////////////////////

void createLoop(Loop* loop, Identifier iterator, Identifier condition, int index) {
    DEBUG_MSG("Warunek pętli: " << condition.name << ", typu: " << condition.type);
    loop->depth = loopIndex;
    loop->iterator = iterator;
    loop->condition = condition;
    loop->index = index;
}

void insertLoop(Loop loop) {
    loops.insert(make_pair(loopIndex, loop));
    loopIndex++;
    DEBUG_MSG("Dodano do pamięci pętlę o id: " << loopIndex - 1);
}

void removeLoop() {
    removeIde(loops.at(loopIndex - 1).iterator.name);
    removeIde(loops.at(loopIndex - 1).condition.name);
    loops.erase(loopIndex - 1);
    loopIndex--;
    DEBUG_MSG("Usunięto z pamięci pętlę o id: " << loopIndex);
}

//////////////////////////////////
//     Condition functions      //
//////////////////////////////////

void createCond() {
    DEBUG_MSG("Nowy warunek");

    Identifier value;
    createIde(&value, "V", "VAR");
    insertIde("V", value);
    identifiers.at("V").initialized = true;
    storeRegister("G", value);

    Condition cond;
    cond.index = cmdIndex;
    cond.value = value;
    conditions.push(cond);
}

void removeCond() {
    removeIde(conditions.top().value.name);
    conditions.pop();
    DEBUG_MSG("Usunięto z pamięci warunek");
}

//////////////////////////////////
//      Compiler functions      //
//////////////////////////////////

void insert(string cmd) {
    commands.push_back(cmd);
    cmdIndex ++;
}

void insert(string cmd, string reg) 
{
    cmd = cmd + " " + reg;
    commands.push_back(cmd);
    cmdIndex ++;
}

void insert(string cmd, string reg1, string reg2) {
    cmd = cmd + " " + reg1 + " " + reg2;
    commands.push_back(cmd);
    cmdIndex ++;
}

void insert(string cmd, string reg, int index) {
    cmd = cmd + " " + reg + " " + to_string(index);
    commands.push_back(cmd);
    cmdIndex ++;
}

void insert(string cmd, int index) {
    cmd = cmd + " " + to_string(index);
    commands.push_back(cmd);
    cmdIndex ++;
}

void initError(Identifier ide, char* a, int yylineno) {
    if (ide.type != "NUM" && ide.initialized == false)
        error(a, yylineno, "Próba użycia niezainicjalizowanej zmiennej:");
}

void print(char* out) {
	long long int cmd;
    ofstream file;
    file.open(out);
    DEBUG_MSG(""/*endl*/"");

    for(cmd = 0; cmd < commands.size(); cmd++) {
        DEBUG_MSG(cmd << ":\t" << commands.at(cmd));
        file << commands.at(cmd) << endl;
    }
    file.close();
}

void error(char* a, int yylineno, char const* msg) {
    cout << "\e[1m\x1B[31m[ ERROR ]\e[0m \e[1m[ LINE " << yylineno << " ] \e[1m\x1B[31m" << msg << " " << a << ".\e[0m\n" << endl;
    exit(1);
}

void replace(string& str, const string& from, const string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}