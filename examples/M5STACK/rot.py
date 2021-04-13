"""
fonts.py

    Pages through all characters of four fonts on the M5Stack Display.

"""
import utime
import random
from machine import Pin, SPI
import ili9342c
import vga1_bold_16x32 as font


def main():
    tft = ili9342c.ILI9342C(
        SPI(
            2,
            baudrate=60000000,
            polarity=1,
            phase=1,
            sck=Pin(18),
            mosi=Pin(23)),
        320,
        240,
        reset=Pin(33, Pin.OUT),
        cs=Pin(14, Pin.OUT),
        dc=Pin(27, Pin.OUT),
        backlight=Pin(32, Pin.OUT),
        rotation=0,
        buffer_size=16*32*2)

    tft.init()
    tft.fill(ili9342c.BLACK)
    utime.sleep(1)

    while True:
        for rot in range(8):
            if rot in [0, 4]:
                tft.fill(ili9342c.BLACK)

            tft.rotation(rot)
            s = "Rot {}".format(rot)
            tft.text(
                font,
                s,
                0,
                0,
                ili9342c.WHITE)

            utime.sleep(3)


main()
