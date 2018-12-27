#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <stack>
#include <map>

//////////////////////////////////
// A - memory register			//
// B - attribution register		//
// C D E F - temporary register //
// G - condition register		//
// H - iterator register		//
//////////////////////////////////

using namespace std;

//////////////////////////////////
//            Debug             //
//////////////////////////////////

#ifdef DEBUG
#define DEBUG_MSG(str) do { cout << str << endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

//////////////////////////////////
//          Structures          //
//////////////////////////////////

typedef struct {
	string name;
	string type;
	int counter;
	int memory;
	bool initialized;
} Identifier;

typedef struct {
	int index;
	int depth;
	Identifier iterator;
	Identifier condition;
} Loop;

typedef struct {
	int index;
	Identifier value;
} Condition;

//////////////////////////////////
//          Variables           //
//////////////////////////////////

extern map<string, Identifier> identifiers;
extern stack<Condition> conditions;
extern map<int, Loop> loops;
extern int memIndex;
extern int loopIndex;
extern int cmdIndex;

//////////////////////////////////
//      Token functions         //
//////////////////////////////////

// declare identifier
void __declareIde (char* a, int yylineno);
// assign to variable
void __cmdAssign(char* a, int yylineno);
// just if
void __end_if();
// for loop
void __for(char* i, char* a, char* b, int yylineno);
// end donwto for
void __end_down_for();
// end upto for
void __end_up_for();
// write from register to stdin
void __cmdWrite(char* a, int yylineno);
// read from stdout to register
void __cmdRead(char* a, int yylineno);
// expression value
void __expressionVal(char* a, int yylineno);
// adds two numbers
void __expressionAdd(char* a, char* b);
// subs two numbers
void __expressionSub(char* a, char* b);
// multiples two numbers
void __expressionMul(char* a, char* b);
// divides two numbers
void __expressionDiv(char* a, char* b);
// modulo from two numbers
void __expressionMod(char* a, char* b);
// condition equal
void __condEq(char* a, char* b);
// condition not equal
void __condNe(char* a, char* b);
// constant number
void __valueNum(char* a, int yylineno);
// simple identifier
void __ideIdetifier(char* a, int yylineno);

//////////////////////////////////
//      Register functions      //
//////////////////////////////////

// sets register to given number
void setRegister(string reg, long long int num);
// stores register to memory
void storeRegister(string reg, int mem);
// loads memory to register
void loadRegister(string reg, int mem);
// assign one identifier to register
void assignRegister(string r, Identifier i);
// assign two identifiers to registers
void assignRegister(string r1, Identifier i1, string r2, Identifier i2);
// zero register
void resetRegister(string reg);

//////////////////////////////////
//    Identifiers functions     //
//////////////////////////////////

// create identifier
void createIde(Identifier *s, string name, string type);
// insert identifier
void insertIde(string key, Identifier i);
// remove identifier
void removeIde(string key);

//////////////////////////////////
//        Loop functions        //
//////////////////////////////////

// create loop
void createLoop(Loop* loop, Identifier iterator, Identifier condition, int index);
// insert loop
void insertLoop(Loop loop);
// remove loop
void removeLoop();

//////////////////////////////////
//     Condition functions      //
//////////////////////////////////

// create condition
void createCond();
// remove condition
void removeCond();

//////////////////////////////////
//      Compiler functions      //
//////////////////////////////////

// insert single command
void insert(string str);
// insert single command with registry
void insert(string str, string reg);
// insert double command with registry and registry
void insert(string str, string reg1, string reg2);
// insert single command with registry and number
void insert(string cmd, int index);
// insert double command with registry and number
void insert(string str, string reg, int index);
// prints stack
void print(char* output);
// throws errors
void error(char* a, int yylineno, char const* msg);
// replace string
void replace(string& str, const string& from, const string& to);