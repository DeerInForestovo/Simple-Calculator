#include "simbc.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <fstream>

char S[1000];

inline void showHelp() {
    std::cout << "\033[33m";
    printf("1. Use #h for help, #q for quit(or you can quit the terminal yourself).\n");
    printf("2. Input a equation like 1*2+3^4 to calculate the result.\n");
    printf("3. Input a equation like x=1*2+3^4 to set a variable.\n");
    printf("4. Input a equation like f(x)=x^2+2*x+1 to set a function.\n");
    printf("5. You can also use: log(x), exp(x), sqrt(x)... #a for all preset mathematical functions.\n");
    printf("6. Use #r to read a file.\n");
    std::cout << "\033[0m";
    return;
}

void showFunctions();

void init();

void calculate(char* S);

void removeBlanks(char *S) {
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
    return;
}

void readFile() {
    std::cout << "\033[33m";
    printf("Input the name of the file: \n");
    std::cout << "\033[0m";
    char fileName[100];
    memset(fileName, 0, sizeof fileName);
    std::cin >> fileName;
    std::ifstream file(fileName, std::ios::in);
    if(!file) {
        std::cout << "\033[31m";
        printf("Failed to open the file.\n");
        std::cout << "\033[0m";
        return;
    }
    while(true) {
        file.getline(S, 1000);
        if(strlen(S) == 0) break;
        removeBlanks(S);
        puts(S);
        calculate(S);
    }
    return;
}

int main() {
    std::cout << "\033[33m";
    printf("Welcome! #h to show help.\n");
    std::cout << "\033[0m";
    init();
    while(true) {
        memset(S, 0, sizeof S);
        std::cin.getline(S, 1000);
        if(S[0] == '#') {
            if(S[1] == 'q') break;
                else switch(S[1]) {
                    case 'h':
                        showHelp();
                        break;
                    case 'a':
                        showFunctions();
                        break;
                    case 'r':
                        readFile();
                        break;
                    default:
                        std::cout << "\033[31m";
                        printf("Error: unknown command\n");
                        std::cout << "\033[0m";
                        break;
                }
            continue;
        }
        if(!strlen(S)) continue;
        removeBlanks(S);
        calculate(S);
    }
    std::cout << "\033[32m";
    printf("Thank you for using!\n");
    std::cout << "\033[0m";
    return 0;
}