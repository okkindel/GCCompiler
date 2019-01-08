/* Copyright (C) 2018 Maciej Hajduk - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the Cofeeware license, which unfortunately won't
 * be written for another century.
 *
 * You should have received a copy of the Cofeeware license
 * with this file. If not, please write to m.hajduk(at)pm.me
 */

#include "compiler.hpp"

//////////////////////////////////
//          Variables           //
//////////////////////////////////

int cmdIndex = 0;
int memIndex = 0;
stack<Loop> loops;
stack<Jump> jumps;
vector<string> tabs;
stack<Array> arrays;
vector<string> commands;
map<string, Variable> variables;

//////////////////////////////////
//      Token functions         //
//////////////////////////////////

void __declareIde (char* a, int yylineno) { 
    DEBUG_MSG("Zadeklarowano zmienną: " << a);     
    if (variables.find(a) != variables.end())
        error(a, yylineno, "Repeated variable declaration:");
    else {
        Variable var;
        createIde(&var, a, "VAR");
        insertIde(a, var);
    }
}

void __declareTab (char* a, char* b, char* c, int yylineno) {
    DEBUG_MSG("Zadeklarowano tablicę: " << a);     
    if (stoll(b) > stoll(c))
        error(a, yylineno, "Negative capacity array:");
    else {
        // we will add tabs on the end of declarations
        tabs.push_back(string(a) + " " + string(b) + " " + string(c) + " " + to_string(yylineno));
    }
}

void insertTabs() {
    for (int cmd = 0; cmd < tabs.size(); cmd++) {
        vector<string> tab;
        split(tabs[cmd], tab, ' ');
        if (variables.find(tab[0]) != variables.end()) {
            char *a = new char[tab[0].length() + 1];
            strcpy(a, tab[0].c_str());
            error(a, stoi(tab[3]), "Repeated variable declaration:");
        }
        Variable var;
        // We have to left one memory cell free.
        int size  = 1 + stoll(tab[2]) - stoll(tab[1]) + 1;
        createIde(&var, tab[0], "TAB", stoll(tab[1]), size);
        insertIde(tab[0], var);
        memIndex++;
    }
}

void __cmdAssign(char* a, int yylineno) {
    Variable var = variables.at(a);
    if (var.type == "ITE")
        error(a, yylineno, "Loop iterator modification:");
    variables.at(a).initialized = true;
    storeRegister("B", var);
    DEBUG_MSG("Przyporządkowano klucz do zmiennej: " << var.name << " na miejscu: " << var.memory << " i jest zainicjowany: " << var.initialized);
}

void __if_else() {
    assignJump(cmdIndex);
    assignRegister("G", jumps.top().value);

    // negation
    setRegister("H", 1);
    insert("SUB", "H", "G");

    removeJump();
    createJump();
    insert("JZERO", "H", "$bookmark");
    DEBUG_MSG("Rozpoczęto procedurę else");
}

void __end_if() {
    assignJump(cmdIndex);
    removeJump();
    DEBUG_MSG("Zakończono warunek if");
}

void __begin_while() {
    createLoop(cmdIndex);
}

void __end_while() {
    insert("JUMP", loops.top().index);
    assignJump(cmdIndex);
    assignRegister("G", jumps.top().value);
    insert("JZERO", "G", cmdIndex + 2);
    insert("JUMP", to_string(jumps.top().index + 1));
    removeJump();
    removeLoop();
    DEBUG_MSG("Zakończono warunek if");
}

void __end_do() {
    assignJump(cmdIndex);
    assignRegister("G", jumps.top().value);
    insert("JZERO", "G", cmdIndex + 2);
    insert("JUMP", loops.top().index);
    removeJump();
    removeLoop();
    DEBUG_MSG("Zakończono warunek do");
}

void __for(char* i, char* a, char* b, int yylineno) {
    
    DEBUG_MSG("Zadeklarowano zmienną: " << a);     
    if (variables.find(i) != variables.end())
        error(i, yylineno, "Repeated variable declaration:");

    Variable iterator;
    createIde(&iterator, i, "ITE");
    insertIde(i, iterator);
    variables.at(i).initialized = true;

    Variable condition;
    createIde(&condition, ("C" + cmdIndex), "VAR");
    insertIde(("C" + cmdIndex), condition);
    variables.at("C" + cmdIndex).initialized = true;

    Variable start = variables.at(a);
    Variable finish = variables.at(b);

    assignRegister("H", start, "F", finish);
    storeRegister("H", iterator);
    storeRegister("F", condition);
    createLoop(iterator, condition, cmdIndex);
}

