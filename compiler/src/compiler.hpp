/* Copyright (C) 2018 Maciej Hajduk - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the Cofeeware license, which unfortunately won't
 * be written for another century.
 *
 * You should have received a copy of the Cofeeware license
 * with this file. If not, please write to m.hajduk(at)pm.me
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <climits>
#include <math.h>
#include <vector>
#include <stack>
#include <map>

//////////////////////////////////
// A - memory register			//
// B - attribution register		//
// C D E F - temporary register	//
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
	int memory;
	bool initialized;
	int begin;
	int size;
} Variable;

typedef struct {
	int index;
	Variable iterator;
	Variable condition;
} Loop;

typedef struct {
	int index;
	Variable value;
} Jump;

typedef struct {
	Variable name;
	Variable index;
} Array;

//////////////////////////////////
//          Variables           //
//////////////////////////////////

extern map<string, Variable> variables;
extern stack<Jump> jumps;
extern stack<Array> arrays;
extern stack<Loop> loops;
extern int memIndex;
extern int cmdIndex;

//////////////////////////////////
//      Token functions         //
//////////////////////////////////

// declare variable
void __declareIde (char* a, int yylineno);
// declare array
void __declareTab (char* a, char* b, char* c, int yylineno);
// insert tabs on the end
void insertTabs();
// assign to variable
void __cmdAssign(char* a, int yylineno);
// else
void __if_else();
// end if
void __end_if();
// begin while
void __begin_while();
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
// simple variable
void __ideIdetifier(char* a, int yylineno);
// tab variable with var
void __ideIdeIde(char* a, char* b, int yylineno);
// tab variable with num
void __ideIdeNum(char* a, char* b, int yylineno);

//////////////////////////////////
//      Register functions      //
//////////////////////////////////

// sets register to given number
void setRegister(string reg, long long int num);
// stores register to memory
void storeRegister(string reg, Variable var);
// loads memory to register
void loadRegister(string reg, Variable var);
// loads proper memory
void assignMemory(Variable var);
// assign one variable to register
void assignRegister(string reg, Variable var);
// assign double register
void assignRegister(string reg1, Variable var1, string reg2, Variable var2);
// zero register
void resetRegister(string reg);

//////////////////////////////////
//    Identifiers functions     //
//////////////////////////////////

// create variable
void createIde(Variable *s, string name, string type);
// create tab
void createIde(Variable *var, string name, string type, int begin, int size);
// insert variable
void insertIde(string key, Variable i);
// remove variable
void removeIde(string key);

//////////////////////////////////
//        Loop functions        //
//////////////////////////////////

// create hook
void createLoop(int index);
// create loop
void createLoop(Variable iterator, Variable condition, int index);
// remove loop
void removeLoop();

//////////////////////////////////
//     Condition functions      //
//////////////////////////////////

// create condition
void createJump();
// assign bookmark to jump
void assignJump(int bookmark);
// remove condition
void removeJump();

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
void initError(Variable var, char* a, int yylineno);
// optymize code, remove unused commands
void optymize();
// prints stack
void print(char* output);
// throws errors
void error(char* a, int yylineno, char const* msg);
// replace string
void replace(string& str, const string& from, const string& to);
// split by space
size_t split(const std::string &txt, std::vector<std::string> &strs, char ch);
// decimal to binary
string decToBin(long long int n);