"""
noto_fonts Writes the names of three Noto fonts centered on the display
    using the font. The fonts were converted from True Type fonts using
    the font2bitmap utility.
"""

from machine import Pin, SPI
import ili9342c
import axp202c

import NotoSans_32 as noto_sans
import NotoSerif_32 as noto_serif
import NotoSansMono_32 as noto_mono


def main():

    def center(font, s, row, color=ili9342c.WHITE):
        screen = tft.width()                     # get screen width
        width = tft.write_len(font, s)           # get the width of the string
        if width and width < screen:             # if the string < display
            col = tft.width() // 2 - width // 2  # find the column to center
        else:                                    # otherwise
            col = 0                              # left justify

        tft.write(font, s, col, row, color)      # and write the string

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

    # center the name of the first font, using the font
    row = 16
    center(noto_sans, "NotoSans", row, ili9342c.RED)
    row += noto_sans.HEIGHT

    # center the name of the second font, using the font
    center(noto_serif, "NotoSerif", row, ili9342c.GREEN)
    row += noto_serif.HEIGHT

    # center the name of the third font, using the font
    center(noto_mono, "NotoSansMono", row, ili9342c.BLUE)
    row += noto_mono.HEIGHT


main()
