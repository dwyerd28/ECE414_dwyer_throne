#include "pico/stdlib.h"
#include "TouchDisplay.h"
#include "inputnum.h"
#include "ts_lcd.h"
#include "TouchDisplay.c"

#include <stdio.h>

typedef enum {
    CALC_INIT = 0,
    CALC_FIRST_NUM,
    CALC_OPERATOR,
    CALC_SECOND_NUM,
    CALC_SHOW_RESULT
} CALC_STATES;

static CALC_STATES CALC_State;

static int32_t num1 = 0, num2 = 0, result = 0;
static OPERATOR currentOp = ERR;

int main() {
    stdio_init_all();
    ts_lcd_init();
    

    CALC_State = CALC_INIT;
    drawCalcScreen();
    displayResult(0);

    while (1) {
        uint16_t touch_x = 0, touch_y = 0;
        Buttons pressedButton = BERR;

        if (get_ts_lcd(&touch_x, &touch_y)) {

    pressedButton = getButton(touch_x, touch_y);

    printf("Touch at: %d, %d -> Button: %d\n", touch_x, touch_y, pressedButton);

}

        if (get_ts_lcd(&touch_x, &touch_y)) {
            pressedButton = getButton(touch_x, touch_y);
            printf("Touch at: %u, %u -> Button: %d\n", touch_x, touch_y, pressedButton);
        }

        switch (CALC_State) {
            case CALC_INIT:
                if (pressNumber(pressedButton)) {
                    clear();
                    num1 = getInput(pressedButton);
                    displayResult(num1);
                    CALC_State = CALC_FIRST_NUM;
                } else if (pressC(pressedButton)) {
                    clear();
                    displayResult(0);
                }
                break;

            case CALC_FIRST_NUM:
                if (pressNumber(pressedButton)) {
                    num1 = getInput(pressedButton);
                    displayResult(num1);
                } else if (pressOperator(pressedButton)) {
                    currentOp = readOperator(pressedButton);
                    clear();
                    CALC_State = CALC_OPERATOR;
                } else if (pressC(pressedButton)) {
                    clear();
                    displayResult(0);
                    CALC_State = CALC_INIT;
                }
                break;

            case CALC_OPERATOR:
                if (pressNumber(pressedButton)) {
                    num2 = getInput(pressedButton);
                    displayResult(num2);
                    CALC_State = CALC_SECOND_NUM;
                } else if (pressC(pressedButton)) {
                    clear();
                    displayResult(0);
                    CALC_State = CALC_INIT;
                }
                break;

            case CALC_SECOND_NUM:
                if (pressNumber(pressedButton)) {
                    num2 = getInput(pressedButton);
                    displayResult(num2);
                } else if (pressEq(pressedButton)) {
                    result = performOperation(num1, num2, currentOp);
                    if (currentOp == DIV && num2 == 0) {
                        displayError();     // div-by-zero
                    } else if (overLimitResult(result)) {
                        displayError();     // sentinel overflow / error
                    } else {
                        displayResult(result);
                    }
                    CALC_State = CALC_SHOW_RESULT;
                } else if (pressC(pressedButton)) {
                    clear();
                    displayResult(0);
                    CALC_State = CALC_INIT;
                }
                break;

            case CALC_SHOW_RESULT:
                if (pressC(pressedButton)) {
                    // reset all
                    num1 = num2 = result = 0;
                    currentOp = ERR;
                    clear();
                    drawCalcScreen();
                    displayResult(0);
                    CALC_State = CALC_INIT;
                }
                break;
        }

        sleep_ms(50);
    }
}


