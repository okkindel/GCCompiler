#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

//////////////////////////////////
//          Structures          //
//////////////////////////////////

typedef struct {
	string name;
    string type; //NUM, VAR, ARR
    int counter;
	long long int mem;
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

// adds two numbers
void __add (char* a, char* b);
// declare identifier
void __declareIde (char* a, int yylineno);
// constant number
void __valueNum(char* a, int yylineno);
// expression value
void __expressionVal(char* a, int yylineno);
// identifier
void __ide(char* a, int yylineno);
// assign to variable
void __assing(char* a, int yylineno);
// write register
void __write(char* a, int yylineno);

//////////////////////////////////
//      Register functions      //
//////////////////////////////////

// sets number to register
void setRegister(string reg, long long int num);
// stores register
void storeRegister(string reg, long long int mem);
// loads register
void loadRegister(string reg, long long int mem);
// resets register
void resetRegister(string reg);

//////////////////////////////////
//      Compiler functions      //
//////////////////////////////////

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
// insert double command with registry and register
void insert(string str, string reg1, string reg2);
// insert double command with registry and number
void insert(string str, string reg, long long int num);
// prints stack
void print();
// throws errors
void error(char* a, int yylineno, char const* msg);