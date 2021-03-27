'''
buttons.py

    A button press program for the M5CORE2 using the adafruit_focaltouh
    driver modified for micropython by franz schaefer from
    https://gitlab.com/mooond/t-watch2020-esp32-with-micropython

    I modied the axp202c driver to accept an initalized i2c object so it can
    be shared with the focaltouch driver. See the lib directory for source.
'''

from machine import Pin, SPI, SoftI2C
import utime
import axp202c
import ili9342c

import focaltouch
import vga1_16x16 as font

# button locations:
#  [Name, col, row, width, height]
BtnA = ['A', 3, 240, 102, 40]
BtnB = ['B', 109, 240, 102, 40]
BtnC = ['C', 215, 240, 102, 40]
buttons = [BtnA, BtnB, BtnC]


def button_pressed(touch):
    '''
    returns name of button touched or None if touch is outside of buttons
    '''
    for btn in buttons:
        if (btn[1] <= touch.touches[0]['x'] <= btn[1] + btn[3]
                and btn[2] <= touch.touches[0]['y'] <= btn[2] + btn[4]):
            return btn[0]

    return None


def main():
    '''
    buttons using focaltouch sensor
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
        tft.fill(ili9342c.BLUE)

        # enable focaltouch touchscreen
        touch = focaltouch.FocalTouch(i2c)

        last = None
        while True:
            # can be up to two touches, just checking first.
            if touch.touched == 1:
                pressed = button_pressed(touch)
                # button was pressed
                if pressed is not None:
                    button = pressed
                    # Check if button was held or new press
                    if (last == button):
                        # update display button still pressed
                        tft.text(
                            font,
                            "Button {} Held   ".format(button),
                            32,
                            120,
                            ili9342c.WHITE,
                            ili9342c.BLUE)
                    else:
                        # update display with new press
                        tft.text(
                            font,
                            "Button {} Pressed".format(button),
                            32,
                            120,
                            ili9342c.WHITE,
                            ili9342c.BLUE)

                    # wait half second for debounce
                    utime.sleep_ms(500)
                    # save last button pressed
                    last = button
                else:
                    # press was outside of button areas
                    button = None
            else:
                # no touch detected
                last = None

    finally:
        # shutdown
        spi.deinit()


main()
