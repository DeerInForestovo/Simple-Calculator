#include "simbc.h"
#include <algorithm>
using namespace std;

Number operator + (Number A, Number B) {
    if(A.length > B.length) swap(A, B);
    
}