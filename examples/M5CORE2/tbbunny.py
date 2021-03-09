"""
tbbunny.py - Tiny Big Buck Bunny for the M5Stack Core2

The program sequentially reads, decodes and displays each frame from
individual JPG's stored on a SD Card. Originally my device did not have
enough RAM to hold a full sized 320x240 uncompressed image (153600 bytes)
I reduced the frame size to 160x120 (38400 bytes). The Core2 has enough
memory but the SPI interface is slow so I kept the 160x120 size.

The JPG files are available from https://github.com/russhughes/TinyBigBuckBunny
Requires the use the sdcard.py and axp202c.py driver available from the lib
directory or in the provided firmware.bin from the firmware/M5CORE2 directory.
"""

from machine import Pin, SPI
import uos
import sdcard
import ili9342c
import axp202c
import gc


def main():

    try:
        # Turn display power
        axp = axp202c.PMU(address=0x34)
        axp.enablePower(axp202c.AXP192_LDO2)
        # Set backlight voltage
        axp.setDC3Voltage(3000)

        spi = SPI(2, baudrate=20000000, sck=Pin(18), miso=Pin(38), mosi=Pin(23))

        # initialize and mount the sdcard
        sd_device = sdcard.SDCard(spi, cs=Pin(4), baudrate=20000000)
        uos.mount(sd_device, '/sd')

        # initialize display

        tft = ili9342c.ILI9342C(
            spi,
            320,
            240,
            cs=Pin(5, Pin.OUT),
            dc=Pin(15, Pin.OUT),
            rotation=0,
            buffer_size=38400)

        tft.init()

        for i in range(1, 2387):
            frame = "/sd/160x120/{:04d}.jpg".format(i)
            tft.jpg(frame, 80, 60, ili9342c.FAST)

    finally:
        # shutdown spi
        if 'spi' in locals():
            spi.deinit()


gc.collect()
main()
