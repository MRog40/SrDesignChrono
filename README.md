[toc]

# Senior Design Chronograph
This is the documentation for the software for senior design project dedicated
to the measurement of the speed of small caliber projectiles. This software is
written for an STM32F4 microcontroller that is the at the core of our system --
a three gate laser plane chronograph.

**Note:** Think of all the links provided in this documentation as part of the
documentation. They are not just relevant tangential links, but meant to be
consumed inline or at the minimum referenced later.

# Hardware

The 32-bit MCU being used for this project is the [STM32F405RGT][STM32F405RGT].
That link has good information about the device, but the primary reason it was
selected was due to its speed. The [datasheet][MCU Datasheet] has better
information than ST's marketing material. This processor has way more
peripherals and processing power than we need, but it was an incredibly cheap
option to get very fast timing capabilities into our project.

# Development Tools

ST's [development ecosystem][ST Tools] has become impressive throughout the
years, going from eclipse addons to fully fledged IDE's with automatic code
generation capabilities from GUI peripheral setup and impressive graphical
clock management systems. This available tooling also contributed to it being
selected over alternative MCUs.

## IDE

For building the project I used the [STM32CubeIDE][STM32CubeIDE]

## DFU Info

## DFU Quick Guide

# Software

## Software Manuals

## LCD Display

### LCD Drivers

```c
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

// Send many characters to the display and increment address
void lcd_send_string (char *str)
{
	while (*str) lcd_send_data (*str++);
}
```

### LCD Setup

```c
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
```

### LCD Usage Example

```c
lcd_put_cur(0,0);
lcd_send_string("3:             Ready");

lcd_put_cur(1,0);
lcd_send_string("2:");

lcd_put_cur(2,0);
lcd_send_string("1:");

lcd_put_cur(3,0);
lcd_send_string("0: 1317.9 fps");
```

### Display Update

## Chronograph Struct

## ISRs and Timers


# Links

## Hardware Info

[STM32F405RGT]: https://www.st.com/en/microcontrollers-microprocessors/stm32f405rg.html
[MCU Datasheet]: https://www.st.com/resource/en/datasheet/dm00037051.pdf

## Dev Tools

[ST Tools]: https://www.st.com/en/development-tools/stm32-software-development-tools.html

[STM32CubeIDE]: https://www.st.com/en/development-tools/stm32cubeide.html
[CubeIDE Quick Start]: https://www.st.com/resource/en/user_manual/dm00598966-stm32cubeide-quick-start-guide-stmicroelectronics.pdf

[ARM GCC]: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm

[STM32CubeMX]: https://www.st.com/en/development-tools/stm32cubemx.html
[CubeMX Manual]: https://www.st.com/resource/en/user_manual/dm00104712-stm32cubemx-for-stm32-configuration-and-initialization-c-code-generation-stmicroelectronics.pdf

## Programming Manuals

[STM32 HAL Manual]: https://www.st.com/resource/en/user_manual/dm00105879-description-of-stm32f4-hal-and-ll-drivers-stmicroelectronics.pdf 
