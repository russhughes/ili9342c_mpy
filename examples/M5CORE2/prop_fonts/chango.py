"""
chango.py test for font2bitmap converter.
"""

from machine import Pin, SPI
import ili9342c
import axp202c

import chango_16 as font_16
import chango_32 as font_32
import chango_64 as font_64


def main():

    try:
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

        tft.init()
        tft.fill(ili9342c.BLACK)

        row = 0
        tft.write(font_16, "abcdefghijklmnopqrstuvwxyz", 0, row)
        row += font_16.HEIGHT

        tft.write(font_32, "abcdefghijklm", 0, row)
        row += font_32.HEIGHT

        tft.write(font_32, "nopqrstuvwxy", 0, row)
        row += font_32.HEIGHT

        tft.write(font_64, "abcdef", 0, row)
        row += font_64.HEIGHT

        tft.write(font_64, "ghijkl", 0, row)

    finally:
        # shutdown spi
        if 'spi' in locals():
            spi.deinit()


main()
