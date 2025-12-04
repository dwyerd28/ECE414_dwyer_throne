// main.c — Drawing FSM with LCD progress bar

#include "pico/stdlib.h"
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include "config.h"
#include "coord_setup.h"
#include "limits.h"
#include "motor.h"
#include "lcd_progress.h"

// ---------- FSM states ----------
typedef enum {
    ST_BOOT = 0,
    ST_LOAD,
    ST_PEN_UP,
    ST_MOVE,       // travel with pen up
    ST_PEN_DOWN,
    ST_DRAW,       // draw with pen down
    ST_DONE
} State;

// ---------- Helpers: get current pose in mm ----------

static void get_pose_mm(float* x_mm, float* y_mm) {
    MotorPose p = motor_get_pose();  // position in steps
    float sx = coord_steps_per_mm_x();
    float sy = coord_steps_per_mm_y();

    // avoid divide-by-zero if calibration not set
    if (sx <= 0.0f) {
        sx = 1.0f;
    }
    if (sy <= 0.0f) {
        sy = 1.0f;
    }

    *x_mm = ((float)p.x_steps) / sx;
    *y_mm = ((float)p.y_steps) / sy;
}

// Euclidean distance (mm) from current pose to a target point
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
    sleep_ms(500);
    printf("ECE414 Final: booting...\n");

    // ----- Initialize hardware subsystems -----
    motor_init();       // stepper motors (CoreXY)
    motor_pen_init();   // servo on, pen up
    limits_init(X_MIN_MM, X_MAX_MM, Y_MIN_MM, Y_MAX_MM);

    // ----- Load drawing path -----
    size_t path_len = 0;
    const Point* path = coord_get_path(&path_len);

    printf("Path length reported: %u points\n", (unsigned)path_len);

    // Index of "next point to go to"
    size_t idx = 0;

    // ----- Initialize LCD progress UI -----
    lcd_progress_init(path_len);
    lcd_progress_update(idx, path_len);   // start at 0%

    // ----- FSM bootstrap -----
    State st = ST_BOOT;
    State prev = (State)(-1);   // for debug printing

    while (true) {

        // DEBUG: print state transitions
        if (st != prev) {
            printf("FSM: state changed to %d (idx=%u)\n", st, (unsigned)idx);
            prev = st;
        }

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
                    printf("ST_LOAD: idx >= path_len (%u >= %u), going to ST_DONE\n",
                           (unsigned)idx, (unsigned)path_len);
                    st = ST_DONE;
                    break;
                }

                {
                    const Point* next = &path[idx];

                    // Skip any out-of-bounds points (just in case)
                    if (!limits_point_ok_mm((float)next->x, (float)next->y)) {
                        printf("ST_LOAD: point %u out of bounds (%.1f, %.1f), skipping\n",
                               (unsigned)idx, (float)next->x, (float)next->y);
                        idx = idx + 1;
                        lcd_progress_update(idx, path_len);
                        st = ST_LOAD;
                        break;
                    }

                    // Decide whether this should be a TRAVEL (pen up)
                    // or DRAW (pen down) based on distance + draw flag.
                    float dist = distance_to_target_mm(next);
                    printf("ST_LOAD: next point %u = (%.1f, %.1f, draw=%d), dist=%.2f mm\n",
                           (unsigned)idx, (float)next->x, (float)next->y,
                           next->draw ? 1 : 0, dist);

                    if (dist > COORD_DIFF_THRESH_MM) {
                        // Far away -> travel with pen up
                        st = ST_PEN_UP;
                    } else {
                        if (next->draw) {
                            // Close and drawing point -> draw with pen down
                            st = ST_PEN_DOWN;
                        } else {
                            // Close but not drawing -> still travel pen up
                            st = ST_PEN_UP;
                        }
                    }
                }
                break;

            case ST_PEN_UP:
                motor_pen_up();
                sleep_ms(PEN_SETTLE_MS);
                st = ST_MOVE;   // next: move with pen up
                break;

            case ST_MOVE:
                // Travel to next coordinate (pen is up)
                if (idx < path_len) {
                    const Point* tgt = &path[idx];
                    printf("ST_MOVE: moving pen-up to (%.1f, %.1f)\n",
                           (float)tgt->x, (float)tgt->y);
                    motor_move_to_mm((float)tgt->x, (float)tgt->y);

                    idx = idx + 1;                       // completed this waypoint
                    lcd_progress_update(idx, path_len);  // update LCD progress
                }
                st = ST_LOAD;                            // decide next action
                break;

            case ST_PEN_DOWN:
                motor_pen_down();
                sleep_ms(PEN_SETTLE_MS);
                st = ST_DRAW;   // next: move with pen down
                break;

            case ST_DRAW:
                // Draw to next coordinate (pen is down)
                if (idx < path_len) {
                    const Point* tgt = &path[idx];
                    printf("ST_DRAW: moving pen-down to (%.1f, %.1f)\n",
                           (float)tgt->x, (float)tgt->y);
                    motor_move_to_mm((float)tgt->x, (float)tgt->y);

                    idx = idx + 1;                       // completed this waypoint
                    lcd_progress_update(idx, path_len);  // update LCD progress
                }
                st = ST_LOAD;                            // decide next action
                break;

            case ST_DONE:
                // Finished path: pen up, show 100%, and park forever
                printf("FSM: ST_DONE reached. Path complete.\n");
                motor_pen_up();
                sleep_ms(250);
                lcd_progress_update(path_len, path_len);

                while (1) {
                    tight_loop_contents();
                }
                // no break; main loop ends inside while(1)
        }

        // FSM pacing (motion calls themselves are blocking)
        sleep_ms(FSM_TICK_MS);
    }

    // Not reached
    return 0;
}
