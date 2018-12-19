#include "compiler.hpp"

/////////////////////////////////////
// Variables
/////////////////////////////////////

long long int memIndex = 0;
map<string, Identifier> identifiers;

/////////////////////////////////////
// Token functions
/////////////////////////////////////

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
    cout << "value" << endl;
    cout << a << endl;    
}

void __declareNum(char* a, int yylineno) {
    // TODO: Próba przypisania do stałej
    cout << "number: " << a << " -> ";
    Identifier ide;
    createIde(&ide, a, 0, "NUM");
    insertIde(a, ide);
}

void __add (char* a, char* b) {
    cout << "add: " << a << ", " << b << endl;
    Identifier ide1 = identifiers.at(a);
    Identifier ide2 = identifiers.at(b);
}

/////////////////////////////////////
// Compiler functions
/////////////////////////////////////

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

void insert(string str) {
    cout << str << endl;
}

void insert(string str, string reg) {
    cout << str << reg << endl;
}

void insert(string str, string reg, long long int num) {
    cout << str << reg << num << endl;
}

void error(char* a, int yylineno, char const* msg) {
    cout << "\e[1mGeneralnie, to zjebałeś na całej lini " << yylineno << \
    ", konkretnie: \e[1m\x1B[31m" << msg << " " << a << ".\e[0m\n" << endl;
}