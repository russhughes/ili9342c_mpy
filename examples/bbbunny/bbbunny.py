'''
jpegtest.py

    Draw a full screen jpg using the slower but less memory intensive method of blitting
    each Minimum Coded Unit (MCU) block. Usually 8×8pixels but can be other multiples of 8.

    bigbuckbunny.jpg (c) copyright 2008, Blender Foundation / www.bigbuckbunny.org
'''

import gc
import time
import random
from machine import Pin, SPI
import ili9342c

gc.enable()
gc.collect()

def main():
    '''
    Decode and draw the jpg on the display
    '''
    try:

        # initialize display

        tft = ili9342c.ILI9342C(
            SPI(2, baudrate=40000000, polarity=1, phase=1, sck=Pin(18), mosi=Pin(23)),
            320,
            240,
            reset=Pin(33, Pin.OUT),
            cs=Pin(14, Pin.OUT),
            dc=Pin(27, Pin.OUT),
            backlight=Pin(32, Pin.OUT),
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
