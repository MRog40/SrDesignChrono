#include "main.h"
#include "chrono.h"
#include "i2c_display.h"
#include "stdint.h"
#include <string.h>

// Sets up the chronograph struct with all of the desired initial states, and
// creates the history buffer as a 2d array with the size specified in H_SIZE.
void init_chrono(volatile chronograph *chrono)
{
    chrono->h_index = 0;
    chrono->v_index = 0;
    chrono->gate_1_time = 0;
    chrono->gate_2_time = 0;
    chrono->gate_3_time = 0;
    chrono->reading = 0;
    for(int i = 0; i < H_SIZE; i++)
    	strcpy((char *)chrono->history[i], "");
}

// Given a new reading and the current chronograph, it puts the new reading
// into the new history position defined by the h_index. 
void add_history(char *new, volatile chronograph *chrono)
{
    // Move index, or reset it to overwrite once history size is full
    if(chrono->h_index == (H_SIZE - 1))
        chrono->h_index = 0;
    else
        chrono->h_index++;

    // Store value in history buffer
    strcpy((char *)chrono->history[chrono->h_index], new);

    // TODO: Write to SD card here
}

void process_reading(volatile chronograph *chrono)
{
    chrono->reading = 0;

    uint32_t time_passed = time_meas(chrono->gate_1_time, chrono->gate_2_time,
                                        chrono->gate_3_time, &gates);

    if(time_passed == 1)
        error();
    else
        new_reading(time_passed, gates, chrono);

    chrono->gate_1_time = 0;
    chrono->gate_2_time = 0;
    chrono->gate_3_time = 0;
}

void new_reading(const uint32_t time_passed, const uint8_t gates, volatile chronograph *chrono)
{
    char buff[17];
    char new_read[17];
    double fps = 0;

    // FPS calculation
    // (DISTANCE / 12) = feet
    // 1e6 / time_passed = 1/seconds
    fps = ((double)(DISTANCE * 1000000) / (time_passed * 12));

    // Copy new reading into the new_read buffer
    // values stored as formatted strings
    strcpy(new_read, d_to_str(fps, buff+16));
    strcat(new_read, " fps ");
    strcat(new_read, u_to_str(gates, buff+16));
    strcat(new_read, "\0");
    add_history(new_read, chrono);
    display_update(chrono);
}

void display_update(volatile chronograph *chrono)
{
    char buff[16];

    // Write each history value to the display
    for(int row = 3; row > -1; row--)
    {
        lcd_put_cur(row,0);
        HAL_Delay(5);

        lcd_send_string(u_to_str(3 - row - chrono->v_index, buff + 16));
        lcd_send_string(": ");

        lcd_send_string((char *)chrono->history[ind(row, chrono)]);
    }
}

// This function is used to find the current index of the buffer and controls
// wrapping back to the beginning when the history buffer is full.
int ind(int row, volatile chronograph *chrono)
{
	int ind = chrono->h_index + 3 - row - chrono->v_index;
	while(ind < 0)
		ind++;
	while(ind > H_SIZE)
		ind--;
	return ind;
}


void error()
{
    lcd_put_cur(3,0);
    lcd_send_string("ERROR");
}


uint32_t time_meas(const uint32_t tim1, const uint32_t tim2,
        const uint32_t tim3, uint8_t *gates)
{
    if(tim1 && tim2 && tim3)
    {
        *gates = 123;
        return ((tim2 - tim1) + (tim3 - tim2))/168;
    }
    else if(tim1 && tim2)
    {
        *gates = 12;
        return (tim2 - tim1)/84;
    }
    else if(tim2 && tim3)
    {
        *gates = 23;
        return (tim3 - tim2)/84;
    }
    else if(tim1 && tim3)
    {
        *gates = 13;
        return (tim3 - tim1)/168;
    }
    return 1;
}


char *d_to_str(double d_x, char *s)
{
    uint32_t x = d_x*100;

    *--s = 0;
    if (!x) {
    	s -= 4;
        strcpy(s, "0.00");
        return s;
    }
    for (int i = 0; i < 2 && x; i++) {
        *--s = '0' + x % 10;
        x /= 10;
    }
    *--s = '.';
    for (; x; x /= 10)
        *--s = '0' + x % 10;
    return s;
}


char *u_to_str(unsigned x, char *s)
{
    *--s = 0;
    if (!x)
        *--s = '0';
    for (; x; x /= 10)
        *--s = '0' + x % 10;
    return s;
}
