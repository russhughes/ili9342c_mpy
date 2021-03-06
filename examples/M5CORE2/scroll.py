"""
scroll.py

    Smoothly scroll all characters in a font up the M5Stack Core Display.
    Fonts heights must be even multiples of the screen height
    (i.e. 8 or 16 pixels high).

"""

import utime
import random
from machine import Pin, SPI

import ili9342c
import axp202c
import vga1_bold_16x16 as font


def cycle(p):
    try:
        len(p)
    except TypeError:
        cache = []
        for i in p:
            yield i
            cache.append(i)
        p = cache
    while p:
        yield from p


def main():
    # Turn display power
    axp = axp202c.PMU(address=0x34)
    axp.enablePower(axp202c.AXP192_LDO2)
    # Set backlight voltage
    axp.setDC3Voltage(3000)

    spi = SPI(
        2,
        baudrate=60000000,
        sck=Pin(18),
        mosi=Pin(23))

    # initialize display

    tft = ili9342c.ILI9342C(
        spi,
        320,
        240,
        reset=Pin(33, Pin.OUT),
        cs=Pin(5, Pin.OUT),
        dc=Pin(15, Pin.OUT),
        rotation=0)

    colors = cycle([0xe000, 0xece0, 0xe7e0, 0x5e0, 0x00d3, 0x7030])
    foreground = next(colors)
    background = ili9342c.BLACK

    tft.init()
    tft.fill(background)
    utime.sleep(1)

    height = tft.height()
    width = tft.width()
    last_line = height - font.HEIGHT

    tfa = 0        # top free area
    tfb = 0        # bottom free area
    tft.vscrdef(tfa, height, tfb)

    scroll = 0
    character = 32

    while True:
        # clear top line before scrolling off display
        tft.fill_rect(0, scroll, width, 1, background)

        # Write new line when we have scrolled the height of a character
        if scroll % font.HEIGHT == 0:
            line = (scroll + last_line) % height

            # write character hex value as a string
            tft.text(
                font,
                'x{:02x}'.format(character),
                16,
                line,
                foreground,
                background)

            # write character using a integer (could be > 0x7f)
            tft.text(
                font,
                character,
                90,
                line,
                foreground,
                background)

            # change color for next line
            foreground = next(colors)

            # next character with rollover at 256
            character += 1
            if character > 127:
                character = 32

        # scroll the screen up 1 row
        tft.vscsad(scroll+tfa)
        scroll += 1
        scroll %= height

        utime.sleep(0.01)


main()