void __end_down_for() {

    Loop loop = loops.top();
    assignRegister("G", loop.condition, "H", loop.iterator);

    insert("COPY", "F", "H");
    insert("SUB", "F", "G");
    createJump();
    insert("JZERO", "F", "$bookmark");
    insert("DEC H");
    storeRegister("H", loop.iterator);
    insert("JUMP", loop.index);
    assignJump(cmdIndex);
    removeJump();
    removeLoop();
}

void __end_up_for() {

    Loop loop = loops.top();
    assignRegister("G", loop.condition, "H", loop.iterator);

    insert("COPY", "F", "G");
    insert("SUB", "F", "H");
    createJump();
    insert("JZERO", "F", "$bookmark");
    insert("INC H");
    storeRegister("H", loop.iterator);
    insert("JUMP", loop.index);
    assignJump(cmdIndex);
    removeJump();
    removeLoop();
}

void __cmdWrite(char* a, int yylineno) {
    Variable var = variables.at(a);
    initError(var, a, yylineno);    
    assignRegister("B", var);
    insert("PUT", "B");
    DEBUG_MSG("Wczytywanie klucza: " << var.name << " z miejsca w pamięci: " << var.memory);
}

void __cmdRead(char* a, int yylineno) {
    Variable var = variables.at(a);
    variables.at(a).initialized = true;
    insert("GET", "B");
    storeRegister("B", var);
    DEBUG_MSG("Zapisywanie klucza: " << var.name << " do miejsca w pamięci: " << var.memory);
}

void __valueNum(char* a, int yylineno) {
    DEBUG_MSG("Znaleziono stałą o wartości: " << a);
    Variable var;
    createIde(&var, a, "NUM");
    insertIde(a, var);
}

void __ideIdetifier(char* a, int yylineno) {
    if (variables.find(a) == variables.end())
        error(a, yylineno, "The variable has not been declared:");
    if (variables.at(a).type == "TAB")
        error(a, yylineno, "Lack of reference to the element of the array:");
    DEBUG_MSG("Znaleziono klucz: " << variables.at(a).name << " i jest zainicjowany: " << variables.at(a).initialized);
}

void __ideIdeIde(char* a, char* b, int yylineno) {
    if (variables.find(a) == variables.end())
        error(a, yylineno, "The variable has not been declared:");
    if (variables.find(b) == variables.end())
        error(b, yylineno, "The variable has not been declared:");

    Variable name = variables.at(a);
    Variable index = variables.at(b);

    if (name.type != "TAB")
        error(a, yylineno, "Incorrect reference to the variable:");
    
    Array arr;
    arr.name = name;
    arr.index = index;
    arrays.push(arr);
    DEBUG_MSG("Znaleziono klucz tablicy: " << variables.at(a).name << " i jest zainicjowany: " << variables.at(a).initialized);
}

void __ideIdeNum(char* a, char* b, int yylineno) {
    if (variables.find(a) == variables.end())
        error(a, yylineno, "The variable has not been declared:");

    Variable var = variables.at(a);
    Variable num;
    createIde(&num, b, "NUM");
    insertIde(b, num);

    if (var.type != "TAB")
        error(a, yylineno, "Incorrect reference to the variable:");
    if (stoll(b) < var.begin || stoll(b) > var.begin + var.size)
        error(a, yylineno, "The range of the array has been exceeded:");
    
    Array arr;
    arr.name = var;
    arr.index = num;
    arrays.push(arr);

    removeIde(num.name);
    DEBUG_MSG("Znaleziono klucz tablicy: " << variables.at(a).name << " i jest zainicjowany: " << variables.at(a).initialized);
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
        string bin = decToBin(num);
        long long int size = bin.size();
        for (long long int i = 0; i < size; ++i) {
            if(bin[i] == '1') {
                insert("INC", reg);
            }
            if(i < (size - 1)) {
	            insert("ADD", reg, reg);
            }
        }
    }
}

void storeRegister(string reg, Variable var) {
    assignMemory(var);
    insert("STORE", reg);
}

void loadRegister(string reg, Variable var) {
    assignMemory(var);
    insert("LOAD", reg);
}

void assignMemory(Variable var) {
    if (var.type == "TAB") {
        assignRegister("H", arrays.top().index);
        setRegister("A", arrays.top().name.memory);
        insert("ADD", "A", "H");
        arrays.pop();
    } else {
        setRegister("A", var.memory);
    }
}

void assignRegister(string reg, Variable var) {
    if (var.type == "NUM")
        setRegister(reg, stoll(var.name));
    else
        loadRegister(reg, var);
}

void assignRegister(string reg1, Variable var1, string reg2, Variable var2) {
    if (var1.type == "TAB" && var2.type == "TAB") {
        // as long as double tab, we have to switch registers
        assignRegister(reg2, var1);
        assignRegister(reg1, var2);
    } else {
        assignRegister(reg1, var1);
        assignRegister(reg2, var2);
    }
}

void resetRegister(string reg) {
    insert("SUB", reg, reg);
}

