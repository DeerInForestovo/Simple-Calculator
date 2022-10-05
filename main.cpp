#include "simbc.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
using namespace std;
char S[1000];
error_in_bc Error_in_bc;
struct Trie { // 0 = Empty node, 1 = Root
    int To[100000][53], Ids[1000000], Cnt, Id;
    Trie() {
        memset(To, 0, sizeof To);
        Cnt = 1;
        Id = 0;
    }
    static int toInt(char s) {
        if(s == '_') return 0;
        if('A' <= s && s <= 'Z') return s - 'A' + 1;
        if('a' <= s && s <= 'z') return s - 'a' + 27;
        return -1;
    }
    int Insert(char* begin, char* end, int setId = 0) {
        int Nod = 1;
        for(char* s = begin; (end ? s != end : *s); ++s) {
            int c = toInt(*s);
            if(c == -1) return -1; // Invalid
            if(To[Nod][c] == 0) To[Nod][c] = ++Cnt;
            Nod = To[Nod][c];
        }
        if(setId) {
            Ids[Nod] = setId;
            return 0;
        }
        if(Ids[Nod]) return -1; // Repeat
        return Ids[Nod] = ++Id;
    }
    int Query(char* begin, char* end) {
        int Nod = 1;
        for(char* s = begin; (end ? s != end : *s); ++s)
            Nod = To[Nod][toInt(*s)];
        return Nod; // 0 if a function/variable do not exist
    }
} variableTrie, funcionTrie;
namespace presetFunctions {
    const int SQRT = -1;
    const int LOG = -2;
    const int EXP = -3;
    const int SIN = -4;
    const int COS = -5;
}
struct SelfDefinedFunction {
    char equation[1000];
    char variable[50];
    SelfDefinedFunction() {
        memset(equation, 0, sizeof equation);
        memset(variable, 0, sizeof variable);
    }
} seflDefinedFunction[1000];
double selfDefinedVariable[1000];
inline void errorCheck() {
    bool position = false;
    switch(Error_in_bc.error_type) {
        case syntax_error:
            printf("syntax_error\n");
            position = true;
            break;
        case invalid_variable_name:
            printf("invalid_variable_name\n");
            break;
        case runtime_error:
            printf("runtime_error\n");
            break;
        case unknown_command:
            printf("unknown_command\n");
            break;
        case unknown_variable:
            printf("unknown_variable\n");
            position = true;
            break;
        case unknown_function:
            printf("unknown_function");
            position = true;
            break;
    }
    if(position) {
        printf("%s\n", S);
        for(int i = 0; i < Error_in_bc.pos - S; ++i)
            printf(" ");
        printf("^\n");
    }
}
inline void ShowHelp() {
    printf("1. Use #h for help, #q for quit(or you can quit the terminal yourself).\n");
    printf("2. Input a equation like 1*2+3^4 to calculate the result.\n");
    printf("3. Input a equation like x=1*2+3^4 to set a variable.\n");
    printf("4. Input a equation like f(x)=x^2+2*x+1 to set a function.\n");
    printf("5. You can also use: log(x), exp(x), sin(x), cos(x), sqrt(x).\n");
    return;
}
int operationPriority[300];
double Solve(char* begin, char* end) {
    if(begin == end) return 0; // Automatically turn -x into 0-x
    int minPriority = -1, leftBracket = 0;
    for(char* s = begin; s != end; ++s) {
        if(*s == '(') {
            ++leftBracket;
            continue;
        }
        if(*s == ')') {
            if(!leftBracket) {
                Error_in_bc = error_in_bc(syntax_error, s);
                return 1;
            }
            --leftBracket;
            continue;
        }
        if(operationPriority[*s])
            minPriority = min(minPriority, leftBracket * 100 + operationPriority[*s]);
    }
    if(minPriority == -1) { // No any operator
        // Single number
        // Tip: If we do not use sscanf here, the number will be low arruracy with too many *10 operation.
        double Ans;
        if(sscanf(begin, "%lf", &Ans)) {
            int decimalPoint = 0;
            for(char* s = begin; s != end; ++s) {
                if('0' <= *s && *s <= '9') continue;
                if(*s == '.') ++decimalPoint;
                if(decimalPoint == 2 || *s != '.') {
                    Error_in_bc = error_in_bc(syntax_error, s);
                    return 1;
                }
            }
            return Ans;
        }
        // Only brackets
        if(*begin == '(' && *(end - 1) == ')') return Solve(begin + 1, end - 1);
        // Single function
        char* leftBracketPos = 0;
        for(char* s = begin; s != end; ++s)
            if(*s == '(') {
                leftBracketPos = s;
                break;
            }
        if(leftBracketPos) {
            int functionId = funcionTrie.Query(begin, leftBracketPos);
            if(functionId == 0) {
                Error_in_bc = error_in_bc(unknown_variable, begin);
                return 1;
            }
            Ans = Solve(leftBracketPos + 1, end - 1); // end - 1 = rightBracketPos
            switch(functionId) {
                case presetFunctions::COS:
                    return cos(Ans);
                    break;
                case presetFunctions::SIN:
                    return sin(Ans);
                    break;
                case presetFunctions::EXP:
                    return exp(Ans);
                    break;
                case presetFunctions::LOG:
                    return log(Ans);
                    break;
                case presetFunctions::SQRT:
                    return sqrt(Ans);
                    break;
                default: // Self-defined function
                    // Give the variable in the function a temporary value
                    int variableId = variableTrie.Query(seflDefinedFunction[functionId].variable, 0), vid;
                    if(variableId == 0) vid = variableTrie.Insert(seflDefinedFunction[functionId].variable, 0, variableTrie.Id + 1);
                        else vid = variableId;
                    double reg = selfDefinedVariable[vid];
                    selfDefinedVariable[vid] = Solve(leftBracketPos + 1, end - 1);
                    // Ans = Solve(seflDefinedFunction[functionId].equation, seflDefinedFunction[functionId].equation + strlen(seflDefinedFunction[functionId].equation));
                    char* equ = seflDefinedFunction[functionId].equation;
                    Ans = Solve(equ, equ + strlen(equ));
                    // Remove the temporary value
                    variableTrie.Insert(seflDefinedFunction[functionId].variable, 0, variableId);
                    selfDefinedVariable[vid] = reg;
                    return Ans;
                    break;
            }
        }
        // Single variable
        int variableId = variableTrie.Query(begin, end);
        if(variableId) return selfDefinedVariable[variableId];
        Error_in_bc = error_in_bc(unknown_variable, begin);
        return 1;
    }
}
inline void Init() {
    operationPriority['+'] = 1;
    operationPriority['-'] = 1;
    operationPriority['*'] = 2;
    operationPriority['/'] = 2;
    operationPriority['%'] = 2;
    operationPriority['^'] = 3;
    funcionTrie.Insert("sqrt", 0, presetFunctions::SQRT);
    funcionTrie.Insert("cos", 0, presetFunctions::COS);
    funcionTrie.Insert("sin", 0, presetFunctions::SIN);
    funcionTrie.Insert("exp", 0, presetFunctions::EXP);
    funcionTrie.Insert("log", 0, presetFunctions::LOG);
}
int main() {
    Init();
    for(; true ; errorCheck()) {
        Error_in_bc.error_type = no_error;
        memset(S, 0, sizeof S);
        cin.getline(S, 1000);
        if(S[0] == '#') {
            if(S[1] == 'q') break;
            else if(S[1] == 'h') ShowHelp();
            else Error_in_bc.error_type = unknown_command;
            continue;
        }
        // Remove blanks and identify equation type
        int length = strlen(S);
        int numberOfBlanks = 0;
        bool isDefinitionType = false;
        for(int i = 0; i < length; ++i) {
            if(S[i] == '=') isDefinitionType = true;
            if(S[i] == ' ') ++numberOfBlanks;
                else S[i - numberOfBlanks] = S[i];
        }
        for(int i = length - numberOfBlanks; i < length; ++i)
            S[i] = '\0';
        // Solve the equation
        if(isDefinitionType) {
            char* equationPos = S; for(; *equationPos != '='; ++equationPos);
            if(equationPos == S) {
                Error_in_bc = error_in_bc(invalid_variable_name);
                continue;
            }
            int variableId = variableTrie.Query(S, equationPos);
        } else {
            double Ans = Solve(S, S + length);
            if(Error_in_bc.error_type == no_error) cout << Ans << endl;
        }
    }
    printf("Thank you for using!\n");
    return 0;
}