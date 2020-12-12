#ifndef __ILI9342C_H__
#define __ILI9342C_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ILI9342C_SWRESET 0x01

#define ILI9342C_CASET 0x2a
#define ILI9342C_PASET 0x2b
#define ILI9342C_RAMWR 0x2c
#define ILI9342C_INVOFF 0x20
#define ILI9342C_INVON 0x21
#define ILI9342C_MADCTL 0x36
#define ILI9342C_MADCTL_BGR 0x0
#define ILI9342C_MADCTL_RGB 0x08
#define ILI9342C_MADCTL_MX 0x40
#define ILI9342C_MADCTL_MV 0x20
#define ILI9342C_MADCTL_MY 0x80
#define ILI9342C_VSCRDEF 0x33
#define ILI9342C_VSCSAD 0x37
#define ILI9342C_SLPIN 0x10
#define ILI9342C_SLPOUT 0x11
#define ILI9342C_COLMOD 0x3A
#define ILI9342C_NORON 0x13
#define ILI9342C_DISPON 0x29
#define COLOR_MODE_65K 0x50
#define COLOR_MODE_262K 0x60
#define COLOR_MODE_12BIT 0x03
#define COLOR_MODE_16BIT  0x05
#define COLOR_MODE_18BIT 0x06
#define COLOR_MODE_16M 0x07

// Color definitions
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#ifdef  __cplusplus
}
#endif /*  __cplusplus */

#endif  /*  __ILI9342C_H__ */
