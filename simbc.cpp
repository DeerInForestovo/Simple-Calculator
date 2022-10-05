#include "simbc.h"
#include <cstring>
#include <cmath>
#include <iostream>
#include <cstdio>
#include <functional>

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
    int Insert(char* begin, char* end, int setId = -999) {
        int Nod = 1;
        for(char* s = begin; (end ? s != end : *s); ++s) {
            int c = toInt(*s);
            if(c == -1) return -1; // Invalid
            if(To[Nod][c] == 0) To[Nod][c] = ++Cnt;
            Nod = To[Nod][c];
        }
        if(setId != -999) {
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
        return Ids[Nod]; // 0 if a function/variable do not exist
    }
} variableTrie, funcionTrie;

int presetFunctionId;
std::function<double(double)> presetFunctions[100];

struct SelfDefinedFunction {
    char equation[1000];
    char variable[50];
    SelfDefinedFunction() {
        memset(equation, 0, sizeof equation);
        memset(variable, 0, sizeof variable);
    }
} selfDefinedFunction[1000];
double selfDefinedVariable[1000];
bool inlineFuncion[1000]; // If an inline function is visited, a recursion occurs.

int operationPriority[300];
int VITRUE_ID = 999;

double Solve(char* begin, char* end) {
    if(begin == end) return 0; // Automatically turn -x into 0 - x
    
    int minPriority = 1 << 30, leftBracket = 0;
    static const int BRACKET_PRIORITY = 100;
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
            minPriority = std::min(minPriority, leftBracket * BRACKET_PRIORITY + operationPriority[*s]);
    }
    if(leftBracket) {
        char *s;
        for(s = end; *(--s) != '(';); // Point to the last '('
        Error_in_bc = error_in_bc(syntax_error, s);
        return 1;
    }

    if(minPriority >= BRACKET_PRIORITY) { // No any operator out of a bracket

        // Single number
        // Tip: If we do not use sscanf here, the number will be low arruracy with too many *10 operation.
        double ans;
        if(sscanf(begin, "%lf", &ans)) { // This number should be like 123 or 123.45, no 'e' or anything else.
            int decimalPoint = 0;
            for(char* s = begin; s != end; ++s) {
                if('0' <= *s && *s <= '9') continue;
                if(*s == '.') ++decimalPoint;
                if(decimalPoint == 2 || *s != '.') {
                    Error_in_bc = error_in_bc(syntax_error, s);
                    return 1;
                }
            }
            return ans;
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
                Error_in_bc = error_in_bc(unknown_function, begin);
                return 1;
            }
            if(inlineFuncion[functionId]) {
                Error_in_bc = error_in_bc(recursion, begin);
                return 1;
            }

            // Calcluate the variable in the function
            ans = Solve(leftBracketPos + 1, end - 1); // end - 1 = rightBracketPos
            if(Error_in_bc.error_type != no_error) return 1;
            
            // Preset funcion
            if(functionId < 0) return presetFunctions[-functionId](ans);
            
            // Self-defined function
            // Give the variable in the function a temporary value
            int variableId = variableTrie.Query(selfDefinedFunction[functionId].variable, 0);
            --VITRUE_ID;
            variableTrie.Insert(selfDefinedFunction[functionId].variable, 0, VITRUE_ID);
            selfDefinedVariable[VITRUE_ID] = ans;

            // Calculate with definition equation
            // ans = Solve(selfDefinedFunction[functionId].equation, selfDefinedFunction[functionId].equation + strlen(selfDefinedFunction[functionId].equation));
            char* equ = selfDefinedFunction[functionId].equation;
            inlineFuncion[functionId] = true;
            ans = Solve(equ, equ + strlen(equ));
            if(Error_in_bc.error_type != no_error) Error_in_bc.pos = begin; // Modify the error position

            // Remove the temporary value
            ++VITRUE_ID;
            variableTrie.Insert(selfDefinedFunction[functionId].variable, 0, variableId);
            inlineFuncion[functionId] = false;

            return ans;
        }

        // Single variable
        int variableId = variableTrie.Query(begin, end);
        if(variableId) return selfDefinedVariable[variableId];
        Error_in_bc = error_in_bc(unknown_variable, begin);
        return 1;

    } else { // Calculate with operators
        double ans, now;
        bool first = true;
        char* last = begin;
        char lastOperator;
        for(char* s = begin; s != end; ++s) {
            if(*s == '(') ++leftBracket;
            if(*s == ')') --leftBracket;
            if(minPriority == leftBracket * BRACKET_PRIORITY + operationPriority[*s] || s == end - 1) {
                now = Solve(last, s + (operationPriority[*s] == 0));
                if(Error_in_bc.error_type != no_error) return 1;
                if(first) {
                    first = false;
                    ans = now;
                } else {
                    switch(lastOperator) {
                        case '+':
                            ans += now;
                            break;
                        case '-':
                            ans -= now;
                            break;
                        case '*':
                            ans *= now;
                            break;
                        case '/':
                            if(fabs(now) <= 1e-14) {
                                Error_in_bc = error_in_bc(runtime_error, last - 1);
                                return 1;
                            }
                            ans /= now;
                            break;
                        case '%':
                            if((long long)now <= 0) {
                                Error_in_bc = error_in_bc(runtime_error, last - 1);
                                return 1;
                            }
                            ans = (long long)ans % (long long)now;
                            break;
                        case '^':
                            ans = pow(ans, now);
                            break;
                        default:
                            Error_in_bc = error_in_bc(syntax_error, last - 1);
                            return 1;
                            break;
                    }
                }
                lastOperator = *s;
                last = s + 1;
            }
        }
        return ans;
    }
}

