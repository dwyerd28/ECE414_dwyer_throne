#include "pico/stdlib.h"
#include "sw_in.h"
#include <stdint.h>
#include <stdlib.h>
#include "led_out.h"
#include <stdbool.h>
#include "timer.h"
#include "debounce_sw1.h"
#include "debounce_sw2.h"
#include "hardware/uart.h"
#include <stdio.h>

#define UART_ID uart0
#define BAUD_RATE 115200

#define UART_TX_PIN 0
#define UART_RX_PIN 1

// FSM States
enum PONG_States { 
    Init, 
    Serve, 
    Travel, 
    Thwack, 
    Victory 
} PONG_State;

// Player enum
enum currentPlayer { 
    PlayerL = 0, 
    PlayerR = 1 
} currentPlayer;


uint8_t  ledsStates = 0;            // 8-bit value for LED control
uint8_t  ledsStatesCounter = 0;     // Counter for multi-tick actions (victory flashes)
uint32_t t1, t2, t3, t4;            // Timers for debounce and game timing
uint32_t gameTimer = 300;           // Game speed timer (starts at 300ms)
uint8_t  numberOfRoundsPlayed = 0;  // Track rounds for speed increase
uint32_t randValue;


static uint8_t ballPos = 0;         
static int8_t  ballDir = 0;         
static uint8_t volleysThisRound = 0;

#define LEFT_IDX   7   
#define RIGHT_IDX  0   


static inline bool seeded_rng(void);
static inline void seed_rng(void);
static inline void set_ball(uint8_t idx);


void tick()
{
    switch (PONG_State)
    {
        case Init:
        {
            if (!seeded_rng()) {
                seed_rng();
            }
            randValue = (uint32_t)(rand() & 1);
            if (randValue != 0u) {
                currentPlayer = PlayerL;
            } else {
                currentPlayer = PlayerR;
            }
            volleysThisRound = 0;
            gameTimer = 300;

            if (currentPlayer == PlayerL) {
                set_ball(LEFT_IDX);
            } else {
                set_ball(RIGHT_IDX);
            }

            PONG_State = Serve;
        } break;

        case Serve:
        {
            bool servePressed = false;
            if (currentPlayer == PlayerL) {
                servePressed = debounce_sw1_pressed();
            } else {
                servePressed = debounce_sw2_pressed();
            }

            bool wrongPressed = false;
            if (currentPlayer == PlayerL) {
                wrongPressed = debounce_sw2_pressed();
            } else {
                wrongPressed = debounce_sw1_pressed();
            }

            if (wrongPressed) {
                ledsStatesCounter = 0;
                PONG_State = Victory;
            } else {
                if (servePressed) {
                    if (currentPlayer == PlayerL) {
                        ballDir = -1;   
                        set_ball(LEFT_IDX);
                    } else {
                        ballDir = +1;   
                        set_ball(RIGHT_IDX);
                    }
                    PONG_State = Travel;
                }
            }
        } break;

        case Travel:
        {
            int next = (int)ballPos + ballDir;
            if (next < RIGHT_IDX) {
                next = RIGHT_IDX;
            }
            if (next > LEFT_IDX) {
                next = LEFT_IDX;
            }
            set_ball((uint8_t)next);

            if (ballPos == LEFT_IDX || ballPos == RIGHT_IDX) {
                PONG_State = Thwack;
            } else {
                bool lp = debounce_sw1_pressed();
                bool rp = debounce_sw2_pressed();
                if (lp || rp) {
                    ledsStatesCounter = 0;
                    PONG_State = Victory;
                }
            }
        } break;

        case Thwack:
        {
            bool atLeft  = (ballPos == LEFT_IDX);
            bool atRight = (ballPos == RIGHT_IDX);

            bool lp = debounce_sw1_pressed();
            bool rp = debounce_sw2_pressed();

            
            if (atLeft && rp) {
                ledsStatesCounter = 0;
                PONG_State = Victory;
                break;
            }
            if (atRight && lp) {
                ledsStatesCounter = 0;
                PONG_State = Victory;
                break;
            }

            
            if ((atLeft && lp) || (atRight && rp)) {
                ballDir = -ballDir;
                volleysThisRound++;

                if (volleysThisRound >= 5) {
                    gameTimer = 100;
                } else if (volleysThisRound >= 3) {
                    gameTimer = 200;
                } else {
                    gameTimer = 300;
                }

                if (currentPlayer == PlayerL) {
                    currentPlayer = PlayerR;
                } else {
                    currentPlayer = PlayerL;
                }

                PONG_State = Travel;
                break;
            }

            
            ledsStatesCounter = 0;
            PONG_State = Victory;
        } break;

        case Victory:
        {
            const uint8_t flashesNeeded = 6; 
            if ((ledsStatesCounter & 1u) == 0u) {
                enum currentPlayer winner;
                if (currentPlayer == PlayerL) {
                    winner = PlayerR;
                } else  {
                    winner = PlayerL;
                }

                if (winner == PlayerL) {
                    ledsStates = (1u << LEFT_IDX);
                } else {
                    ledsStates = (1u << RIGHT_IDX);
                }
            } else {
                ledsStates = 0x00;
            }

            led_out_write(ledsStates);
            ledsStatesCounter++;

            if (ledsStatesCounter >= flashesNeeded) {
                numberOfRoundsPlayed++;
                PONG_State = Init;
            }
        } break;

        default:
        
            break;
    }
}


void initializeStuff()
{
    led_out_init();
    sw_in_init();
    debounce_sw1_init();
    debounce_sw2_init();

    t1 = timer_read();          
    t2 = timer_read();           
    t3 = t4 = t2;

   
}


int main()
{
    initializeStuff();
    PONG_State = Init;

    while (1)
    {
        
        if (timer_elapsed_ms(t1, timer_read()) >= DEBOUNCE_PD_MS) {
            debounce_sw1_tick();
            debounce_sw2_tick();
            t1 = timer_read();
        }

        
        if (timer_elapsed_ms(t2, timer_read()) >= gameTimer) {
            tick();
            t2 = timer_read();
        }

        tight_loop_contents();
    }
}

static bool _rng_seeded = false;

static inline bool seeded_rng(void) {
    return _rng_seeded;
}

static inline void seed_rng(void) {
    srand(timer_read());
    _rng_seeded = true;
}

static inline void set_ball(uint8_t idx) {
    if (idx > 7) {
        idx = 7;
    }
    ballPos = idx;
    ledsStates = (1u << ballPos);
    led_out_write(ledsStates);
}




