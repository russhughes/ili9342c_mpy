# ILI9342C Driver for MicroPython

![Image](docs/m5stack_hello.jpg)

## This is a work in progress

This driver is based on [devbis' st7789_mpy driver.](https://github.com/devbis/st7789_mpy)  I modified the original driver for one of my projects by adding
support for display rotation, scrolling and drawing text using 8 and 16 bit
wide bitmap fonts. Included are 12 bitmap fonts derived from classic pc text
mode fonts and a couple of example programs that run on the M5Stack Core.
The driver supports 320x240 displays.

## Pre-compiled MicroPython firmware

The firmware directory contains pre-compiled MicroPython v1.13-248-gf305c62a5
ESP32 GENERIC firmware.bin with the ILI9342C C driver and frozen python font
files compiled with ESP-IDF 4

## 128 Character Fonts


Font               | Example
------------------ | -----------------------------------------------------------
vga1_8x8.py        | ![Image](docs/vga1_8x8.png)
vga1_16x16.py      | ![Image](docs/vga1_16x16.png)
vga1_16x32.py      | ![Image](docs/vga1_16x32.png)
vga1_bold_16x16.py | ![Image](docs/vga1_bold_16x16.png)
vga1_bold_16x32.py | ![Image](docs/vga1_bold_16x32.png)


## 256 Character Fonts

Font               | Example
------------------ | -----------------------------------------------------------
vga2_8x8.py        | ![Image](docs/vga2_8x8.png)
vga2_8x16.py       | ![Image](docs/vga2_8x16.png)
vga2_16x16.py      | ![Image](docs/vga2_16x16.png)
vga2_16x32.py      | ![Image](docs/vga2_16x32.png)
vga2_bold_16x16.py | ![Image](docs/vga2_bold_16x16.png)
vga2_bold_16x32.py | ![Image](docs/vga2_bold_16x32.png)



## Thanks go out to:

- https://github.com/devbis for the original driver this is based on.
- https://github.com/hklang10 for letting me know of the new mp_raise_ValueError().
- https://github.com/aleggon for finding the correct offsets for a 240x240 display and discovering issues compiling for STM32 based boards.

-- Russ

## Overview

This is a driver for MicroPython to handle displays based on the ILI9342C chip.


## Building instruction

Prepare build tools as described in the manual. You should follow the
instruction for building MicroPython and ensure that you can build the
firmware without this display module.

Clone this module alongside the MPY sources:

    $ git clone https://github.com/russhughes/ili9342c_mpy.git

Go to MicroPython ports directory:

    $ cd micropython/ports/esp32

And then compile the module setting USER_C_MODULES to the directory with the
driver source code.

    $ make USER_C_MODULES=../../../ili9342c_mpy/ all


If you have other user modules, copy the ILI9342C_driver/ILI9342C to
the user modules directory

Upload the resulting firmware to your device using the esptool.py program.
(See
[MicroPython docs](http://docs.micropython.org/en/latest/esp8266/tutorial/intro.html#deploying-the-firmware)
for more info)


## Examples

This module was (barely) tested on M5Stack Core and should run on other ESP32
devices that are able run GENERIC MicroPython Firmware. See the examples folder
for sample programs.

    # ESP32

    import machine
    import ili9342c
    spi = machine.SPI(2, baudrate=40000000, polarity=1, phase=1, sck=Pin(18), mosi=Pin(23)),
    display = ili9342c.ILI9342C(
        spi,
        320,
        240,
        reset=Pin(33, Pin.OUT),
        cs=Pin(14, Pin.OUT),
        dc=Pin(27, Pin.OUT),
        backlight=Pin(32, Pin.OUT),
        rotation=0)


## Methods

- `ili9342c.ILI9342C(spi, width, height, reset, dc, cs, backlight, rotation)`

    required args:

        `spi` spi device
        `width` display width
        `height` display height

    optional args:

        `reset` reset pin
        `dc` dc pin
        `cs` cs pin
        `backlight` backlight pin
        `rotation` 0-0 degrees, 1-90 degrees, 2-180 degrees, 3-270 degrees

This driver supports only 16bit colors in RGB565 notation.

- `ILI9342C.fill(color)`

  Fill the entire display with the specified color.

- `ILI9342C.pixel(x, y, color)`

  Set the specified pixel to the given color.

- `ILI9342C.line(x0, y0, x1, y1, color)`

  Draws a single line with the provided `color` from (`x0`, `y0`) to
  (`x1`, `y1`).

- `ILI9342C.hline(x, y, length, color)`

  Draws a single horizontal line with the provided `color` and `length`
  in pixels. Along with `vline`, this is a fast version with reduced
  number of SPI calls.

- `ILI9342C.vline(x, y, length, color)`

  Draws a single horizontal line with the provided `color` and `length`
  in pixels.

- `ILI9342C.rect(x, y, width, height, color)`

  Draws a rectangle from (`x`, `y`) with corresponding dimensions

- `ILI9342C.fill_rect(x, y, width, height, color)`

  Fill a rectangle starting from (`x`, `y`) coordinates

- `ILI9342C.blit_buffer(buffer, x, y, width, height)`

  Copy bytes() or bytearray() content to the screen internal memory.
  Note: every color requires 2 bytes in the array

- `ILI9342C.text(font, s, x, y[, fg, bg])`

  Write text to the display using the specified font with the coordinates as
  the upper-left corner of the text. The foreground and background colors of
  the text can be set by the optional arguments fg and bg, otherwise the
  foreground color defaults to `WHITE` and the background color defaults to
  `BLACK`.  See the fonts directory for example fonts and the utils directory
  for a font conversion program. Currently has issues with characters > 127.

- `ILI9342C.width()`

  Returns the current logical width of the display. (ie a 320x240 display
  rotated 90 degrees is 240 pixels wide)

- `ILI9342C.height()`

  Returns the current logical height of the display. (ie a 320x240 display
  rotated 90 degrees is 320 pixels high)

- `ILI9342C.rotation(r)`

  Set the rotates the logical display in a clockwise direction. 0-Portrait
  (0 degrees), 1-Landscape (90 degrees), 2-Inverse Portrait (180 degrees),
  3-Inverse Landscape (270 degrees)


The module exposes predefined colors:
  `BLACK`, `BLUE`, `RED`, `GREEN`, `CYAN`, `MAGENTA`, `YELLOW`, and `WHITE`


## Helper functions

- `color565(r, g, b)`

  Pack a color into 2-bytes rgb565 format

- `map_bitarray_to_rgb565(bitarray, buffer, width, color=WHITE, bg_color=BLACK)`

  Convert a bitarray to the rgb565 color buffer which is suitable for blitting.
  Bit 1 in bitarray is a pixel with `color` and 0 - with `bg_color`.

  This is a helper with a good performance to print text with a high
  resolution font. You can use an awesome tool
  https://github.com/peterhinch/micropython-font-to-py
  to generate a bitmap fonts from .ttf and use them as a frozen bytecode from
  the ROM memory.

## Performance

For the comparison I used an excellent library for Arduino
that can handle this screen.

https://github.com/ananevilya/Arduino-ILI9342C-Library/

Also, I used my slow driver for this screen, written in pure python.

https://github.com/devbis/ILI9342Cpy_mpy/

I used these modules to draw a line from 0,0 to 239,239
The table represents the time in milliseconds for each case

|         | Arduino-ILI9342C | ILI9342Cpy_mpy | ili9342c_mpy    |
|---------|----------------|--------------|---------------|
| ESP8266 | 26             | 450          | 12            |
| ESP32   | 23             | 450          | 47            |


As you can see, the ESP32 module draws a line 4 times slower than
the older ESP8266 module.


## Troubleshooting

#### Overflow of iram1_0_seg

When building a firmware for esp8266 you can see this failure message from
the linker:

    LINK build/firmware.elf
    xtensa-lx106-elf-ld: build/firmware.elf section `.text' will not fit in
    region `iram1_0_seg' xtensa-lx106-elf-ld: region `iram1_0_seg' overflowed
    by 292 bytes
    Makefile:192: recipe for target 'build/firmware.elf' failed

To fix this issue, you have to put ILI9342C module to irom0 section.
Edit `esp8266_common.ld` file in the `ports/esp8266` dir and add a line

    *ILI9342C/*.o(.literal* .text*)

in the `.irom0.text : ALIGN(4)` section


#### Unsupported dimensions

This driver supports only 320x240 pixel displays.
