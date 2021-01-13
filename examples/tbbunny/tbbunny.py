'''

'''

from machine import Pin, SPI
import uos
import sd as sdcard
import ili9342c
import time
import gc

gc.collect()

spi = SPI(2, baudrate=40000000, sck=Pin(18), mosi=Pin(23))
sd_device = sdcard.SDCard(spi, cs=Pin(4))
uos.mount(sd_device, '/sd')

tft = ili9342c.ILI9342C(
    spi,
    320,
    240,
    reset=Pin(33, Pin.OUT),
    cs=Pin(14, Pin.OUT),
    dc=Pin(27, Pin.OUT),
    backlight=Pin(32, Pin.OUT),
    rotation=0,
    buffer_size=38401)

tft.init()

for i in range(1, 2387):
    last = time.ticks_ms()
    frame = "/sd/160x120/{:04d}.jpg".format(i)
    tft.jpg(frame, 80, 60, ili9342c.FAST)
