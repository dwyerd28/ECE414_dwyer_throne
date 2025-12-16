#include "pico/stdlib.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "coordsetup.h"
#include "limits.h"
#include "motor.h"
#include "servo.h"
#include "ui_lcd.h"

// FSM states
typedef enum {
    ST_BOOT = 0,
    ST_LOAD,
    ST_PEN_UP,
    ST_MOVE,       // travel with pen up
    ST_PEN_DOWN,
    ST_DRAW,       // draw with pen down
    ST_DONE
} State;

static const char *state_name(State s) {
    switch (s) {
        case ST_BOOT:     return "ST_BOOT";
        case ST_LOAD:     return "ST_LOAD";
        case ST_PEN_UP:   return "ST_PEN_UP";
        case ST_MOVE:     return "ST_MOVE";
        case ST_PEN_DOWN: return "ST_PEN_DOWN";
        case ST_DRAW:     return "ST_DRAW";
        case ST_DONE:     return "ST_DONE";
        default:          return "UNKNOWN";
    }
}

int main() {
    //initialize usb
    stdio_init_all();

    sleep_ms(500);  

    printf("\n=== CoreXY Drawing Robot Start ===\n");

    //initilize hardware subsystems
    limits_init();
    motor_init();
    servo_init();
    ui_lcd_init();

    //starts FSM
    State state = ST_BOOT;
    bool pen_is_down = false;

    size_t point_count = 0;
    size_t point_index = 0;

    //printf("[INIT] Finished hardware init, entering main loop.\n");

    while (1) {
       //delay
        sleep_ms(5);

        switch (state) {

            //intializes all the hardware and set up the robot for drawing
        case ST_BOOT:
            printf("[FSM] State = %s\n", state_name(state));
            printf("[BOOT] Transitioning to ST_LOAD.\n");
            state = ST_LOAD;
            break;

             //Loads the coordinate points that the robot will follow for the drawing.
        case ST_LOAD:
            printf("[FSM] State = %s\n", state_name(state));
            //gets number of drawing points
            point_count = coord_get_count();
            //starts at first point
            point_index = 0;
            printf("[LOAD] Path loaded with %u points.\n", (unsigned int)point_count);

            //resets lcd progress bar
            ui_lcd_update_progress(0, point_count);

            printf("[LOAD] Forcing pen up, going to ST_PEN_UP.\n");
            state = ST_PEN_UP;
            break;

             //Raises the pen using the servo so the robot can move without drawing.
        case ST_PEN_UP:
            printf("[FSM] State = %s\n", state_name(state));
            if (pen_is_down) {
                printf("[PEN_UP] Pen is currently DOWN, raising pen.\n");
                pen_up();
                pen_is_down = false;
            } else {
                printf("[PEN_UP] Pen already UP, nothing to do.\n");
            }
            printf("[PEN_UP] Moving to ST_MOVE.\n");
            state = ST_MOVE;
            break;

            //Moves the robot to the next starting point with the pen lifted.
        case ST_MOVE: {
            printf("[FSM] State = %s\n", state_name(state));

            //if weve done all the points, we are done
            if (point_index >= point_count) {
                printf("[MOVE] No more points (index=%u, count=%u). Going to ST_DONE.\n",
                       (unsigned int)point_index, (unsigned int)point_count);
                state = ST_DONE;
                break;
            }

            const DrawPoint *pt = coord_get_point(point_index);
            if (!pt) {
                printf("[MOVE] coord_get_point returned NULL at index %u. Going to ST_DONE.\n",
                       (unsigned int)point_index);
                state = ST_DONE;
                break;
            }

            printf("[MOVE] Point %u: (x=%.2f mm, y=%.2f mm, pen_down=%d)\n",
                   (unsigned int)point_index, pt->x_mm, pt->y_mm, pt->pen_down ? 1 : 0);

            // If the next point wants pen down, we should go to ST_PEN_DOWN instead
            if (pt->pen_down) {
                printf("[MOVE] Next segment requires pen DOWN. Switching to ST_PEN_DOWN.\n");
                state = ST_PEN_DOWN;
                break;
            }

            // Travel move (pen up)
            //checks limits as well
            if (!limits_within(pt->x_mm, pt->y_mm)) {
                printf("[LIMIT] Travel target OUT OF BOUNDS! (x=%.2f, y=%.2f)\n",
                       pt->x_mm, pt->y_mm);
                printf("[LIMIT] Emergency stop and transitioning to ST_DONE.\n");
                motor_emergency_stop();
                state = ST_DONE;
                break;
            }

            //moves motors to next coordinate with pen up
            printf("[MOVE] Moving (pen UP) to (%.2f, %.2f)\n", pt->x_mm, pt->y_mm);
            motor_move_to_mm(pt->x_mm, pt->y_mm);

            //move to next point
            point_index++;
            printf("[MOVE] Completed point %u of %u.\n",
                   (unsigned int)point_index, (unsigned int)point_count);

            ui_lcd_update_progress(point_index, point_count);
            // Stay in ST_MOVE to keep handling travel points
            break;
        }

            //Lowers the pen using the servo to prepare for drawing.
        case ST_PEN_DOWN:
            printf("[FSM] State = %s\n", state_name(state));
            if (!pen_is_down) {
                printf("[PEN_DOWN] Lowering pen.\n");
                pen_down();
                pen_is_down = true;
            } else {
                printf("[PEN_DOWN] Pen already DOWN.\n");
            }
            printf("[PEN_DOWN] Moving to ST_DRAW.\n");
            state = ST_DRAW;
            break;

            //Drives the motors to trace the shape between coordinate points.
        case ST_DRAW: {
            printf("[FSM] State = %s\n", state_name(state));

            //stop if all points have been done
            if (point_index >= point_count) {
                printf("[DRAW] No more points (index=%u, count=%u). Going to ST_DONE.\n",
                       (unsigned int)point_index, (unsigned int)point_count);
                state = ST_DONE;
                break;
            }

            const DrawPoint *pt = coord_get_point(point_index);
            if (!pt) {
                printf("[DRAW] coord_get_point returned NULL at index %u. Going to ST_DONE.\n",
                       (unsigned int)point_index);
                state = ST_DONE;
                break;
            }

            printf("[DRAW] Point %u: (x=%.2f mm, y=%.2f mm, pen_down=%d)\n",
                   (unsigned int)point_index, pt->x_mm, pt->y_mm, pt->pen_down ? 1 : 0);

            // If this point is a travel point (pen_up), switch back to ST_PEN_UP
            if (!pt->pen_down) {
                printf("[DRAW] Next segment is travel (pen UP). Switching to ST_PEN_UP.\n");
                state = ST_PEN_UP;
                break;
            }

            //checking limits
            if (!limits_within(pt->x_mm, pt->y_mm)) {
                printf("[LIMIT] DRAW target OUT OF BOUNDS! (x=%.2f, y=%.2f)\n",
                       pt->x_mm, pt->y_mm);
                printf("[LIMIT] Emergency stop and transitioning to ST_DONE.\n");
                motor_emergency_stop();
                state = ST_DONE;
                break;
            }

            //moves motor while drawing
            printf("[DRAW] Moving (pen DOWN) to (%.2f, %.2f)\n", pt->x_mm, pt->y_mm);
            motor_move_to_mm(pt->x_mm, pt->y_mm);

            //advance to next drawing point
            point_index++;
            printf("[DRAW] Completed point %u of %u.\n",
                   (unsigned int)point_index, (unsigned int)point_count);

            ui_lcd_update_progress(point_index, point_count);
            // Stay in ST_DRAW while pen_down points continue
            break;
        }

            //Stops all motion and indicates that the drawing is complete.
        case ST_DONE:
            printf("[FSM] State = %s\n", state_name(state));
            printf("[DONE] Drawing complete or stopped. Showing DONE message.\n");
            ui_lcd_show_done();

            if (pen_is_down) {
                printf("[DONE] Lifting pen.\n");
                pen_up();
                pen_is_down = false;
            }

            printf("[DONE] Entering idle loop.\n");
            while (1) {
                tight_loop_contents();
            }
            break;

        default:
            printf("[ERROR] Unknown state %d, forcing ST_DONE.\n", state);
            state = ST_DONE;
            break;
        }
    }

    return 0;
}



