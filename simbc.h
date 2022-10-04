#include <cstring>

const int MaxN = 1005;
struct Number {
    int length;        // Valid length of number[]. Length = 0 means the number is actually zero
    int number[MaxN];  // Integer part of the number
    int power;         // Power of 10. This should not be too big
    bool sign;         // isNegitiveNumber
    Number() {         // Initialize
        memset(number, 0, sizeof number);
        length = power = 0;
        sign = false;
    }
    inline void deleteSuffixZeros() {
        while(length && number[length - 1] == 0) {
            --length;
            ++power;
        }
    }
};

Number operator + (Number A, Number B);
Number operator - (Number A, Number B);
Number operator * (Number A, Number B);
Number operator / (Number A, Number B);
Number operator % (Number A, Number B);
Number operator ^ (Number A, Number B);

void Input(Number* n, char* begin, char* end);

enum errors_in_bc {
    no_error,
    syntax_error,
    invalid_variable_name,
    runtime_error,
    unknown_command,
    unknown_variable
};
struct error_in_bc {
    errors_in_bc error_type;
    char* pos;
    error_in_bc(errors_in_bc e, char* p) {
        error_type = e;
        pos = p;
    }
};