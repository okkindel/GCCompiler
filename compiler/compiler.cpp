#include "compiler.hpp"

//////////////////////////////////
//          Variables           //
//////////////////////////////////

long long int memIndex = 0;
vector<string> commands;
map<string, Identifier> identifiers;

//////////////////////////////////
//      Token functions         //
//////////////////////////////////

void __declareIde (char* a, int yylineno) { 
    cout << "Zadeklarowano zmienną: " << a << endl;     
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

void __expressionVal(char* a, int yylineno) {
    Identifier ide = identifiers.at(a);
    cout << "WYRAŻENIE: wartość zmiennej: " << ide.name << ": " << ide.type << endl;
    if(ide.type == "NUM") {
        setRegister("B", stoll(ide.name));
        removeIde(ide.name);
    }
    else if(ide.type == "VAR") {
        loadRegister("B", ide.memory);
    } else {
        // TODO: TAB
        cout << "value TAB" << endl;
    }
}

void __valueNum(char* a, int yylineno) {
    // TODO: próba przypisania do stałej
    cout << "Znaleziono stałą o wartości: " << a << endl;
    Identifier ide;
    createIde(&ide, a, 0, "NUM");
    insertIde(a, ide);
}

void __ide(char* a, int yylineno) {
    if(identifiers.find(a) == identifiers.end()) {
        error(a, yylineno, "Zmienna nie została zadeklarowana:");
        exit(1);
    }
    cout << "Znaleziono klucz: " << a << endl;
}

void __add (char* a, char* b) {
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
        setRegister("C", stoll(ide1.name));
        loadRegister("B", ide2.memory);
        insert("ADD", "B", "C");
        removeIde(ide1.name);
    } else if (ide1.type == "VAR" && ide2.type == "VAR") {
        loadRegister("B", ide1.memory);
        loadRegister("C", ide2.memory);
        insert("ADD", "B", "C");
    }
    cout << "   Dodawanie: " << ide1.name << ": " << ide1.type << " + " << ide2.name << ": " << ide2.type << endl;
}

void __sub (char* a, char* b) {
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
        setRegister("C", stoll(ide1.name));
        loadRegister("B", ide2.memory);
        insert("SUB", "B", "C");
        removeIde(ide1.name);
    } else if (ide1.type == "VAR" && ide2.type == "VAR") {
        loadRegister("B", ide1.memory);
        loadRegister("C", ide2.memory);
        insert("SUB", "B", "C");
    }
    cout << "   Usuwanie: " << ide1.name << ": " << ide1.type << " + " << ide2.name << ": " << ide2.type << endl;
}

void __assign(char* a, int yylineno) {
    Identifier ide = identifiers.at(a);
    storeRegister("B", ide.memory);
    cout << "Przyporządkowano klucz do zmiennej: " << ide.name << " na miejscu: " << ide.memory << endl;
}

void __write(char* a, int yylineno) {
    if(identifiers.find(a) == identifiers.end()) {
        error(a, yylineno, "Zmienna nie została zadeklarowana:");
        exit(1);
    }
    Identifier ide = identifiers.at(a);
    cout << "Wczytywanie klucza: " << ide.name << " z miejsca w pamięci: " << ide.memory << endl;
    loadRegister("B", ide.memory);
    insert("PUT", "B");
}

//////////////////////////////////
//      Register functions      //
//////////////////////////////////

void setRegister(string reg, int num) {
    resetRegister(reg);
	for (long long int i = 0; i < num; ++i) {
        insert("INC", reg);
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
    cout << "   Dodano do pamięci klucz: " << key << ", typu: " << ide.type << ", na miejscu: " << memIndex - 1 << endl;
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
    cout << "Usunięto z pamięci klucz: " << key << endl;
}

//////////////////////////////////
//      Compiler functions      //
//////////////////////////////////

void insert(string cmd) {
    commands.push_back(cmd);
}

void insert(string cmd, string reg) 
{
    cmd = cmd + " " + reg;
    commands.push_back(cmd);
}

void insert(string cmd, string reg1, string reg2) {
    cmd = cmd + " " + reg1 + " " + reg2;
    commands.push_back(cmd);
}

void insert(string cmd, string reg, long long int num) {
    cmd = cmd + " " + reg + " " + to_string(num);
    commands.push_back(cmd);
}

void print() {
	long long int pos;
    cout << endl;

    ofstream file;
    file.open ("./programs/out");

	for(pos = 0; pos < commands.size(); pos++) {
        cout << commands.at(pos) << endl;
        file << commands.at(pos) << endl;
    }
    file.close();
}

void error(char* a, int yylineno, char const* msg) {
    cout << "\e[1mBłąd na lini " << yylineno << ", konkretnie: \e[1m\x1B[31m" << msg << " " << a << ".\e[0m\n" << endl;
}