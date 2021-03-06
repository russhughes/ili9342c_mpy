'''
hershey.py
    Draw greetings on display cycling thru hershey fonts and colors
'''

import sys
import utime
from machine import SPI, Pin
import ili9342c
import random

# Load several frozen fonts from flash

import gotheng
import greeks
import italicc
import italiccs
import meteo
import music
import romanc
import romancs
import romand
import romanp
import romans
import romant
import scriptc
import scripts


def cycle(p):
    '''
    return the next item in a list
    '''
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


COLORS = cycle([0xe000, 0xece0, 0xe7e0, 0x5e0, 0x00d3, 0x7030])

FONTS = cycle([
    gotheng, greeks, italicc, italiccs, meteo, music, romanc,
    romancs, romand, romanp, romans, romant, scriptc, scripts])

GREETINGS = cycle([
    "bonjour", "buenas noches", "buenos dias",
    "good day", "good morning", "hey",
    "hi-ya", "hi", "how are you", "how goes it",
    "howdy-do", "howdy", "shalom", "welcome",
    "what's happening", "what's up"])


def main():
    '''
    Draw greetings on display cycling thru hershey fonts and colors
    '''
    try:
        tft = ili9342c.ILI9342C(
            SPI(2, baudrate=60000000, sck=Pin(18), mosi=Pin(23)),
            320,
            240,
            reset=Pin(33, Pin.OUT),
            cs=Pin(14, Pin.OUT),
            dc=Pin(27, Pin.OUT),
            backlight=Pin(32, Pin.OUT),
            rotation=0)

        tft.init()
        tft.fill(ili9342c.BLACK)
        height = tft.height()
        width = tft.width()
        row = 0

        while True:
            row += 32
            color = next(COLORS)
            tft.fill_rect(0, row-16, width, 32, ili9342c.BLACK)
            tft.draw(next(FONTS), next(GREETINGS), 0, row, color)

            if row > 192:
                row = 0

            utime.sleep(0.25)

    finally:
        # shutdown spi
        if 'spi' in locals():
            spi.deinit()

main()
