"""
mono_fonts.py for monofont2bitmap converter and bitmap method.
"""

import time
from machine import Pin, SPI
import ili9342c
import axp202c

import inconsolata_16 as font_16
import inconsolata_32 as font_32
import inconsolata_64 as font_64


def main():
    fast = False

    def display_font(font):
        tft.fill(ili9342c.BLUE)
        column = 0
        row = 0
        for char in font.MAP:
            tft.bitmap(font, column, row, font.MAP.index(char))
            column += font.WIDTH
            if column >= tft.width() - font.WIDTH:
                row += font.HEIGHT
                column = 0

                if row > tft.height() - font.HEIGHT:
                    row = 0

            if not fast:
                time.sleep(0.05)

    tft = ili9342c.ILI9342C(
        SPI(2, baudrate=60000000, sck=Pin(18), mosi=Pin(23)),
        320,
        240,
        reset=Pin(33, Pin.OUT),
        cs=Pin(14, Pin.OUT),
        dc=Pin(27, Pin.OUT),
        backlight=Pin(32, Pin.OUT),
        rotation=0,
        buffer_size=66*32*2)

    tft.init()

    while True:
        for font in [font_16, font_32, font_64]:
            display_font(font)

        fast = not fast


main()
