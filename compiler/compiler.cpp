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
    cout << "declaration: " << a << " -> ";     
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

void __declareVal(char* a, int yylineno) {
    cout << "value -> ";
    Identifier ide = identifiers.at(a);
    cout << ide.name << ": " << ide.type << endl;    
}

void __declareNum(char* a, int yylineno) {
    // TODO: Próba przypisania do stałej
    cout << "number: " << a << " -> ";
    Identifier ide;
    createIde(&ide, a, 0, "NUM");
    insertIde(a, ide);
}

void __add (char* a, char* b) {
    cout << "add -> ";
    Identifier ide1 = identifiers.at(a);
    Identifier ide2 = identifiers.at(b);

    if(ide1.type == "NUM" && ide2.type == "NUM") {
        long long int val = stoll(ide1.name) + stoll(ide2.name);
        // setRegister(to_string(val));
        removeIde(ide1.name);
        removeIde(ide2.name);
    }
    cout << ide1.name << ": " << ide1.type << ", " << ide2.name << ": " << ide2.type << endl;
}

//////////////////////////////////
//      Compiler functions      //
//////////////////////////////////

void storeRegister(long long int memory) {
    insert("STORE");
}

void resetRegister(){

}

void createIde(Identifier *ide, string name, long long int isArray, string type) {
    ide->name = name;
    ide->mem = memIndex;
    ide->type = type;
    ide->initialized = 0;
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
    cout << "identifier added: " << key << ", type: " << ide.type << ", at index: " << memIndex - 1 << endl;
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
    cout << "identifier removed: " << key << endl;
}

void insert(string cmd) {
    commands.push_back(cmd);
}

void insert(string cmd, string reg) 
{
    cmd = cmd + " " + reg;
    commands.push_back(cmd);
}

void insert(string cmd, string reg, long long int num) {
    cmd = cmd + " " + reg + " " + to_string(num);
    commands.push_back(cmd);
}

void print() {
	long long int pos;
	for(pos = 0; pos < commands.size(); pos++)
        cout << commands.at(pos) << endl;
}

void error(char* a, int yylineno, char const* msg) {
    cout << "\e[1mGeneralnie, to zjebałeś na całej lini " << yylineno << \
    ", konkretnie: \e[1m\x1B[31m" << msg << " " << a << ".\e[0m\n" << endl;
}