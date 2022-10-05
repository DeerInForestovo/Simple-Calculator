#include "simbc.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>

char S[1000];

inline void ShowHelp() {
    printf("1. Use #h for help, #q for quit(or you can quit the terminal yourself).\n");
    printf("2. Input a equation like 1*2+3^4 to calculate the result.\n");
    printf("3. Input a equation like x=1*2+3^4 to set a variable.\n");
    printf("4. Input a equation like f(x)=x^2+2*x+1 to set a function.\n");
    printf("5. You can also use: log(x), exp(x), sin(x), cos(x), sqrt(x).\n");
    return;
}

void Init();

void Calculate(char* S);

int main() {
    Init();
    while(true) {
        memset(S, 0, sizeof S);
        cin.getline(S, 1000);
        if(S[0] == '#') {
            if(S[1] == 'q') break;
            else if(S[1] == 'h') ShowHelp();
            else printf("unknown command\n");
            continue;
        }
        // Remove blanks
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
        Calculate(S);
    }
    printf("Thank you for using!\n");
    return 0;
}