inline void addFunction(char* name, std::function<double(double)> F) {
    funcionTrie.Insert(name, 0, --presetFunctionId);
    presetFunctions[-presetFunctionId] = F;
}

double sec(double x) { return 1 / cos(x); }
double csc(double x) { return 1 / sin(x); }
double cot(double x) { return 1 / tan(x); }
void Init() {
    operationPriority['+'] = operationPriority['-'] = 1;
    operationPriority['*'] = operationPriority['/'] = operationPriority['%'] = 2;
    operationPriority['^'] = 3;
    
    // Add more funcion here easily
    addFunction("sqrt", sqrt);
    addFunction("log", log);
    addFunction("exp", exp);
    addFunction("sin", sin);
    addFunction("cos", cos);
    addFunction("fabs", fabs);
    addFunction("abs", abs);
    addFunction("tan", tan);
    addFunction("acos", acos);
    addFunction("asin", asin);
    addFunction("atan", atan);
    addFunction("sec", sec);
    addFunction("csc", csc);
    addFunction("cot", cot);
    addFunction("ceil", ceil);
    addFunction("floor", floor);
    addFunction("round", round);

}

inline void errorCheck(char* S) {
    bool position = false;
    switch(Error_in_bc.error_type) {
        case syntax_error:
            printf("syntax error\n");
            position = true;
            break;
        case invalid_variable_name:
            printf("invalid variable name\n");
            break;
        case invalid_function_name:
            printf("invalid function name\n");
            break;
        case runtime_error:
            printf("runtime error\n");
            position = true;
            break;
        case unknown_command:
            printf("unknown command\n");
            break;
        case unknown_variable:
            printf("unknown variable\n");
            position = true;
            break;
        case unknown_function:
            printf("unknown function");
            position = true;
            break;
        case recursion:
            printf("recursion\n");
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

void Calculate(char* S) {
    Error_in_bc.error_type = no_error;
    int length = strlen(S);
    int numberOfBlanks = 0;
    bool isDefinitionType = false;
    bool isFunctionDefinitionType = false;
    for(int i = 0; i < length; ++i) {
        if(S[i] == '(' && !isDefinitionType) isFunctionDefinitionType = true; // If there is a '(' before '='
        if(S[i] == '=') isDefinitionType = true;
        if(S[i] == ' ') ++numberOfBlanks;
            else S[i - numberOfBlanks] = S[i];
    }
    for(int i = length - numberOfBlanks; i < length; ++i)
        S[i] = '\0';
    // Solve the equation
    if(isDefinitionType) {
        char name[100];
        memset(name, 0, sizeof name);
        if(isFunctionDefinitionType) {
            sscanf(S, "%[^(]", name);
            int fid = funcionTrie.Insert(name, 0);
            if(fid == -1) Error_in_bc.error_type = invalid_function_name;
                else sscanf(S, "%[^(]%*1[(]%[^)]%*1[)]%*1[=]%s", name, selfDefinedFunction[fid].variable, selfDefinedFunction[fid].equation);
        } else {
            sscanf(S, "%[^=]", name);
            char* equationPos = S;
            while(*equationPos != '=') ++equationPos;
            double value = Solve(equationPos + 1, S + length);
            if(Error_in_bc.error_type == no_error) {
                int vid = variableTrie.Insert(name, 0);
                if(vid == -1) Error_in_bc.error_type = invalid_variable_name;
                    else selfDefinedVariable[vid] = value;
            }
        }
    } else {
        double ans = Solve(S, S + length);
        if(Error_in_bc.error_type == no_error) std::cout << ans << std::endl;
    }
    errorCheck(S);
}