'''
toasters.py

    An example using bitmap to draw sprites on the
    M5Stack Core Display.

    youtube video: https://youtu.be/0uWsjKQmCpU

    spritesheet from CircuitPython_Flying_Toasters
    https://learn.adafruit.com/circuitpython-sprite-animation-pendant-mario-clouds-flying-toasters
'''

import time
import random
from machine import Pin, SPI
import ili9342c
import axp202c
import t1
import t2
import t3
import t4
import t5

TOASTERS = [t1, t2, t3, t4]
TOAST = [t5]


class toast():
    '''
    toast class to keep track of a sprites locaton and step
    '''
    def __init__(self, sprites, x, y):
        self.sprites = sprites
        self.steps = len(sprites)
        self.x = x
        self.y = y
        self.step = random.randint(0, self.steps-1)
        self.speed = random.randint(2, 5)

    def move(self):
        if self.x <= 0:
            self.speed = random.randint(2, 5)
            self.x = 320-64

        self.step += 1
        self.step %= self.steps
        self.x -= self.speed


def main():
    '''
    Draw and move sprite
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
        tft.fill(ili9342c.BLACK)

        # create toast spites in random positions
        sprites = [
            toast(TOASTERS, 320-64, 0),
            toast(TOAST, 320-64*2, 80),
            toast(TOASTERS, 320-64*4, 160)
        ]

        # move and draw sprites
        while True:
            for man in sprites:
                bitmap = man.sprites[man.step]
                tft.fill_rect(
                    man.x+bitmap.WIDTH-man.speed,
                    man.y,
                    man.speed,
                    bitmap.HEIGHT,
                    ili9342c.BLACK)

                man.move()

                if man.x > 0:
                    tft.bitmap(bitmap, man.x, man.y)
                else:
                    tft.fill_rect(
                        0,
                        man.y,
                        bitmap.WIDTH,
                        bitmap.HEIGHT,
                        ili9342c.BLACK)

    finally:
        # shutdown spi
        if 'spi' in locals():
            spi.deinit()


main()