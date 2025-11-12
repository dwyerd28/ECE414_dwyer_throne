#include "pico/stdlib.h"
#include <stdio.h>
#include <math.h>

#include "config.h"
#include "coord_setup.h"
#include "limits.h"
#include "motor.h"

//FSM
typedef enum {
    ST_BOOT = 0,
    ST_LOAD,
    ST_PEN_UP,
    ST_MOVE,       // travel with pen up
    ST_PEN_DOWN,
    ST_DRAW,       // draw with pen down
    ST_DONE
} State;

// helpers
static void get_pose_mm(float* x_mm, float* y_mm) {
    MotorPose p = motor_get_pose();  // steps
    float sx = coord_steps_per_mm_x();
    float sy = coord_steps_per_mm_y();
    if (sx <= 0.0f) sx = 1.0f;
    if (sy <= 0.0f) sy = 1.0f;
    *x_mm = ((float)p.x_steps) / sx;
    *y_mm = ((float)p.y_steps) / sy;
}


static float distance_to_target_mm(const Point* pt) {
    float cx, cy;
    get_pose_mm(&cx, &cy);
    float dx = ((float)pt->x) - cx;
    float dy = ((float)pt->y) - cy;
    float d2 = dx*dx + dy*dy;
    float d  = sqrtf(d2);
    return d;
}

int main(void) {
    stdio_init_all();
    sleep_ms(200); 

    //init subsystems 
    motor_init();
    motor_pen_init();
    limits_init(X_MIN_MM, X_MAX_MM, Y_MIN_MM, Y_MAX_MM);

    // loads Kdemo path
    size_t path_len = 0;
    const Point* path = coord_get_path(&path_len);
    size_t idx = 0;  

    // FSM start
    State st = ST_BOOT;

    while (true) {
        switch (st) {

            case ST_BOOT:
                // Raise pen at boot and start at first path element
                motor_pen_up();
                sleep_ms(PEN_SETTLE_MS);
                idx = 0;
                st = ST_LOAD;
                break;

            case ST_LOAD:
                // If we've exhausted all points -> DONE
                if (idx >= path_len) {
                    st = ST_DONE;
                    break;
                }

                // If the *last* point was processed and idx points beyond end -> DONE
                if (idx == path_len - 1) {
                    // nextCoord == end in your diagram
                    st = ST_DONE;
                    break;
                }

                // Otherwise, decide whether the next segment is travel or draw
                {
                    const Point* next = &path[idx + 0];  // current target
                    float dist = distance_to_target_mm(next);

                    // Safety: if target is out-of-bounds, skip it
                    if (!limits_point_ok_mm((float)next->x, (float)next->y)) {
                        idx = idx + 1;
                        st = ST_LOAD;
                        break;
                    }

                   
                    //  - if distance > threshold => travel (pen up)
                    //  - else if draw flag true  => draw (pen down)
                    //  - else                     => travel
                    if (dist > COORD_DIFF_THRESH_MM) {
                        st = ST_PEN_UP;
                    } else {
                        if (next->draw) {
                            st = ST_PEN_DOWN;
                        } else {
                            st = ST_PEN_UP;
                        }
                    }
                }
                break;

            case ST_PEN_UP:
                motor_pen_up();
                sleep_ms(PEN_SETTLE_MS);
                st = ST_MOVE;   
                break;

            case ST_MOVE:
                // Travel to the next coordinate (pen is up)
                if (idx < path_len) {
                    const Point* tgt = &path[idx];
                    motor_move_to_mm((float)tgt->x, (float)tgt->y);
                    idx = idx + 1;  
                }
                st = ST_LOAD;        // decide next step
                break;

            case ST_PEN_DOWN:
                motor_pen_down();
                sleep_ms(PEN_SETTLE_MS);
                st = ST_DRAW;   // draw with pen down
                break;

            case ST_DRAW:
                // Draw to the next coordinate 
                if (idx < path_len) {
                    const Point* tgt = &path[idx];
                    motor_move_to_mm((float)tgt->x, (float)tgt->y);
                    idx = idx + 1;   
                }
                st = ST_LOAD;        // decide next step
                break;

            case ST_DONE:
                // Finished the path: lift pen and idle
                motor_pen_up();
                sleep_ms(250);
                while (1) { tight_loop_contents(); } // park forever
                // no break — parked
        }

        
        sleep_ms(FSM_TICK_MS);
    }
    
    return 0;
}
