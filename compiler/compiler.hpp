#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

//////////////////////////////////
//          Structures          //
//////////////////////////////////

typedef struct {
	string name;
    string type; //NUM, VAR, ARR
    int initialized;
    int counter;
	long long int mem;
	long long int local;
  	long long int tableSize;
} Identifier;

//////////////////////////////////
//          Variables           //
//////////////////////////////////

extern map<string, Identifier> identifiers;
extern long long int memIndex;

//////////////////////////////////
//      Token functions         //
//////////////////////////////////

void __add (char* a, char* b);
void __declareIde (char* a, int yylineno);
void __declareVal(char* a, int yylineno);
void __declareNum(char* a, int yylineno);

//////////////////////////////////
//      Compiler functions      //
//////////////////////////////////

// stores number
void storeNum(string number);
// create identyfier
void createIde(Identifier *s, string name, long long int isArray, string type);
// insert identyfier
void insertIde(string key, Identifier i);
// remove identifier
void removeIde(string key);
// insert single command
void insert(string str);
// insert single command with registry
void insert(string str, string reg);
// insert double command with registry and number
void insert(string str, string reg, long long int num);
// prints stack
void print();
// throws errors
void error(char* a, int yylineno, char const* msg);