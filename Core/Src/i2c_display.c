#include "main.h"
#include "i2c_display.h"
#include "stdint.h"

#define SLAVE_ADDRESS_LCD 0x4E

extern I2C_HandleTypeDef hi2c1;

// Each byte of data is actually sent in 4 bytes to instruct the display on how
// to interpret the data over its 4 data lines
void lcd_send_cmd (uint8_t cmd)
{
    uint8_t data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd & 0xf0);
	data_l = ((cmd << 4) & 0xf0);
	data_t[0] = data_u | 0x0C;  //en=1, rs=0
	data_t[1] = data_u | 0x08;  //en=0, rs=0
	data_t[2] = data_l | 0x0C;  //en=1, rs=0
	data_t[3] = data_l | 0x08;  //en=0, rs=0
	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD, (uint8_t *) data_t, 4, 100);
}

// Almost the same as send command, but instructs the display to write directly
// to the correct positions register and display the character
void lcd_send_data (uint8_t data)
{
	uint8_t data_u, data_l;
	uint8_t data_t[4];
	data_u = (data & 0xf0);
	data_l = ((data << 4) & 0xf0);
	data_t[0] = data_u | 0x0D;  //en=1, rs=0
	data_t[1] = data_u | 0x09;  //en=0, rs=0
	data_t[2] = data_l | 0x0D;  //en=1, rs=0
	data_t[3] = data_l | 0x09;  //en=0, rs=0
	HAL_I2C_Master_Transmit (&hi2c1, SLAVE_ADDRESS_LCD, (uint8_t *) data_t, 4, 100);
}

// Go to the first address, then write spaces to all of the registers
void lcd_clear (void)
{
	lcd_send_cmd (0x80);
	for (int i = 0; i < 80; i++)
	{
		lcd_send_data (' ');
	}
}

// Set the cursor to any row, col on the display for writing (the cursor is not
// visible with how I have the display set up)
void lcd_put_cur(uint8_t row, uint8_t col)
{
    switch (row)
    {
        case 0:
            col = 0x80 | (0x80 + col);
            break;
        case 1:
            col = 0x80 | (0x40 + col);
            break;
        case 2:
            col = 0x80 | (0x14 + col);
            break;
        case 3:
            col = 0x80 | (0x54 + col);
            break;
    }

    lcd_send_cmd (col);
}


void lcd_init (void)
{
	// 4 bit initialisation
	HAL_Delay(50);  // wait for >40ms
	lcd_send_cmd (0x30);
	HAL_Delay(5);  // wait for >4.1ms
	lcd_send_cmd (0x30);
	HAL_Delay(1);  // wait for >100us
	lcd_send_cmd (0x30);
	HAL_Delay(10);
	lcd_send_cmd (0x20);  // 4bit mode
	HAL_Delay(10);

  // display initialisation
	lcd_send_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	HAL_Delay(1);
	lcd_send_cmd (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	HAL_Delay(1);
	lcd_send_cmd (0x01);  // clear display
	HAL_Delay(1);
	HAL_Delay(1);
	lcd_send_cmd (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	HAL_Delay(1);
	lcd_send_cmd (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
}

// Send many characters to the display and increment address
void lcd_send_string (char *str)
{
	while (*str) lcd_send_data (*str++);
}
