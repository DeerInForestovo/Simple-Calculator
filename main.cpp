#include "simbc.h"
#include <iostream>
#include <cstring>
#include <cstdio>
using namespace std;
char S[1000];
error_in_bc Error_in_bc;
inline void errorCheck() {
    bool position = false;
    switch(Error_in_bc.error_type) {
        case syntax_error:
            printf("syntax_error\n");
            position = true;
            break;
        case invalid_variable_name:
            printf("invalid_variable_name\n");
            position = true;
            break;
        case runtime_error:
            printf("runtime_error\n");
            break;
        case unknown_command:
            printf("unknown_command\n");
            break;
        case unknown_variable:
            position = true;
            printf("unknown_variable\n");
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
double Solve(char *begin, char* end) {
    Error_in_bc.error_type = syntax_error;
    Error_in_bc.pos = begin + 5;
}
int main() {
    for(; true ; errorCheck()) {
        Error_in_bc.error_type = no_error;
        cin.getline(S, 1000);
        if(S[0] == '#') {
            if(S[1] == 'q') break;
            else if(S[1] == 'h') ShowHelp();
            else Error_in_bc.error_type = unknown_command;
            continue;
        }
        int numberOfBlanks = 0;
        int length = strlen(S);
        bool isDefinitionType = false;
        for(int i = 0; i < length; ++i) {
            if(S[i] == '=') isDefinitionType = true;
            if(S[i] == ' ') ++numberOfBlanks;
                else S[i - numberOfBlanks] = S[i];
        }
        for(int i = length - numberOfBlanks; i < length; ++i)
            S[i] = '\0';
        if(isDefinitionType) {

        } else {
            double Ans = Solve(S, S + length);
            if(Error_in_bc.error_type == no_error) cout << Ans << endl;
        }
    }
    printf("Thank you for using!\n");
    return 0;
}