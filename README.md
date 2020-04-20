__TOC__

# Senior Design Chronograph - The Passing of the Torch
This is the documentation for the software for senior design project for the
measurement of the speed of small caliber projectiles. This software is written
for an STM32F4 microcontroller that is the at the core of our system -- a three
gate laser plane chronograph. 

_**Note:** Most of the links provided in this documentation are crucial. They
are not just relevant tangential links, but meant to be consumed inline or at
minimum referenced later on._

# Hardware
## STM32F4
The 32-bit MCU being used for this project is the [STM32F405RGT][STM32F405RGT].
That link has good information about the device, but the primary reason it was
selected was due to its speed. The [datasheet][MCU Datasheet] has better
information than ST's marketing material. This processor has way more
peripherals and processing power than we need, but it was an incredibly cheap
option to get very fast timing capabilities into our project.

## Prototype Board
The prototype board is a [STM32F4Stamp][STM432F4Stamp] designed by Frank Zhao.
EagleCAD files were provided, which I used to generate gerbers and order the
boards from OSH Park. I ordered all the parts from Digi-key, and assembled two
boards. This prototype is breadboard compatible and has all of the pins exposed
so software was able to be tested throughout the other designs. All of the
development done so far was using this board.

_**Note:** One of the protoype boards does not work. It seems like it does, and
will succesfully connect and flash, but it will not do what you program it to
do. I'm not sure what is wrong with it, bonus points if you figure it out. I
spent many many hours thinking there were software errors before trying the
other board and having it work right away._

## Digital Board
This board builds from the prototype board and connectorizes all of the pins we
need and adds SMPS and battery management. It doesn't make much sense for me to
describe this board any further, just go ahead and look at the [Digital Design
Files][DigDesignFiles] included in the repo. CAD work was done using EasyEDA,
as it has integrated collaboration that is very handy for group board designs.

This documentation is primarily focused on software so I won't dive into this
too heavily here. I also won't dive in because I never actually was able to be
hands on with this board or test it due to the coronavirus, and am not sure
what the status of it is at this time.

# Development Tools
ST's [development ecosystem][ST Tools] has become impressive throughout the
years, going from eclipse addons to fully fledged IDE's with automatic code
generation capabilities from GUI peripheral setup and impressive graphical
clock management systems. This available tooling also contributed to it being
selected over alternative MCUs.

## IDE
For building the project I used the [STM32CubeIDE][STM32CubeIDE]. This IDE
doesn't need to be used, but it nicely integrated with
[STM32CubeMX][STM32CubeMX] for peripheral setup. There are many compilers that
could be used instead using the same project files, some good alternatives are
[Keil][Keil] or the [arm-gcc toolchain][ARM GCC].

Peripheral code generation can save you some time reading the [STM32 HAL
Manual][STM32 HAL Manual], but take a look at [CubeMX Manual][CubeMX Manual] to
see for yourself.


## DFU Mode
**DFU** is [Device Firmware Upgrade][DFU AN], and is a USB protocol implemented
in many STM32 devices that essentially makes the microcontroller USB
programmable without any other ICs needed. It is a boot mode built into the
device that is permanent, and can be easily accessed and programmed using ST
provided tools. This [DFU User Manual][DFU UM] has all the information you need
to know to interact with the GUI and successfully flash a the device. 

DFU Mode is entered by holding a specific pin while booting, which is done with
the **BOOT** button on our devices. There are multiple ways to enter DFU Mode:
* Hold the **BOOT** button and then plug in the USB cable
* When already plugged in, hold the **BOOT** button and press the **RST**
  button

## Step-by-Step Guide to Programming With DFU 

1. If you haven't already, download and install the [DfuSe Demo][DfuSe]
   software package from ST. This package includes the programs **DfuSeDemo**
   and **DFU file manager**.

1. Compile the project using STM32CubeIDE with the `Build All` command, or with
   your IDE of choice and the associated method.

2. Run **DFU file manager** and use it to convert the `Chrono.hex` file
   in `SrDesignChrono/Debug/` into `Chrono.dfu`. I save this file in the same
   directory, but it doesn't need to be. [More details here if
   needed][DFU UM]

3. Plug in the microcontroller and enter DFU Mode one of two ways:
    * Hold the **BOOT** button and then plug in the USB cable
    * When already plugged in, hold the **BOOT** button and press the **RST**
      button

