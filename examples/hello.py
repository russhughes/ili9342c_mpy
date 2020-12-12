"""
hello.py

    Writes "Hello!" in random colors at random locations on a
    M5Stack core display.

"""
import time
import random
from machine import Pin, SPI
import ili9342c

# Choose a font

# import vga1_8x8 as font
# import vga2_8x8 as font

# import vga1_8x16 as font
# import vga2_8x16 as font

# import vga1_16x16 as font
# import vga1_bold_16x16 as font
# import vga2_16x16 as font
# import vga2_bold_16x16 as font

# import vga1_16x32 as font
# import vga1_bold_16x32 as font
# import vga2_16x32 as font
import vga2_bold_16x32 as font

def main():
    tft = ili9342c.ILI9342C(
        SPI(2, baudrate=40000000, polarity=1, phase=1, sck=Pin(18), mosi=Pin(23)),
        320,
        240,
        reset=Pin(33, Pin.OUT),
        cs=Pin(14, Pin.OUT),
        dc=Pin(27, Pin.OUT),
        backlight=Pin(32, Pin.OUT),
        rotation=0)

    tft.init()
    tft.fill(0)
    time.sleep(3)

    while True:
        for rotation in range(4):
            tft.rotation(rotation)
            tft.fill(0)
            col_max = tft.width() - font.WIDTH*6
            row_max = tft.height() - font.HEIGHT

            for _ in range(250):
                tft.text(
                    font,
                    "Hello!",
                    random.randint(0, col_max),
                    random.randint(0, row_max),
                    ili9342c.color565(
                        random.getrandbits(8),
                        random.getrandbits(8),
                        random.getrandbits(8)),
                    ili9342c.color565(
                        random.getrandbits(8),
                        random.getrandbits(8),
                        random.getrandbits(8))
                )

main()
