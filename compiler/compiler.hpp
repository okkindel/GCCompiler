/* Copyright (C) 2018 Maciej Hajduk - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the Beerware license, which unfortunately won't
 * be written for another century.
 *
 * You should have received a copy of the Beerware license
 * with this file. If not, please write to m.hajduk(at)pm.me
 */

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
	int begin;
	int size;
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

typedef struct {
	Identifier value;
	Identifier index;
} Array;

//////////////////////////////////
//          Variables           //
//////////////////////////////////

extern map<string, Identifier> identifiers;
extern stack<Condition> conditions;
extern map<int, Loop> loops;
extern stack<Array> arrays;
extern stack<int> hooks;
extern int loopIndex;
extern int memIndex;
extern int cmdIndex;

//////////////////////////////////
//      Token functions         //
//////////////////////////////////

// declare identifier
void __declareIde (char* a, int yylineno);
// declare array
void __declareTab (char* a, char* b, char* c, int yylineno);
// assign to variable
void __cmdAssign(char* a, int yylineno);
// else
void __if_else();
// end if
void __end_if();
// begin while
void __begin_loop();
// end while
void __end_while();
// end do
void __end_do();
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
void __condEq(char* a, char* b, int yylineno);
// condition not equal
void __condNotEq(char* a, char* b, int yylineno);
// condition lower
void __condLow(char* a, char* b, int yylineno);
// condition greater
void __condGre(char* a, char* b, int yylineno);
// condition lower or equal
void __condLowEq(char* a, char* b, int yylineno);
// condition greater or equal
void __condGreEq(char* a, char* b, int yylineno);
// constant number
void __valueNum(char* a, int yylineno);
// simple identifier
void __ideIdetifier(char* a, int yylineno);
// tab identifier with ide
void __ideIdeIde(char* a, char* b, int yylineno);
// tab identifier with num
void __ideIdeNum(char* a, char* b, int yylineno);

//////////////////////////////////
//      Register functions      //
//////////////////////////////////

// sets register to given number
void setRegister(string reg, long long int num);
// stores register to memory
void storeRegister(string reg, Identifier i);
// loads memory to register
void loadRegister(string reg, Identifier i);
// assign one identifier to register
void assignRegister(string r, Identifier i);
// zero register
void resetRegister(string reg);

//////////////////////////////////
//    Identifiers functions     //
//////////////////////////////////

// create identifier
void createIde(Identifier *s, string name, string type);
// create tab
void createIde(Identifier *ide, string name, string type, int begin, int size);
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
// check for initialized error
void initError(Identifier ide, char* a, int yylineno);
// check if tab and shift
int tabShift(Identifier ide);
// prints stack
void print(char* output);
// throws errors
void error(char* a, int yylineno, char const* msg);
// replace string
void replace(string& str, const string& from, const string& to);