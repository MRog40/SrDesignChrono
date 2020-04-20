#ifndef I2C_2004_DISPLAY_H
#define I2C_2004_DISPLAY_H

#include "main.h"
#include "i2c_display.h"
#include "stdint.h"

void lcd_send_cmd (uint8_t cmd);
void lcd_send_data (uint8_t data);
void lcd_clear (void);
void lcd_put_cur(uint8_t row, uint8_t col);
void lcd_init (void);
void lcd_send_string (char *str);

#endif
