'''
paint.py

    A very simple paint program for the M5CORE2 using the adafruit_focaltouch
    driver modified for micropython by franz schaefer from
    https://gitlab.com/mooond/t-watch2020-esp32-with-micropython

    I modied the axp202c driver to accept an initalized i2c object so it can
    be shared with the focaltouch driver. See the lib directory for source.
'''

from machine import Pin, SPI, SoftI2C
import axp202c
import ili9342c

import focaltouch
import vga1_8x8 as font

# color swatches
COLORS = [
    ili9342c.BLACK,
    ili9342c.BLUE,
    ili9342c.RED,
    ili9342c.GREEN,
    ili9342c.CYAN,
    ili9342c.MAGENTA,
    ili9342c.YELLOW,
    ili9342c.WHITE]


def main():
    '''
    Draw on screen using focaltouch sensor
    '''
    try:
        i2c = SoftI2C(scl=Pin(22), sda=Pin(21))

        # Turn display power
        axp = axp202c.PMU(i2c=i2c, address=0x34)
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

        # enable display and clear screen
        tft.init()
        tft.fill(ili9342c.BLACK)
        tft.text(font, "Draw", 158, 1, ili9342c.WHITE)

        # enable focaltouch touchscreen
        touch = focaltouch.FocalTouch(i2c)

        color_index = 0
        color = 0
        # draw color swatches used to select color to draw
        for color_index, color in enumerate(COLORS):
            tft.fill_rect(color_index*40, 200, 40, 40, color)

        # draw boxes around currently selected color
        tft.rect(color_index*40, 200, 40, 40, ili9342c.WHITE)
        tft.rect(color_index*40+1, 201, 38, 38, ili9342c.BLACK)
        tft.rect(color_index*40+2, 202, 36, 36, ili9342c.BLACK)

        start = None
        while True:
            # can be up to two touches
            if touch.touched == 1:
                # get x and y points of the first touch point
                p_x = touch.touches[0]['x']
                p_y = touch.touches[0]['y']

                # If point is in the lowest 40 rows of the screen
                # change color to swatch pressed.
                if p_y > 200:
                    # remove box from around previous color swatch
                    tft.rect(color_index*40, 200, 40, 40, color)
                    tft.rect(color_index*40+1, 201, 38, 38, color)
                    tft.rect(color_index*40+2, 202, 36, 36, color)

                    # update new color
                    color_index = p_x//40
                    color = COLORS[color_index]

                    # draw box around newly selected color swatch
                    tft.rect(color_index*40, 200, 40, 40, ili9342c.WHITE)
                    tft.rect(color_index*40+1, 201, 38, 38, ili9342c.BLACK)
                    tft.rect(color_index*40+2, 202, 36, 36, ili9342c.BLACK)

                else:
                    # press is out side of colors
                    if start is None:
                        # 1st press, save start of line
                        start = (p_x, p_y)
                    else:
                        # draw line to current location and make new start
                        tft.line(start[0], start[1], p_x, p_y, color)
                        start = (p_x, p_y)
            else:
                # no touch, reset for new line
                start = None

    finally:
        # shutdown spi
        spi.deinit()


main()
