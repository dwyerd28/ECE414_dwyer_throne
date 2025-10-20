#include "inputnum.h"
#include <limits.h>

static int32_t currentInput = 0;

void clear(void) {
    currentInput = 0;
}

int32_t getInput(Buttons button) {
    
    switch (button) {
        case B0: currentInput = currentInput * 10 + 0; break;
        case B1: currentInput = currentInput * 10 + 1; break;
        case B2: currentInput = currentInput * 10 + 2; break;
        case B3: currentInput = currentInput * 10 + 3; break;
        case B4: currentInput = currentInput * 10 + 4; break;
        case B5: currentInput = currentInput * 10 + 5; break;
        case B6: currentInput = currentInput * 10 + 6; break;
        case B7: currentInput = currentInput * 10 + 7; break;
        case B8: currentInput = currentInput * 10 + 8; break;
        case B9: currentInput = currentInput * 10 + 9; break;
        default: break;
    }
    return currentInput;
}

int32_t performOperation(int32_t n1, int32_t n2, OPERATOR op) {
    switch (op) {
        case PLUS:  return n1 + n2;
        case MINUS: return n1 - n2;
        case MULT:  return n1 * n2;
        case DIV:
            if (n2 == 0) return INT32_MAX;  
            return n1 / n2;                 
        default:   return 0;
    }
}

bool overLimitResult(int32_t num) {
   
    return (num == INT32_MAX);
}


bool pressC(Buttons button) { return (button == BCL); }
bool pressEq(Buttons button) { return (button == BEQ); }
bool pressOperator(Buttons button) {
    return (button == BPLUS || button == BMINUS || button == BMULT || button == BDIV);
}
bool pressNumber(Buttons button) {
    return (button >= B0 && button <= B9);
}

OPERATOR readOperator(Buttons button) {
    switch (button) {
        case BPLUS:  return PLUS;
        case BMINUS: return MINUS;
        case BMULT:  return MULT;
        case BDIV:   return DIV;
        default:     return ERR;
    }
}


