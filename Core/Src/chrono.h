#ifndef CHRONO_H
#define CHRONO_H

#define DISTANCE 10 // inches
#define H_SIZE 10

#include "i2c_display.h"

typedef struct Chronograph {
    uint8_t h_index;
    uint8_t v_index;

    char history[H_SIZE][10];

    uint32_t gate_1_time;
    uint32_t gate_2_time;
    uint32_t gate_3_time;
    uint8_t reading;
} chronograph;

uint32_t time_meas(const uint32_t tim1, const uint32_t tim2, const uint32_t tim3, uint8_t *gates);
void process_reading(volatile chronograph *chrono);
void new_reading(const uint32_t time_passed, const uint8_t gates, volatile chronograph *chrono);
char *u_to_str(unsigned x, char *s);
char *d_to_str(double x, char *s);
void init_chrono(volatile chronograph *chrono);
void LCD_Example();
void error();
void display_update(volatile chronograph *chrono);
int ind(int col, volatile chronograph *chrono);

#endif