4. Open **DfuSeDemo**.

5. Select the device from the list in the _Available DFU and compatible HID
   Devices_ section. [More details here if needed][DFU UM]

6. Select the **Choose** button from the _Upgrade or Verify Action_ section and
   select the `Chrono.dfu` file generated earlier. 

7. Select the **Upgrade** button, wait until the device is done upgrading and
   then press the **RST** button to exit DFU mode.

### Step-by-Step 3-Step Summary

1. Compile C project to `.hex` with **STM32CubeIDE**
2. Convert `.hex` to `.dfu` with **DFU file manager**
3. Flash `.dfu` to device with **DfuSeDemo**


# Software
After much deliberation and time and effort poored into CMSIS drivers, I
succumbed to using ST's HAL drivers for this project.

## Theory of Operation

## Software Manuals
The [STM32F405 Reference Manual][STM32F405 RM] is the most important document
to become proficient with for development on this project, closely followed by
the [STM32 HAL Manual][STM32 HAL Manual].

It is arduous task. There is a lot there to chew on, and if you've never worked
on something like this before it can be overhwhelming (or at least it was for
me). I highly recommend reading the introductory information for each session
and learning how to navigate the PDF using the bookmarks that are provided for
each section. Even google chrome can navigate using these bookmarks, but they
are best leveraged using something like Adobe Reader (which is also faster and
uses less memory).

If you do become familiar with these, put it on your resume. STM32s are used
everywhere because of how powerful and cheap they are, and if you are
interested in embedded software in the future this is a great skillset to have.

The [STM32 Cortex-M4 Programming Manual][M4 PM] is useful for lower level
information, but I doubt it will be needed much for this project.

There are lots of other ST documents available about specific peripherals and
implementations, but most of them are just abstractions of information in these
manuals available. You realistically should not need any information outside of
the links provided in this section.

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


<!-- Reference Links -->
[STM32F405RGT]: https://www.st.com/en/microcontrollers-microprocessors/stm32f405rg.html
[MCU Datasheet]: https://www.st.com/resource/en/datasheet/dm00037051.pdf

[DigDesignFiles]: https://github.com/MRog40/SrDesignChrono/blob/master/Project_digital_20200420210721.zip
[AnalogDesignFiles]: https://github.com/MRog40/SrDesignChrono/blob/master/Project_sensorboard_20200420210733.zip
[STM432F4Stamp]: https://eleccelerator.com/stm32f4stamp-breakout-board/

[ST Tools]: https://www.st.com/en/development-tools/stm32-software-development-tools.html

[STM32CubeIDE]: https://www.st.com/en/development-tools/stm32cubeide.html
[CubeIDE Quick Start]: https://www.st.com/resource/en/user_manual/dm00598966-stm32cubeide-quick-start-guide-stmicroelectronics.pdf

[Keil]: http://www2.keil.com/mdk5/uvision/
[ARM GCC]: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm

[STM32CubeMX]: https://www.st.com/en/development-tools/stm32cubemx.html
[CubeMX Manual]: https://www.st.com/resource/en/user_manual/dm00104712-stm32cubemx-for-stm32-configuration-and-initialization-c-code-generation-stmicroelectronics.pdf

[DFU AN]: https://www.st.com/resource/en/application_note/cd00264379-usb-dfu-protocol-used-in-the-stm32-bootloader-stmicroelectronics.pdf

[DFU UM]: https://www.st.com/resource/en/user_manual/cd00155676-getting-started-with-dfuse-usb-device-firmware-upgrade-stmicroelectronics-extension-stmicroelectronics.pdf

[DfuSe]: https://www.st.com/en/development-tools/stsw-stm32080.html

[STM32 HAL Manual]: https://www.st.com/resource/en/user_manual/dm00105879-description-of-stm32f4-hal-and-ll-drivers-stmicroelectronics.pdf 

[STM32F405 RM]: https://www.st.com/resource/en/reference_manual/dm00031020-stm32f405-415-stm32f407-417-stm32f427-437-and-stm32f429-439-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf

[M4 PM]: https://www.st.com/resource/en/programming_manual/dm00046982-stm32-cortex-m4-mcus-and-mpus-programming-manual-stmicroelectronics.pdf
