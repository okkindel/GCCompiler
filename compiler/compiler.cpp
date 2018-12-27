#include "compiler.hpp"

//////////////////////////////////
//          Variables           //
//////////////////////////////////

int memIndex = 0;
int cmdIndex = 0;
int loopIndex = 0;
vector<string> commands;
map<string, Identifier> identifiers;
map<int, Loop> loops;
int bookmark;
int endif;

//////////////////////////////////
//      Token functions         //
//////////////////////////////////

void __declareIde (char* a, int yylineno) { 
    DEBUG_MSG("Zadeklarowano zmienną: " << a);     
    if (identifiers.find(a) != identifiers.end())
        error(a, yylineno, "Kolejna deklaracja zmiennej");
    else {
        Identifier ide;
        createIde(&ide, a, "VAR");
        insertIde(a, ide);
    }
}

void __cmdAssign(char* a, int yylineno) {
    Identifier ide = identifiers.at(a);
    if (ide.type == "ITE")
        error(a, yylineno, "Modyfikacja iteratora pętli:");
    identifiers.at(a).initialized = true;
    storeRegister("B", ide.memory);
    DEBUG_MSG("Przyporządkowano klucz do zmiennej: " << ide.name << " na miejscu: " << ide.memory << " i jest zainicjowany: " << ide.initialized);
}

void __endIf() {
    insert("PUT", "G");
    insert("JZERO", "G", cmdIndex + 3);
    resetRegister("G");
    insert("JUMP", bookmark);
    DEBUG_MSG("Zakończono warunek if");
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
    createIde(&condition, b, "VAR");
    insertIde(b, condition);
    identifiers.at(b).initialized = true;

    Identifier start = identifiers.at(a);
    Identifier finish = identifiers.at(b);

    assignRegister("F", start);
    storeRegister("F", iterator.memory);

    assignRegister("F", finish);
    storeRegister("F", condition.memory);

    Loop loop;
    createLoop(&loop, iterator, condition, cmdIndex);
    insertLoop(loop);
}

void __end_down_for() {

    Loop loop = loops.at(loopIndex - 1);

    assignRegister("G", loop.condition);
    loadRegister("H", loop.iterator.memory);

    insert("COPY", "F", "H");
    insert("SUB", "F", "G");
    insert("JZERO", "F", cmdIndex + loop.iterator.memory + 5);
    insert("DEC H");
    storeRegister("H", loop.iterator.memory);
    insert("JUMP", loop.index);
    removeLoop();
}

void __end_up_for() {

    Loop loop = loops.at(loopIndex - 1);

    assignRegister("G", loop.condition);
    loadRegister("H", loop.iterator.memory);

    insert("COPY", "F", "G");
    insert("SUB", "F", "H");
    insert("JZERO", "F", cmdIndex + loop.iterator.memory + 5);
    insert("INC H");
    storeRegister("H", loop.iterator.memory);
    insert("JUMP", loop.index);
    removeLoop();
}

void __cmdWrite(char* a, int yylineno) {
    Identifier ide = identifiers.at(a);
    if (ide.type != "NUM" && ide.initialized == false)
        error(a, yylineno, "Próba użycia niezainicjalizowanej zmiennej:");
    assignRegister("B", ide);
    insert("PUT", "B");
    DEBUG_MSG("Wczytywanie klucza: " << ide.name << " z miejsca w pamięci: " << ide.memory);
}

void __cmdRead(char* a, int yylineno) {
    Identifier ide = identifiers.at(a);
    identifiers.at(a).initialized = true;
    insert("GET", "B");
    storeRegister("B", ide.memory);
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
        if (ide.initialized == false)
            error(a, yylineno, "Próba użycia niezainicjalizowanej zmiennej:");
        loadRegister("B", ide.memory);
    } else {
        // TODO: TAB
        DEBUG_MSG("value TAB");
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
        assignRegister("B", ide1, "C", ide2);
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

    if (ide1.type == "NUM" && ide2.type == "NUM") {
        long long int val = stoll(ide1.name) - stoll(ide2.name);
        if (val < 0)
            val = 0;

        setRegister("B", val);
        removeIde(ide1.name);
        removeIde(ide2.name);
    } else {
        assignRegister("B", ide1, "C", ide2);
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
        loadRegister("C", ide2.memory);
        resetRegister("B");
        for (long long int i = 0; i < stoll(ide1.name); ++i) {
            insert("ADD", "B", "C");
        }
        removeIde(ide1.name);
    } else if (ide1.type == "VAR" && ide2.type == "NUM") {
        loadRegister("C", ide1.memory);
        resetRegister("B");
        for (long long int i = 0; i < stoll(ide2.name); ++i) {
            insert("ADD", "B", "C");
        }
        removeIde(ide2.name);
    } else if (ide1.type == "VAR" && ide2.type == "VAR") {
        loadRegister("B", ide1.memory);
        loadRegister("C", ide2.memory);
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
    } else {
        assignRegister("B", ide1, "C", ide2);
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
        assignRegister("B", ide1, "C", ide2);
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

void __condEq(char* a, char* b) {
    Identifier ide1 = identifiers.at(a);
    Identifier ide2 = identifiers.at(b);

    // condition reg
    resetRegister("G");
    assignRegister("C", ide1, "D", ide2);
    // eq if a <= b && b >= a
    insert("COPY", "E", "C");
    insert("SUB", "E", "D");
    insert("JZERO", "E", cmdIndex + 2);
    insert("JUMP", cmdIndex + 6);
    insert("COPY", "E", "D");
    insert("SUB", "E", "C"); 
    insert("JZERO", "E", cmdIndex + 2);
    insert("JUMP", cmdIndex + 2);
    insert("INC", "G");

    bookmark = cmdIndex;
    DEBUG_MSG("Porównano: " << ide1.name << " i " << ide2.name);
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
    DEBUG_MSG("Znaleziono klucz: " << identifiers.at(a).name << " i jest zainicjowany: " << identifiers.at(a).initialized);
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

void storeRegister(string reg, int mem) {
    setRegister("A", mem);
    insert("STORE", reg);
}

void loadRegister(string reg, int mem) {
    setRegister("A", mem);
	insert("LOAD", reg);
}

void assignRegister(string r, Identifier i) {
    if (i.type == "NUM")
        setRegister(r, stoll(i.name));
    else if (i.type == "VAR" || i.type == "ITE")
        loadRegister(r, i.memory);
}

void assignRegister(string r1, Identifier i1, string r2, Identifier i2) {
    if (i1.type == "NUM")
        setRegister(r1, stoll(i1.name));
    else if (i1.type == "VAR" || i1.type == "ITE")
        loadRegister(r1, i1.memory);
    if (i2.type == "NUM")
        setRegister(r2, stoll(i2.name));
    else if (i2.type == "VAR" || i2.type == "ITE")
        loadRegister(r2, i2.memory);    
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
}

void insertIde(string key, Identifier ide) {
    if (identifiers.count(key) == 0) {
        identifiers.insert(make_pair(key, ide));
        identifiers.at(key).counter = 0;
        memIndex++;
    }
    else {
        identifiers.at(key).counter = identifiers.at(key).counter + 1;
    }
    DEBUG_MSG("Dodano do pamięci klucz: " << key << ", typu: " << ide.type << ", na miejscu: " << memIndex - 1);
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
    loops.erase(loopIndex - 1);
    loopIndex--;
    DEBUG_MSG("Usunięto z pamięci pętlę o id: " << loopIndex);
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
    cout << "\e[1mBłąd na lini " << yylineno << ": \e[1m\x1B[31m" << msg << " " << a << ".\e[0m\n" << endl;
    exit(1);
}