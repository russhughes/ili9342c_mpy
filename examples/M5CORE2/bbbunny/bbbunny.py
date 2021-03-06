'''
bbbunny.py

    Draw a full screen jpg using the slower but less memory intensive method
    of blitting each Minimum Coded Unit (MCU) block. Usually 8×8pixels but can
    be other multiples of 8.

    bigbuckbunny.jpg (c)copyright 2008, Blender Foundation/www.bigbuckbunny.org
'''

import gc
import random
import time

import ili9342c
from machine import SPI, Pin
import axp202c

gc.enable()
gc.collect()


def main():
    '''
    Decode and draw the jpg on the display
    '''
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

        # enable display and clear screen
        tft.init()

        # display jpg
        tft.jpg("bigbuckbunny.jpg", 0, 0, ili9342c.SLOW)

    finally:
        # shutdown spi
        if 'spi' in locals():
            spi.deinit()


main()