//////////////////////////////////
//    Variables functions     //
//////////////////////////////////

void createIde(Variable* var, string name, string type) {
    var->name = name;
    var->memory = memIndex;
    var->type = type;
    var->initialized = false;
    var->size = 1;
}

void createIde(Variable* var, string name, string type, int begin, int size) {
    var->name = name;
    var->memory = memIndex;
    var->type = type;
    var->initialized = false;
    var->begin = begin;
    var->size = size;
}

void insertIde(string key, Variable var) {
    DEBUG_MSG("Dodano do pamięci klucz: " << key << ", typu: " << var.type << ", na miejscu: " << memIndex);
    variables.insert(make_pair(key, var));
    memIndex += var.size;
    
}

void removeIde(string key) {
    variables.erase(key);
    memIndex--;
    DEBUG_MSG("Usunięto z pamięci klucz: " << key);
}

//////////////////////////////////
//        Loop functions        //
//////////////////////////////////

void createLoop(int index) {
    DEBUG_MSG("Zakotwiczenie: " << index);
    Loop loop;
    loop.index = index;
    loops.push(loop);
}

void createLoop(Variable iterator, Variable condition, int index) {
    DEBUG_MSG("Warunek pętli: " << condition.name << ", typu: " << condition.type);
    Loop loop;
    loop.iterator = iterator;
    loop.condition = condition;
    loop.index = index;
    loops.push(loop);
}

void removeLoop() {
    removeIde(loops.top().iterator.name);
    removeIde(loops.top().condition.name);
    loops.pop();
    DEBUG_MSG("Usunięto z pamięci pętlę o id: " << loopIndex);
}

//////////////////////////////////
//     Condition functions      //
//////////////////////////////////

void createJump() {
    DEBUG_MSG("Nowy warunek");

    Variable value;
    createIde(&value, ("J" + cmdIndex), "VAR");
    insertIde(("J" + cmdIndex), value);
    variables.at("J" + cmdIndex).initialized = true;
    storeRegister("G", value);

    Jump jump;
    jump.index = cmdIndex;
    jump.value = value;
    jumps.push(jump);
}

void assignJump(int bookmark) {
    replace(commands.at(jumps.top().index), "$bookmark", to_string(bookmark));
}

void removeJump() {
    removeIde(jumps.top().value.name);
    jumps.pop();
    DEBUG_MSG("Usunięto z pamięci warunek");
}

//////////////////////////////////
//      Compiler functions      //
//////////////////////////////////

void insert(string cmd) {
    commands.push_back(cmd);
    cmdIndex ++;
}

void insert(string cmd, string reg) {
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

void initError(Variable var, char* a, int yylineno) {
    if (var.type != "NUM" && var.initialized == false)
        error(a, yylineno, "An attempt to use an uninitialized variable:");
}

void optymize() {
    bool is_written = false;
    for (int cmd = 0; cmd < commands.size(); cmd++) {
        char const *COMMANDS = commands.at(cmd).c_str();
        char const *COMMAND = "PUT";
        if(strstr(COMMANDS, COMMAND) != NULL) {
            is_written = true;
            break;
        }
    }
    if (!is_written) {
        for (auto it = begin(commands); it != end(commands) - 1;) {
            char const *COMMANDS = commands.at(distance(commands.begin(), it)).c_str();
            char const *COMMAND = "GET";
            if (!strstr(COMMANDS, COMMAND))
                it = commands.erase(it);
            else
                ++it;
        }
    }
}

void print(char* out) {
    ofstream file;
    file.open(out);
    DEBUG_MSG(""/*endl*/"");

    for (int cmd = 0; cmd < commands.size(); cmd++) {
        DEBUG_MSG(cmd << ":\t" << commands.at(cmd));
        file << commands.at(cmd) << endl;
    }
    file.close();
}

void error(char* a, int yylineno, char const* msg) {
    cout << "\e[1m\x1B[31m[ ERROR ]\e[0m \e[1m[ LINE " << yylineno << " ] \e[1m\x1B[31m" << msg << " " << a << ".\e[0m\n" << endl;
    exit(1);
}

void replace(string& str, const string& from, const string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

size_t split(const std::string &txt, std::vector<std::string> &strs, char ch) {
    size_t pos = txt.find( ch );
    size_t initialPos = 0;
    strs.clear();
    while( pos != std::string::npos ) {
        strs.push_back( txt.substr( initialPos, pos - initialPos ) );
        initialPos = pos + 1;

        pos = txt.find( ch, initialPos );
    }
    strs.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos + 1 ) );
    return strs.size();
}

string decToBin(long long int num) {
    string bin;
    while (num != 0) { 
        bin = (num % 2 == 0 ? "0" : "1") + bin; 
        num /= 2;
    }
    return bin;
}