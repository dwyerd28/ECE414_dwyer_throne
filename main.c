#include "pico/stdlib.h"
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include "config.h"
#include "coord_setup.h"
#include "limits.h"
#include "motor.h"
#include "lcd_progress.h"

// FSM States
typedef enum {
    ST_BOOT = 0,
    ST_LOAD,
    ST_PEN_UP,
    //travels with pen up
    ST_MOVE,       
    ST_PEN_DOWN,
    //travels with pen down
    ST_DRAW,      
    ST_DONE
} State;

// Gets current postiton in steps to mm 

static void get_pose_mm(float* x_mm, float* y_mm) {
    MotorPose p = motor_get_pose();  // position in steps
    float sx = coord_steps_per_mm_x();
    float sy = coord_steps_per_mm_y();

    if (sx <= 0.0f) {
        sx = 1.0f;
    }
    if (sy <= 0.0f) {
        sy = 1.0f;
    }

    *x_mm = ((float)p.x_steps) / sx;
    *y_mm = ((float)p.y_steps) / sy;
}

// Distance (mm) from current pose to a target point
static float distance_to_target_mm(const Point* pt) {
    float cx;
    float cy;
    get_pose_mm(&cx, &cy);

    float dx = ((float)pt->x) - cx;
    float dy = ((float)pt->y) - cy;

    float d2 = dx * dx + dy * dy;
    float d  = sqrtf(d2);

    return d;
}

int main(void) {
    stdio_init_all();
    sleep_ms(200); 

   //stepper motors (CoreXY)
    motor_init();   
    //servo on, pen up   
    motor_pen_init();  
    limits_init(X_MIN_MM, X_MAX_MM, Y_MIN_MM, Y_MAX_MM);

    // Loads my drawing path
    size_t path_len = 0;
    const Point* path = coord_get_path(&path_len);

    // next point to go
    size_t idx = 0;

   
    lcd_progress_init(path_len);
    lcd_progress_update(idx, path_len);   

    // FSM bootstrap
    State st = ST_BOOT;

    while (true) {
        switch (st) {

            case ST_BOOT:
                // Raise pen and go to the first state
                motor_pen_up();
                sleep_ms(PEN_SETTLE_MS);
                idx = 0;
                lcd_progress_update(idx, path_len);
                st = ST_LOAD;
                break;

            case ST_LOAD:
                // If we've processed all points, we're done
                if (idx >= path_len) {
                    st = ST_DONE;
                    break;
                }

                {
                    const Point* next = &path[idx];

                    // Skip any out-of-bounds points
                    if (!limits_point_ok_mm((float)next->x, (float)next->y)) {
                        idx = idx + 1;
                        lcd_progress_update(idx, path_len);
                        st = ST_LOAD;
                        break;
                    }

                    //Decides if it should be travel or draw based on distance
                    float dist = distance_to_target_mm(next);

                    if (dist > COORD_DIFF_THRESH_MM) {
                        // If far away it travels with pen up
                        st = ST_PEN_UP;
                    } else {
                        if (next->draw) {
                            // If close and at drawing point we draw with pen down
                            st = ST_PEN_DOWN;
                        } else {
                            //If close but not at drawing point we travel with pen up
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
                // Travel to next coordinate 
                if (idx < path_len) {
                    const Point* tgt = &path[idx];
                    motor_move_to_mm((float)tgt->x, (float)tgt->y);

                    idx = idx + 1;                   
                    lcd_progress_update(idx, path_len); 
                }
                //decides our next action
                st = ST_LOAD;                        
                break;

            case ST_PEN_DOWN:
                motor_pen_down();
                sleep_ms(PEN_SETTLE_MS);
                //Moves with pen down 
                st = ST_DRAW;   
                break;

            case ST_DRAW:
                // Draw to next coordinate 
                if (idx < path_len) {
                    const Point* tgt = &path[idx];
                    motor_move_to_mm((float)tgt->x, (float)tgt->y);

                    idx = idx + 1;                  
                    lcd_progress_update(idx, path_len); 
                }
                //decides our next action
                st = ST_LOAD;                        
                break;

            case ST_DONE:
                // Finished path
                motor_pen_up();
                sleep_ms(250);
                lcd_progress_update(path_len, path_len);

                while (1) {
                    tight_loop_contents();
                }
                
        }

       
        sleep_ms(FSM_TICK_MS);
    }

    
    return 0;
}
