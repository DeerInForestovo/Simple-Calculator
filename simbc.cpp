#include "simbc.h"
#include <iostream>
#include <algorithm>

error_in_bc Error_in_bc;

void input(Number* n, char* begin, char* end) {
    n = new Number();
    if(begin == end) return;
    // Input the number
    if(*begin == '-') {
        n -> sign = true;
        ++begin;
    } else if(*begin == '+') ++begin;
    bool decimalPoint = false; // If there is a decimal point
    while(('0' <= *begin && *begin <= '9') || *begin == '.' && begin != end)
        if(*begin == '.') {
            if(decimalPoint) Error_in_bc = error_in_bc(syntax_error, begin); // More than one decimal points
            decimalPoint = true;
            ++begin;
        } else {
            if(decimalPoint) --n -> power;
            n -> number[n -> length++] = *begin - '0';
            ++begin;
        }
    if(begin != end) Error_in_bc = error_in_bc(syntax_error, begin);
    // Delete suffix zeros
    n -> deleteSuffixZeros();
    // Delete leading zeros
    int leadingZeros = 0;
    bool leading = true;
    for(int i = 0; i < n -> length; ++i)
        if(leading && n -> number[i] == 0)
            ++leadingZeros;
        else {
            if(leading) {
                leading = false;
                if(leadingZeros == 0) break; // Nothing to delete
            }
            n -> number[i - leadingZeros] = n -> number[i];
            n -> number[i] = 0;
        }
    n -> length -= leadingZeros;
}

void Output(Number* n) {
    n -> deleteSuffixZeros();
    if(n -> length == 0) {
        printf("0");
        return;
    }
    if(n -> sign) printf("-");
    if(-5 <= n -> power + n -> length && n -> power + n -> length <= 9) {
        // It will be more natural to output directly.
        if(n -> power >= 0) {
            // No decimal point
            for(int i = 0; i < n -> length; ++i)
                printf("%d", n -> number[i]);
            for(int i = 0; i < n -> power; ++i)
                printf("0");
        } else if(n -> power + n -> length > 0) {
            // Decimal point is in the number
            for(int i = 0; i < n -> power + n -> length; ++i)
                printf("%d", n -> number[i]);
            printf(".");
            for(int i = n -> power + n -> length; i < n -> length; ++i)
                printf("%d", n -> number[i]);
        } else {
            // Decimal point is out of number, i.e., 0.000xxx
            printf("0.");
            for(int i = 0; i < -(n -> power + n -> length); ++i)
                printf("0");
            for(int i = 0; i < n -> length; ++i)
                printf("%d", n -> number[i]);
        }
    } else {
        // Output with scientific notation
        if(n -> length == 1) {
            // No decimal point
            printf("%de%d", n -> number[0], n -> power);
        } else {
            printf("%d.", n -> number[0]);
            for(int i = 1; i < n -> length; ++i)
                printf("%d", n -> number[i]);
            printf("e%d", n -> power + n -> length - 1);
        }
    }
}

Number operator + (Number A, Number B) {
    Number S;
    S.length = std::max(A.length, B.length);
    for(int i = 0; i < S.length; ++i)
        S.number[i] = A.number[i] + B.number[i];
    for(int i = 0; i < S.length; ++i)
        if(S.number[i] > 9) {
            if(i == S.length - 1) ++S.length;
            S.number[i + 1] += S.number[i] / 10;
            S.number[i] %= 10;
        }
    return S;
}

Number operator - (Number A, Number B) {
    Number S;
    
}