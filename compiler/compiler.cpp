#include "compiler.hpp"

//////////////////////////////////
//          Variables           //
//////////////////////////////////

int memIndex = 0;
int cmdIndex = 0;
vector<string> commands;
map<string, Identifier> identifiers;

//////////////////////////////////
//      Token functions         //
//////////////////////////////////

void __declareIde (char* a, int yylineno) { 
    DEBUG_MSG("Zadeklarowano zmienną: " << a);     
    if(identifiers.find(a) != identifiers.end()) {
        error(a, yylineno, "kolejna deklaracja zmiennej");
        exit(1);
    }
    else {
        Identifier ide;
        createIde(&ide, a, 0, "VAR");
        insertIde(a, ide);
    }
}

void __cmdAssign(char* a, int yylineno) {
    Identifier ide = identifiers.at(a);
    storeRegister("B", ide.memory);
    DEBUG_MSG("Przyporządkowano klucz do zmiennej: " << ide.name << " na miejscu: " << ide.memory);
}

void __cmdWrite(char* a, int yylineno) {
    if(identifiers.find(a) == identifiers.end()) {
        error(a, yylineno, "Zmienna nie została zadeklarowana:");
        exit(1);
    }
    Identifier ide = identifiers.at(a);
    loadRegister("B", ide.memory);
    insert("PUT", "B");
    DEBUG_MSG("Wczytywanie klucza: " << ide.name << " z miejsca w pamięci: " << ide.memory);
}

void __cmdRead(char* a, int yylineno) {
    if(identifiers.find(a) == identifiers.end()) {
        error(a, yylineno, "Zmienna nie została zadeklarowana:");
        exit(1);
    }
    insert("GET", "B");
    Identifier ide = identifiers.at(a);
    storeRegister("B", ide.memory);
    DEBUG_MSG("Zapisywanie klucza: " << ide.name << " do miejsca w pamięci: " << ide.memory);
}

void __expressionVal(char* a, int yylineno) {
    Identifier ide = identifiers.at(a);
    DEBUG_MSG("WYRAŻENIE: wartość zmiennej: " << ide.name << ": " << ide.type);
    if(ide.type == "NUM") {
        setRegister("B", stoll(ide.name));
        removeIde(ide.name);
    }
    else if(ide.type == "VAR") {
        loadRegister("B", ide.memory);
    } else {
        // TODO: TAB
        DEBUG_MSG("value TAB");
    }
}

void __expressionAdd (char* a, char* b) {
    Identifier ide1 = identifiers.at(a);
    Identifier ide2 = identifiers.at(b);

    if(ide1.type == "NUM" && ide2.type == "NUM") {
        long long int val = stoll(ide1.name) + stoll(ide2.name);
        setRegister("B", val);
        removeIde(ide1.name);
        removeIde(ide2.name);
    } else if (ide1.type == "VAR" && ide2.type == "NUM") {
        setRegister("C", stoll(ide2.name));
        loadRegister("B", ide1.memory);
        insert("ADD", "B", "C");
        removeIde(ide2.name);
    } else if (ide1.type == "NUM" && ide2.type == "VAR") {
        setRegister("B", stoll(ide1.name));
        loadRegister("C", ide2.memory);
        insert("ADD", "B", "C");
        removeIde(ide1.name);
    } else if (ide1.type == "VAR" && ide2.type == "VAR") {
        loadRegister("B", ide1.memory);
        loadRegister("C", ide2.memory);
        insert("ADD", "B", "C");
    }
    DEBUG_MSG("   Dodawanie: " << ide1.name << ": " << ide1.type << " + " << ide2.name << ": " << ide2.type);
}

void __expressionSub (char* a, char* b) {
    Identifier ide1 = identifiers.at(a);
    Identifier ide2 = identifiers.at(b);

    if(ide1.type == "NUM" && ide2.type == "NUM") {

        long long int val = stoll(ide1.name) - stoll(ide2.name);
        if (val < 0)
            val = 0;

        setRegister("B", val);
        removeIde(ide1.name);
        removeIde(ide2.name);
    } else if (ide1.type == "VAR" && ide2.type == "NUM") {
        setRegister("C", stoll(ide2.name));
        loadRegister("B", ide1.memory);
        insert("SUB", "B", "C");
        removeIde(ide2.name);
    } else if (ide1.type == "NUM" && ide2.type == "VAR") {
        setRegister("B", stoll(ide1.name));
        loadRegister("C", ide2.memory);
        insert("SUB", "B", "C");
        removeIde(ide1.name);
    } else if (ide1.type == "VAR" && ide2.type == "VAR") {
        loadRegister("B", ide1.memory);
        loadRegister("C", ide2.memory);
        insert("SUB", "B", "C");
    }
    DEBUG_MSG("   Usuwanie: " << ide1.name << ": " << ide1.type << " + " << ide2.name << ": " << ide2.type);
}

void __valueNum(char* a, int yylineno) {
    // TODO: próba przypisania do stałej
    DEBUG_MSG("Znaleziono stałą o wartości: " << a);
    Identifier ide;
    createIde(&ide, a, 0, "NUM");
    insertIde(a, ide);
}

void __ideIdetifier(char* a, int yylineno) {
    if(identifiers.find(a) == identifiers.end()) {
        error(a, yylineno, "Zmienna nie została zadeklarowana:");
        exit(1);
    }
    DEBUG_MSG("Znaleziono klucz: " << a);
}

//////////////////////////////////
//      Register functions      //
//////////////////////////////////

void setRegister(string reg, long long int num) {
    resetRegister(reg);

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

void resetRegister(string reg) {
    insert("SUB", reg, reg);
}

//////////////////////////////////
//    Identifiers functions     //
//////////////////////////////////

void createIde(Identifier *ide, string name, int isArray, string type) {
    ide->name = name;
    ide->memory = memIndex;
    ide->type = type;
    if(isArray) {
        ide->tableSize = isArray;
    }
    else {
        ide->tableSize = 0;
    }
}

void insertIde(string key, Identifier ide) {
    if(identifiers.count(key) == 0) {
        identifiers.insert(make_pair(key, ide));
        identifiers.at(key).counter = 0;
        memIndex++;
    }
    else {
        identifiers.at(key).counter = identifiers.at(key).counter + 1;
    }
    DEBUG_MSG("   Dodano do pamięci klucz: " << key << ", typu: " << ide.type << ", na miejscu: " << memIndex - 1);
}

void removeIde(string key) {
    if(identifiers.count(key) > 0) {
        if(identifiers.at(key).counter > 0) {
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

void insert(string cmd, string reg, long long int num) {
    cmd = cmd + " " + reg + " " + to_string(num);
    commands.push_back(cmd);
    cmdIndex ++;
}

void print() {
	long long int pos;
    ofstream file;
    file.open ("./out/out");
    DEBUG_MSG(""/*endl*/"");

	for(pos = 0; pos < commands.size(); pos++) {
        DEBUG_MSG(commands.at(pos));
        file << commands.at(pos) << endl;
    }
    file.close();
}

void error(char* a, int yylineno, char const* msg) {
    cout << "\e[1mBłąd na lini " << yylineno << ", konkretnie: \e[1m\x1B[31m" << msg << " " << a << ".\e[0m\n" << endl;
}