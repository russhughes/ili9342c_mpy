/*
 * Copyright (c) 2020, 2021 Russ Hughes
 *
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ivan Belokobylskiy
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#define __ILI9342C_VERSION__ "0.0.4"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "py/obj.h"
#include "py/objstr.h"
#include "py/objmodule.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/mphal.h"

// Fix for MicroPython > 1.21 https://github.com/ricksorensen
#if MICROPY_VERSION_MAJOR >= 1 && MICROPY_VERSION_MINOR > 21
#include "extmod/modmachine.h"
#else
#include "extmod/machine_spi.h"
#endif

#include "mpfile.h"
#include "ili9342c.h"
#include "tjpgd565.h"

#define _swap_int16_t(a, b) \
	{                       \
		int16_t t = a;      \
		a		  = b;      \
		b		  = t;      \
	}

#define _swap_bytes(val) ((((val) >> 8) & 0x00FF) | (((val) << 8) & 0xFF00))
#define ABS(N) (((N) < 0) ? (-(N)) : (N))
#define mp_hal_delay_ms(delay) (mp_hal_delay_us(delay * 1000))

#define CS_LOW()                           \
	{                                      \
		if (self->cs) {                    \
			mp_hal_pin_write(self->cs, 0); \
		}                                  \
	}

#define CS_HIGH()                          \
	{                                      \
		if (self->cs) {                    \
			mp_hal_pin_write(self->cs, 1); \
		}                                  \
	}

#define DC_LOW() (mp_hal_pin_write(self->dc, 0))
#define DC_HIGH() (mp_hal_pin_write(self->dc, 1))

#define RESET_LOW()                           \
	{                                         \
		if (self->reset)                      \
			mp_hal_pin_write(self->reset, 0); \
	}

#define RESET_HIGH()                          \
	{                                         \
		if (self->reset)                      \
			mp_hal_pin_write(self->reset, 1); \
	}

// this is the actual C-structure for our new object
typedef struct _ili9342c_ILI9342C_obj_t {
	mp_obj_base_t	 base;
	mp_obj_base_t   *spi_obj;
	mp_file_t		*fp;				// file object
	uint16_t		*i2c_buffer;		// resident buffer if buffer_size given
	uint16_t	 	 buffer_size;		// resident buffer size, 0=dynamic
	uint16_t		 display_width;	 	// physical width
	uint16_t		 width;			 	// logical width (after rotation)
	uint16_t		 display_height; 	// physical width
	uint16_t		 height;		 	// logical height (after rotation)
	uint8_t			 rotation;
	mp_hal_pin_obj_t reset;
	mp_hal_pin_obj_t dc;
	mp_hal_pin_obj_t cs;
	mp_hal_pin_obj_t backlight;
} ili9342c_ILI9342C_obj_t;


// forward reference prototype
mp_obj_t ili9342c_ILI9342C_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);

//
//  methods start
//

static void ili9342c_ILI9342C_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
	(void) kind;
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(self_in);
	(void) self;
    mp_printf(print, "<ILI9342C  width=%u, height=%u, spi=%p>", self->width, self->height, self->spi_obj);
}

static void write_spi(mp_obj_base_t *spi_obj, const uint8_t *buf, int len) {
    #ifdef MP_OBJ_TYPE_GET_SLOT
    mp_machine_spi_p_t *spi_p = (mp_machine_spi_p_t *)MP_OBJ_TYPE_GET_SLOT(spi_obj->type, protocol);
    #else
    mp_machine_spi_p_t *spi_p = (mp_machine_spi_p_t *)spi_obj->type->protocol;
    #endif
    spi_p->transfer(spi_obj, len, buf, NULL);
}

static void write_cmd(ili9342c_ILI9342C_obj_t *self, uint8_t cmd, const uint8_t *data, int len) {
	CS_LOW()
	if (cmd) {
		DC_LOW();
		write_spi(self->spi_obj, &cmd, 1);
	}
	if (len > 0) {
		DC_HIGH();
		write_spi(self->spi_obj, data, len);
	}
	CS_HIGH()
}

static void set_window(ili9342c_ILI9342C_obj_t *self, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	if (x0 > x1 || x1 >= self->width) {
		return;
	}
	if (y0 > y1 || y1 >= self->height) {
		return;
	}
	uint8_t bufx[4] = {x0 >> 8, x0 & 0xFF, x1 >> 8, x1 & 0xFF};
	uint8_t bufy[4] = {y0 >> 8, y0 & 0xFF, y1 >> 8, y1 & 0xFF};
	write_cmd(self, ILI9342C_CASET, bufx, 4);
	write_cmd(self, ILI9342C_PASET, bufy, 4);
	write_cmd(self, ILI9342C_RAMWR, NULL, 0);
}

static void fill_color_buffer(mp_obj_base_t *spi_obj, uint16_t color, int length) {
	const int buffer_pixel_size = 128;
	int		  chunks			= length / buffer_pixel_size;
	int		  rest				= length % buffer_pixel_size;
	uint16_t  color_swapped		= _swap_bytes(color);
	uint16_t  buffer[buffer_pixel_size]; // 128 pixels

	// fill buffer with color data
	for (int i = 0; i < length && i < buffer_pixel_size; i++) {
		buffer[i] = color_swapped;
	}
	if (chunks) {
		for (int j = 0; j < chunks; j++) {
			write_spi(spi_obj, (uint8_t *) buffer, buffer_pixel_size * 2);
		}
	}
	if (rest) {
		write_spi(spi_obj, (uint8_t *) buffer, rest * 2);
	}
}

static void draw_pixel(ili9342c_ILI9342C_obj_t *self, uint16_t x, uint16_t y, uint16_t color) {
	uint8_t hi = color >> 8, lo = color;
	set_window(self, x, y, x, y);
	DC_HIGH();
	CS_LOW();
	write_spi(self->spi_obj, &hi, 1);
	write_spi(self->spi_obj, &lo, 1);
	CS_HIGH();
}

static void fast_hline(ili9342c_ILI9342C_obj_t *self, uint16_t x, uint16_t y, uint16_t _w, uint16_t color) {
	int w;

	if (x + _w > self->width)
		w = self->width - x;
	else
		w = _w;

	if (w > 0) {
		set_window(self, x, y, x + w - 1, y);
		DC_HIGH();
		CS_LOW();
		fill_color_buffer(self->spi_obj, color, w);
		CS_HIGH();
	}
}

static void fast_vline(ili9342c_ILI9342C_obj_t *self, uint16_t x, uint16_t y, uint16_t w, uint16_t color) {
	set_window(self, x, y, x, y + w - 1);
	DC_HIGH();
	CS_LOW();
	fill_color_buffer(self->spi_obj, color, w);
	CS_HIGH();
}

static mp_obj_t ili9342c_ILI9342C_hard_reset(mp_obj_t self_in) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(self_in);

	CS_LOW();
	RESET_HIGH();
	mp_hal_delay_ms(50);
	RESET_LOW();
	mp_hal_delay_ms(50);
	RESET_HIGH();
	mp_hal_delay_ms(150);
	CS_HIGH();
	return mp_const_none;
}

static mp_obj_t ili9342c_ILI9342C_soft_reset(mp_obj_t self_in) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(self_in);

	write_cmd(self, ILI9342C_SWRESET, NULL, 0);
	mp_hal_delay_ms(150);
	return mp_const_none;
}

/*
static mp_obj_t ili9342c_ILI9342C_write(mp_obj_t self_in, mp_obj_t command, mp_obj_t data) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(self_in);
	mp_buffer_info_t		 src;

	if (data == mp_const_none) {
		write_cmd(self, (uint8_t) mp_obj_get_int(command), NULL, 0);
	} else {
		mp_get_buffer_raise(data, &src, MP_BUFFER_READ);
		write_cmd(self, (uint8_t) mp_obj_get_int(command), (const uint8_t *) src.buf, src.len);
	}
	return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_3(ili9342c_ILI9342C_write_obj, ili9342c_ILI9342C_write);
*/

MP_DEFINE_CONST_FUN_OBJ_1(ili9342c_ILI9342C_hard_reset_obj, ili9342c_ILI9342C_hard_reset);
MP_DEFINE_CONST_FUN_OBJ_1(ili9342c_ILI9342C_soft_reset_obj, ili9342c_ILI9342C_soft_reset);

static mp_obj_t ili9342c_ILI9342C_sleep_mode(mp_obj_t self_in, mp_obj_t value) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(self_in);
	if (mp_obj_is_true(value)) {
		write_cmd(self, ILI9342C_SLPIN, NULL, 0);
	} else {
		write_cmd(self, ILI9342C_SLPOUT, NULL, 0);
	}
	return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(ili9342c_ILI9342C_sleep_mode_obj, ili9342c_ILI9342C_sleep_mode);

static mp_obj_t ili9342c_ILI9342C_set_window(size_t n_args, const mp_obj_t *args) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(args[0]);

	mp_int_t x0 = mp_obj_get_int(args[1]);
	mp_int_t x1 = mp_obj_get_int(args[2]);
	mp_int_t y0 = mp_obj_get_int(args[3]);
	mp_int_t y1 = mp_obj_get_int(args[4]);

	set_window(self, x0, y0, x1, y1);
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ili9342c_ILI9342C_set_window_obj, 5, 5, ili9342c_ILI9342C_set_window);
static mp_obj_t ili9342c_ILI9342C_inversion_mode(mp_obj_t self_in, mp_obj_t value) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(self_in);

	if (mp_obj_is_true(value)) {
		write_cmd(self, ILI9342C_INVON, NULL, 0);
	} else {
		write_cmd(self, ILI9342C_INVOFF, NULL, 0);
	}
	return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(ili9342c_ILI9342C_inversion_mode_obj, ili9342c_ILI9342C_inversion_mode);

static mp_obj_t ili9342c_ILI9342C_fill_rect(size_t n_args, const mp_obj_t *args) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(args[0]);

	mp_int_t x	   = mp_obj_get_int(args[1]);
	mp_int_t y	   = mp_obj_get_int(args[2]);
	mp_int_t w	   = mp_obj_get_int(args[3]);
	mp_int_t h	   = mp_obj_get_int(args[4]);
	mp_int_t color = mp_obj_get_int(args[5]);

	set_window(self, x, y, x + w - 1, y + h - 1);
	DC_HIGH();
	CS_LOW();
	fill_color_buffer(self->spi_obj, color, w * h);
	CS_HIGH();

	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ili9342c_ILI9342C_fill_rect_obj, 6, 6, ili9342c_ILI9342C_fill_rect);

static mp_obj_t ili9342c_ILI9342C_fill(mp_obj_t self_in, mp_obj_t _color) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(self_in);

	mp_int_t color = mp_obj_get_int(_color);

	set_window(self, 0, 0, self->width - 1, self->height - 1);
	DC_HIGH();
	CS_LOW();
	fill_color_buffer(self->spi_obj, color, self->width * self->height);
	CS_HIGH();

	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(ili9342c_ILI9342C_fill_obj, ili9342c_ILI9342C_fill);

static mp_obj_t ili9342c_ILI9342C_pixel(size_t n_args, const mp_obj_t *args) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(args[0]);

	mp_int_t x	   = mp_obj_get_int(args[1]);
	mp_int_t y	   = mp_obj_get_int(args[2]);
	mp_int_t color = mp_obj_get_int(args[3]);

	draw_pixel(self, x, y, color);

	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ili9342c_ILI9342C_pixel_obj, 4, 4, ili9342c_ILI9342C_pixel);

static void line(ili9342c_ILI9342C_obj_t *self, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t color) {
	bool steep = ABS(y1 - y0) > ABS(x1 - x0);
	if (steep) {
		_swap_int16_t(x0, y0);
		_swap_int16_t(x1, y1);
	}

	if (x0 > x1) {
		_swap_int16_t(x0, x1);
		_swap_int16_t(y0, y1);
	}

	int16_t dx = x1 - x0, dy = ABS(y1 - y0);
	int16_t err = dx >> 1, ystep = -1, xs = x0, dlen = 0;

	if (y0 < y1)
		ystep = 1;

	// Split into steep and not steep for FastH/V separation
	if (steep) {
		for (; x0 <= x1; x0++) {
			dlen++;
			err -= dy;
			if (err < 0) {
				err += dx;
				if (dlen == 1)
					draw_pixel(self, y0, xs, color);
				else
					fast_vline(self, y0, xs, dlen, color);
				dlen = 0;
				y0 += ystep;
				xs = x0 + 1;
			}
		}
		if (dlen)
			fast_vline(self, y0, xs, dlen, color);
	} else {
		for (; x0 <= x1; x0++) {
			dlen++;
			err -= dy;
			if (err < 0) {
				err += dx;
				if (dlen == 1)
					draw_pixel(self, xs, y0, color);
				else
					fast_hline(self, xs, y0, dlen, color);
				dlen = 0;
				y0 += ystep;
				xs = x0 + 1;
			}
		}
		if (dlen)
			fast_hline(self, xs, y0, dlen, color);
	}
}


static mp_obj_t ili9342c_ILI9342C_line(size_t n_args, const mp_obj_t *args) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(args[0]);

	mp_int_t x0	   = mp_obj_get_int(args[1]);
	mp_int_t y0	   = mp_obj_get_int(args[2]);
	mp_int_t x1	   = mp_obj_get_int(args[3]);
	mp_int_t y1	   = mp_obj_get_int(args[4]);
	mp_int_t color = mp_obj_get_int(args[5]);

	line(self, x0, y0, x1, y1, color);

	return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ili9342c_ILI9342C_line_obj, 6, 6, ili9342c_ILI9342C_line);

static mp_obj_t ili9342c_ILI9342C_blit_buffer(size_t n_args, const mp_obj_t *args) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(args[0]);

	mp_buffer_info_t buf_info;
	mp_get_buffer_raise(args[1], &buf_info, MP_BUFFER_READ);
	mp_int_t x = mp_obj_get_int(args[2]);
	mp_int_t y = mp_obj_get_int(args[3]);
	mp_int_t w = mp_obj_get_int(args[4]);
	mp_int_t h = mp_obj_get_int(args[5]);

	set_window(self, x, y, x + w - 1, y + h - 1);
	DC_HIGH();
	CS_LOW();

	const int buf_size = 256;
	int		  limit	   = MIN(buf_info.len, w * h * 2);
	int		  chunks   = limit / buf_size;
	int		  rest	   = limit % buf_size;
	int		  i		   = 0;

	for (; i < chunks; i++) {
		write_spi(self->spi_obj, (const uint8_t *) buf_info.buf + i * buf_size, buf_size);
	}

	if (rest) {
		write_spi(self->spi_obj, (const uint8_t *) buf_info.buf + i * buf_size, rest);
	}
	CS_HIGH();

	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ili9342c_ILI9342C_blit_buffer_obj, 6, 6, ili9342c_ILI9342C_blit_buffer);


static mp_obj_t ili9342c_ILI9342C_draw(size_t n_args, const mp_obj_t *args) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(args[0]);
	char		single_char_s[] = {0, 0};
	const char *s;

	mp_obj_module_t *hershey   = MP_OBJ_TO_PTR(args[1]);

	if (mp_obj_is_int(args[2])) {
		mp_int_t c		 = mp_obj_get_int(args[2]);
		single_char_s[0] = c & 0xff;
		s				 = single_char_s;
	} else {
		s= mp_obj_str_get_str(args[2]);
	}

	mp_int_t		 x		   = mp_obj_get_int(args[3]);
	mp_int_t		 y		   = mp_obj_get_int(args[4]);
	mp_int_t		 color     = mp_obj_get_int(args[5]);

    mp_float_t scale = 1.0;
    if (mp_obj_is_float(args[6])) {
        scale = mp_obj_float_get(args[6]);
    }

    if (mp_obj_is_int(args[6])) {
        scale =  (float) mp_obj_get_int(args[6]);
    }

	mp_obj_dict_t *	 dict	   = MP_OBJ_TO_PTR(hershey->globals);
	mp_obj_t *		 index_data_buff = mp_obj_dict_get(dict, MP_OBJ_NEW_QSTR(MP_QSTR_INDEX));
	mp_buffer_info_t index_bufinfo;
	mp_get_buffer_raise(index_data_buff, &index_bufinfo, MP_BUFFER_READ);
	uint8_t *index = index_bufinfo.buf;

	mp_obj_t *		 font_data_buff = mp_obj_dict_get(dict, MP_OBJ_NEW_QSTR(MP_QSTR_FONT));
	mp_buffer_info_t font_bufinfo;
	mp_get_buffer_raise(font_data_buff, &font_bufinfo, MP_BUFFER_READ);
	int8_t *font = font_bufinfo.buf;

    int16_t from_x = x;
    int16_t from_y  = y;
    int16_t to_x = x;
    int16_t to_y = y;
    int16_t pos_x = x;
    int16_t pos_y = y;
    bool penup = true;
    char c;
    int16_t ii;

    while ((c = *s++)) {
        if (c >= 32 && c <= 127) {
            ii = (c-32) * 2;

			int16_t offset = index[ii] | (index[ii+1] << 8);
            int16_t length = font[offset++];
            int16_t left = lround((font[offset++] - 0x52) * scale);
            int16_t right = lround((font[offset++] - 0x52) * scale);
            int16_t width = right - left;

            if (length) {
                int16_t i;
                for (i = 0; i < length; i++) {
                    if (font[offset] == ' ') {
                        offset+=2;
                        penup = true;
                        continue;
                    }

                    int16_t vector_x = lround((font[offset++] - 0x52) * scale);
                    int16_t vector_y = lround((font[offset++] - 0x52) * scale);

                    if (!i ||  penup) {
                        from_x = pos_x + vector_x - left;
                        from_y = pos_y + vector_y;
                    } else {
                        to_x = pos_x + vector_x - left;
                        to_y = pos_y + vector_y;

                        line(self, from_x, from_y, to_x, to_y, color);
                        from_x = to_x;
                        from_y = to_y;
                    }
                    penup = false;
                }
            }
			pos_x += width;
        }
    }

	return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ili9342c_ILI9342C_draw_obj, 6, 7, ili9342c_ILI9342C_draw);


static uint32_t bs_bit		= 0;
uint8_t * bitmap_data = NULL;

uint8_t get_color(uint8_t bpp) {
	uint8_t color = 0;
	int		i;

	for (i = 1; i < bpp; i++) {
		color <<= 1;
		color |= (bitmap_data[bs_bit / 8] & 1 << (7 - (bs_bit % 8))) > 0;
		bs_bit++;
	}
	return color;
}


mp_obj_t dict_lookup(mp_obj_t self_in, mp_obj_t index) {
    mp_obj_dict_t *self = MP_OBJ_TO_PTR(self_in);
    mp_map_elem_t *elem = mp_map_lookup(&self->map, index, MP_MAP_LOOKUP);
    if (elem == NULL) {
        return NULL;
    } else {
        return elem->value;
    }
}

static mp_obj_t ili9342c_ILI9342C_write_len(size_t n_args, const mp_obj_t *args) {
	mp_obj_module_t *font = MP_OBJ_TO_PTR(args[1]);
	char single_char_s[2] = {0, 0};
	const char *str;

	if (mp_obj_is_int(args[2])) {
		mp_int_t c = mp_obj_get_int(args[2]);
		single_char_s[0] = c & 0xff;
		str	= single_char_s;
	} else {
		str = mp_obj_str_get_str(args[2]);
	}

	mp_obj_dict_t *dict	= MP_OBJ_TO_PTR(font->globals);
	const char 	  *map  = mp_obj_str_get_str(mp_obj_dict_get(dict, MP_OBJ_NEW_QSTR(MP_QSTR_MAP)));

	mp_obj_t widths_data_buff = mp_obj_dict_get(dict, MP_OBJ_NEW_QSTR(MP_QSTR_WIDTHS));
	mp_buffer_info_t widths_bufinfo;
	mp_get_buffer_raise(widths_data_buff, &widths_bufinfo, MP_BUFFER_READ);
	const uint8_t *widths_data = widths_bufinfo.buf;

	uint16_t print_width = 0;
	uint8_t chr;

	while ((chr = *str++)) {
		char *char_pointer = strchr(map, chr);
		if (char_pointer) {
			uint16_t char_index = char_pointer - map;
			print_width += widths_data[char_index];
        }
    }
	return mp_obj_new_int(print_width);
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ili9342c_ILI9342C_write_len_obj, 3, 3, ili9342c_ILI9342C_write_len);


//
//	write(font_module, s, x, y[, fg, bg])
//

static mp_obj_t ili9342c_ILI9342C_write(size_t n_args, const mp_obj_t *args) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(args[0]);
	mp_obj_module_t *font = MP_OBJ_TO_PTR(args[1]);

	char single_char_s[2] = {0, 0};
	const char *str;

	if (mp_obj_is_int(args[2])) {
		mp_int_t c = mp_obj_get_int(args[2]);
		single_char_s[0] = c & 0xff;
		str	= single_char_s;
	} else {
		str = mp_obj_str_get_str(args[2]);
	}

	mp_int_t x = mp_obj_get_int(args[3]);
	mp_int_t y = mp_obj_get_int(args[4]);
	mp_int_t fg_color;
	mp_int_t bg_color;

	fg_color = (n_args > 5) ? _swap_bytes(mp_obj_get_int(args[5])) : _swap_bytes(WHITE);
	bg_color = (n_args > 6) ? _swap_bytes(mp_obj_get_int(args[6])) : _swap_bytes(BLACK);

	mp_obj_dict_t *dict			  = MP_OBJ_TO_PTR(font->globals);
	const char 	  *map 			  = mp_obj_str_get_str(mp_obj_dict_get(dict, MP_OBJ_NEW_QSTR(MP_QSTR_MAP)));
	const uint8_t  bpp			  = mp_obj_get_int(mp_obj_dict_get(dict, MP_OBJ_NEW_QSTR(MP_QSTR_BPP)));
	const uint8_t  height		  = mp_obj_get_int(mp_obj_dict_get(dict, MP_OBJ_NEW_QSTR(MP_QSTR_HEIGHT)));
	const uint8_t  offset_width	  = mp_obj_get_int(mp_obj_dict_get(dict, MP_OBJ_NEW_QSTR(MP_QSTR_OFFSET_WIDTH)));
	const uint8_t  max_width	  = mp_obj_get_int(mp_obj_dict_get(dict, MP_OBJ_NEW_QSTR(MP_QSTR_MAX_WIDTH)));

	mp_obj_t widths_data_buff = mp_obj_dict_get(dict, MP_OBJ_NEW_QSTR(MP_QSTR_WIDTHS));
	mp_buffer_info_t widths_bufinfo;
	mp_get_buffer_raise(widths_data_buff, &widths_bufinfo, MP_BUFFER_READ);
	const uint8_t *widths_data = widths_bufinfo.buf;

	mp_obj_t offsets_data_buff = mp_obj_dict_get(dict, MP_OBJ_NEW_QSTR(MP_QSTR_OFFSETS));
	mp_buffer_info_t offsets_bufinfo;
	mp_get_buffer_raise(offsets_data_buff, &offsets_bufinfo, MP_BUFFER_READ);
	const uint8_t *offsets_data = offsets_bufinfo.buf;

	mp_obj_t bitmaps_data_buff = mp_obj_dict_get(dict, MP_OBJ_NEW_QSTR(MP_QSTR_BITMAPS));
	mp_buffer_info_t bitmaps_bufinfo;
	mp_get_buffer_raise(bitmaps_data_buff, &bitmaps_bufinfo, MP_BUFFER_READ);
	bitmap_data = bitmaps_bufinfo.buf;

	uint32_t buf_size = max_width * height * 2;
	if (self->buffer_size == 0) {
		self->i2c_buffer = m_malloc(buf_size);
	}

	uint16_t print_width = 0;
	uint8_t chr;

	while ((chr = *str++)) {
		char *char_pointer = strchr(map, chr);
		if (char_pointer) {
			uint16_t char_index = char_pointer - map;
			uint8_t width = widths_data[char_index];

			bs_bit = 0;
			switch (offset_width) {
				case 1:
					bs_bit = offsets_data[char_index * offset_width];
					break;

				case 2:
					bs_bit = (offsets_data[char_index * offset_width] << 8) +
                			 (offsets_data[char_index * offset_width + 1]);
					break;

				case 3:
					bs_bit = (offsets_data[char_index * offset_width] << 16) +
                		     (offsets_data[char_index * offset_width + 1] << 8) +
                		     (offsets_data[char_index * offset_width + 2]);
					break;
			}

			uint32_t ofs = 0;
			for (int yy = 0; yy < height; yy++) {
				for (int xx = 0; xx < width; xx++) {
					self->i2c_buffer[ofs++] = get_color(bpp + 1) ? fg_color : bg_color;
				}
			}

			uint32_t data_size = width * height * 2;
			uint16_t x1 = x + width - 1;
			if (x1 < self->width) {
				set_window(self, x, y, x1, y + height - 1);
				DC_HIGH();
				CS_LOW();
				write_spi(self->spi_obj, (uint8_t *) self->i2c_buffer, data_size);
				CS_HIGH();
				print_width += width;
			}
			else
				break;

			x += width;
		}
	}

	if (self->buffer_size == 0) {
		m_free(self->i2c_buffer);
	}

	return mp_obj_new_int(print_width);
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ili9342c_ILI9342C_write_obj, 5, 7, ili9342c_ILI9342C_write);


static mp_obj_t ili9342c_ILI9342C_bitmap(size_t n_args, const mp_obj_t *args) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(args[0]);

	mp_obj_module_t *bitmap		 = MP_OBJ_TO_PTR(args[1]);
	mp_int_t		 x			 = mp_obj_get_int(args[2]);
	mp_int_t		 y			 = mp_obj_get_int(args[3]);

    mp_int_t idx;
    if (n_args > 3) {
        idx = mp_obj_get_int(args[4]);
    } else {
        idx = 0;
    }

	mp_obj_dict_t *	 dict		 = MP_OBJ_TO_PTR(bitmap->globals);
	const uint16_t	 height		 = mp_obj_get_int(mp_obj_dict_get(dict, MP_OBJ_NEW_QSTR(MP_QSTR_HEIGHT)));
	const uint16_t	 width		 = mp_obj_get_int(mp_obj_dict_get(dict, MP_OBJ_NEW_QSTR(MP_QSTR_WIDTH)));
	uint16_t         bitmaps     = 0;
	const uint8_t	 bpp		 = mp_obj_get_int(mp_obj_dict_get(dict, MP_OBJ_NEW_QSTR(MP_QSTR_BPP)));
	mp_obj_t *		 palette_arg = mp_obj_dict_get(dict, MP_OBJ_NEW_QSTR(MP_QSTR_PALETTE));
	mp_obj_t *		 palette	 = NULL;
	size_t			 palette_len = 0;

	mp_map_elem_t *elem = dict_lookup(bitmap->globals, MP_OBJ_NEW_QSTR(MP_QSTR_BITMAPS));
    if (elem) {
        bitmaps = mp_obj_get_int(elem);
    }

	mp_obj_get_array(palette_arg, &palette_len, &palette);

	mp_obj_t *		 bitmap_data_buff = mp_obj_dict_get(dict, MP_OBJ_NEW_QSTR(MP_QSTR_BITMAP));
	mp_buffer_info_t bufinfo;

	mp_get_buffer_raise(bitmap_data_buff, &bufinfo, MP_BUFFER_READ);
	bitmap_data = bufinfo.buf;

	uint32_t buf_size = width * height * 2;
	if (self->buffer_size == 0) {
		self->i2c_buffer = m_malloc(buf_size);
	}

	uint32_t ofs = 0;
    bs_bit = 0;
    if (bitmaps) {
        if (idx < bitmaps ) {
            bs_bit = height * width * bpp * idx;
        } else {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("index out of range"));
        }
    }

	for (int yy = 0; yy < height; yy++) {
		for (int xx = 0; xx < width; xx++) {
			self->i2c_buffer[ofs++] = mp_obj_get_int(palette[get_color(bpp + 1)]);
		}
	}

	uint16_t x1 = x + width - 1;
	if (x1 < self->width) {
		set_window(self, x, y, x1, y + height - 1);
		DC_HIGH();
		CS_LOW();
		write_spi(self->spi_obj, (uint8_t *) self->i2c_buffer, buf_size);
		CS_HIGH();
	}

	if (self->buffer_size == 0) {
		m_free(self->i2c_buffer);
	}
	return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ili9342c_ILI9342C_bitmap_obj, 4, 5, ili9342c_ILI9342C_bitmap);

static mp_obj_t ili9342c_ILI9342C_text(size_t n_args, const mp_obj_t *args) {
	char		single_char_s[2] = {0, 0};
	const char *str;

	// extract arguments
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(args[0]);
	mp_obj_module_t *		 font = MP_OBJ_TO_PTR(args[1]);

	if (mp_obj_is_int(args[2])) {
		mp_int_t c		 = mp_obj_get_int(args[2]);
		single_char_s[0] = c & 0xff;
		str				 = single_char_s;
	} else {
		str = mp_obj_str_get_str(args[2]);
	}

	mp_int_t x0 = mp_obj_get_int(args[3]);
	mp_int_t y0 = mp_obj_get_int(args[4]);

	mp_obj_dict_t *dict			  = MP_OBJ_TO_PTR(font->globals);
	const uint8_t  width		  = mp_obj_get_int(mp_obj_dict_get(dict, MP_OBJ_NEW_QSTR(MP_QSTR_WIDTH)));
	const uint8_t  height		  = mp_obj_get_int(mp_obj_dict_get(dict, MP_OBJ_NEW_QSTR(MP_QSTR_HEIGHT)));
	const uint8_t  first		  = mp_obj_get_int(mp_obj_dict_get(dict, MP_OBJ_NEW_QSTR(MP_QSTR_FIRST)));
	const uint8_t  last			  = mp_obj_get_int(mp_obj_dict_get(dict, MP_OBJ_NEW_QSTR(MP_QSTR_LAST)));
	mp_obj_t	   font_data_buff = mp_obj_dict_get(dict, MP_OBJ_NEW_QSTR(MP_QSTR_FONT));

	mp_buffer_info_t bufinfo;
	mp_get_buffer_raise(font_data_buff, &bufinfo, MP_BUFFER_READ);
	const uint8_t *font_data = bufinfo.buf;

	mp_int_t fg_color;
	mp_int_t bg_color;

	if (n_args > 5)
		fg_color = _swap_bytes(mp_obj_get_int(args[5]));
	else
		fg_color = _swap_bytes(WHITE);

	if (n_args > 6)
		bg_color = _swap_bytes(mp_obj_get_int(args[6]));
	else
		bg_color = _swap_bytes(BLACK);

	uint8_t	 wide	  = width / 8;
	uint16_t buf_size = width * height * 2;

	if (self->buffer_size == 0) {
		self->i2c_buffer = m_malloc(buf_size);
	}

	if (self->i2c_buffer) {
		uint8_t chr;
		while ((chr = *str++)) {
			if (chr >= first && chr <= last) {
				uint16_t buf_idx = 0;
				uint16_t chr_idx = (chr - first) * (height * wide);
				for (uint8_t line = 0; line < height; line++) {
					for (uint8_t line_byte = 0; line_byte < wide; line_byte++) {
						uint8_t chr_data = font_data[chr_idx];
						for (uint8_t bit = 8; bit; bit--) {
							if (chr_data >> (bit - 1) & 1)
								self->i2c_buffer[buf_idx] = fg_color;
							else
								self->i2c_buffer[buf_idx] = bg_color;
							buf_idx++;
						}
						chr_idx++;
					}
				}
				uint16_t x1 = x0 + width - 1;
				if (x1 < self->width) {
					set_window(self, x0, y0, x1, y0 + height - 1);
					DC_HIGH();
					CS_LOW();
					write_spi(self->spi_obj, (uint8_t *) self->i2c_buffer, buf_size);
					CS_HIGH();
				}
				x0 += width;
			}
		}
		if (self->buffer_size == 0) {
			m_free(self->i2c_buffer);
		}
	}
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ili9342c_ILI9342C_text_obj, 5, 7, ili9342c_ILI9342C_text);

static void set_rotation(ili9342c_ILI9342C_obj_t *self) {
	uint8_t madctl_value = ILI9342C_MADCTL_RGB;

	if (self->rotation == 0) { // Portrait
		self->width	 = self->display_width;
		self->height = self->display_height;
	} else if (self->rotation == 1) { // Landscape
		madctl_value |= ILI9342C_MADCTL_MX | ILI9342C_MADCTL_MV;
		self->width	 = self->display_height;
		self->height = self->display_width;
	} else if (self->rotation == 2) { // Inverted Portrait
		madctl_value |= ILI9342C_MADCTL_MX | ILI9342C_MADCTL_MY;
		self->width	 = self->display_width;
		self->height = self->display_height;
	} else if (self->rotation == 3) { // Inverted Landscape
		madctl_value |= ILI9342C_MADCTL_MV | ILI9342C_MADCTL_MY;
		self->width	 = self->display_height;
		self->height = self->display_width;
	} else if (self->rotation == 4) { // Portrait Mirrored
		madctl_value |= ILI9342C_MADCTL_MX;
		self->width	 = self->display_width;
		self->height = self->display_height;
	} else if (self->rotation == 5) { // Landscape Mirrored
		madctl_value |=  ILI9342C_MADCTL_MV;
		self->width	 = self->display_height;
		self->height = self->display_width;
	} else if (self->rotation == 6) { // Inverted Portrait Mirrored
		madctl_value |= ILI9342C_MADCTL_MY;
		self->width	 = self->display_width;
		self->height = self->display_height;
	} else if (self->rotation == 7) { // Inverted Landscape Mirrored
		madctl_value |= ILI9342C_MADCTL_MX | ILI9342C_MADCTL_MY | ILI9342C_MADCTL_MV;
		self->width	 = self->display_height;
		self->height = self->display_width;
	}

	const uint8_t madctl[] = {madctl_value};
	write_cmd(self, ILI9342C_MADCTL, madctl, 1);
}


static mp_obj_t ili9342c_ILI9342C_rotation(mp_obj_t self_in, mp_obj_t value) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(self_in);

	mp_int_t rotation = mp_obj_get_int(value) % 8;
	self->rotation	  = rotation;
	set_rotation(self);
	return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(ili9342c_ILI9342C_rotation_obj, ili9342c_ILI9342C_rotation);


static mp_obj_t ili9342c_ILI9342C_width(mp_obj_t self_in) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(self_in);
	return mp_obj_new_int(self->width);
}

MP_DEFINE_CONST_FUN_OBJ_1(ili9342c_ILI9342C_width_obj, ili9342c_ILI9342C_width);

static mp_obj_t ili9342c_ILI9342C_height(mp_obj_t self_in) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(self_in);
	return mp_obj_new_int(self->height);
}

MP_DEFINE_CONST_FUN_OBJ_1(ili9342c_ILI9342C_height_obj, ili9342c_ILI9342C_height);

static mp_obj_t ili9342c_ILI9342C_vscrdef(size_t n_args, const mp_obj_t *args) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(args[0]);

	mp_int_t tfa = mp_obj_get_int(args[1]);
	mp_int_t vsa = mp_obj_get_int(args[2]);
	mp_int_t bfa = mp_obj_get_int(args[3]);

	uint8_t buf[6] = {(tfa) >> 8, (tfa) &0xFF, (vsa) >> 8, (vsa) &0xFF, (bfa) >> 8, (bfa) &0xFF};
	write_cmd(self, ILI9342C_VSCRDEF, buf, 6);

	return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ili9342c_ILI9342C_vscrdef_obj, 4, 4, ili9342c_ILI9342C_vscrdef);

static mp_obj_t ili9342c_ILI9342C_vscsad(mp_obj_t self_in, mp_obj_t vssa_in) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(self_in);

	mp_int_t vssa	= mp_obj_get_int(vssa_in);
	uint8_t	 buf[2] = {(vssa) >> 8, (vssa) &0xFF};
	write_cmd(self, ILI9342C_VSCSAD, buf, 2);

	return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(ili9342c_ILI9342C_vscsad_obj, ili9342c_ILI9342C_vscsad);

static mp_obj_t ili9342c_ILI9342C_init(mp_obj_t self_in) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(self_in);
	ili9342c_ILI9342C_hard_reset(self_in);
	ili9342c_ILI9342C_soft_reset(self_in);
	write_cmd(self, ILI9342C_SLPOUT, NULL, 0);

	const uint8_t color_mode[] = {COLOR_MODE_65K | COLOR_MODE_16BIT};
	write_cmd(self, ILI9342C_COLMOD, color_mode, 1);
	mp_hal_delay_ms(10);

	set_rotation(self);

	write_cmd(self, ILI9342C_INVON, NULL, 0);
	mp_hal_delay_ms(10);
	write_cmd(self, ILI9342C_NORON, NULL, 0);
	mp_hal_delay_ms(10);

	const mp_obj_t args[] = {
		self_in,
		mp_obj_new_int(0),
		mp_obj_new_int(0),
		mp_obj_new_int(self->width),
		mp_obj_new_int(self->height),
		mp_obj_new_int(BLACK)};
	ili9342c_ILI9342C_fill_rect(6, args);

	if (self->backlight)
		mp_hal_pin_write(self->backlight, 1);

	write_cmd(self, ILI9342C_DISPON, NULL, 0);
	mp_hal_delay_ms(500);

	return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(ili9342c_ILI9342C_init_obj, ili9342c_ILI9342C_init);

static mp_obj_t ili9342c_ILI9342C_hline(size_t n_args, const mp_obj_t *args) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(args[0]);

	mp_int_t x	   = mp_obj_get_int(args[1]);
	mp_int_t y	   = mp_obj_get_int(args[2]);
	mp_int_t w	   = mp_obj_get_int(args[3]);
	mp_int_t color = mp_obj_get_int(args[4]);

	fast_hline(self, x, y, w, color);

	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ili9342c_ILI9342C_hline_obj, 5, 5, ili9342c_ILI9342C_hline);

static mp_obj_t ili9342c_ILI9342C_vline(size_t n_args, const mp_obj_t *args) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(args[0]);

	mp_int_t x	   = mp_obj_get_int(args[1]);
	mp_int_t y	   = mp_obj_get_int(args[2]);
	mp_int_t w	   = mp_obj_get_int(args[3]);
	mp_int_t color = mp_obj_get_int(args[4]);

	fast_vline(self, x, y, w, color);

	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ili9342c_ILI9342C_vline_obj, 5, 5, ili9342c_ILI9342C_vline);

static mp_obj_t ili9342c_ILI9342C_rect(size_t n_args, const mp_obj_t *args) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(args[0]);

	mp_int_t x	   = mp_obj_get_int(args[1]);
	mp_int_t y	   = mp_obj_get_int(args[2]);
	mp_int_t w	   = mp_obj_get_int(args[3]);
	mp_int_t h	   = mp_obj_get_int(args[4]);
	mp_int_t color = mp_obj_get_int(args[5]);

	fast_hline(self, x, y, w, color);
	fast_vline(self, x, y, h, color);
	fast_hline(self, x, y + h - 1, w, color);
	fast_vline(self, x + w - 1, y, h, color);
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ili9342c_ILI9342C_rect_obj, 6, 6, ili9342c_ILI9342C_rect);

static uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
}

static mp_obj_t ili9342c_color565(mp_obj_t r, mp_obj_t g, mp_obj_t b) {
	return MP_OBJ_NEW_SMALL_INT(color565(
		(uint8_t) mp_obj_get_int(r),
		(uint8_t) mp_obj_get_int(g),
		(uint8_t) mp_obj_get_int(b)));
}
static MP_DEFINE_CONST_FUN_OBJ_3(ili9342c_color565_obj, ili9342c_color565);

static void map_bitarray_to_rgb565(uint8_t const *bitarray, uint8_t *buffer, int length, int width, uint16_t color, uint16_t bg_color) {
	int row_pos = 0;
	for (int i = 0; i < length; i++) {
		uint8_t byte = bitarray[i];
		for (int bi = 7; bi >= 0; bi--) {
			uint8_t	 b		   = byte & (1 << bi);
			uint16_t cur_color = b ? color : bg_color;
			*buffer			   = (cur_color & 0xff00) >> 8;
			buffer++;
			*buffer = cur_color & 0xff;
			buffer++;

			row_pos++;
			if (row_pos >= width) {
				row_pos = 0;
				break;
			}
		}
	}
}

// bitarray buffer width color bg_color

static mp_obj_t ili9342c_map_bitarray_to_rgb565(size_t n_args, const mp_obj_t *args) {
	mp_buffer_info_t bitarray_info;
	mp_buffer_info_t buffer_info;

	mp_get_buffer_raise(args[1], &bitarray_info, MP_BUFFER_READ);
	mp_get_buffer_raise(args[2], &buffer_info, MP_BUFFER_WRITE);
	mp_int_t width	  = mp_obj_get_int(args[3]);
	mp_int_t color	  = mp_obj_get_int(args[4]);
	mp_int_t bg_color = mp_obj_get_int(args[5]);
	map_bitarray_to_rgb565(bitarray_info.buf, buffer_info.buf, bitarray_info.len, width, color, bg_color);
	return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ili9342c_map_bitarray_to_rgb565_obj, 3, 6, ili9342c_map_bitarray_to_rgb565);

//
// jpg routines
//

#define JPG_MODE_FAST (0)
#define JPG_MODE_SLOW (1)

// User defined device identifier
typedef struct {
    mp_file_t *fp;          		// File pointer for input function
    uint8_t *fbuf;          		// Pointer to the frame buffer for output function
    unsigned int wfbuf;     		// Width of the frame buffer [pix]
	ili9342c_ILI9342C_obj_t *self;	// display object
} IODEV;

//
// User defined input function
//

static unsigned int in_func(    // Returns number of bytes read (zero on error)
    JDEC* jd,                   // Decompression object
    uint8_t* buff,              // Pointer to the read buffer (null to remove data)
    unsigned int nbyte )        // Number of bytes to read/remove
{
    IODEV *dev = (IODEV*)jd->device;   // Device identifier for the session (5th argument of jd_prepare function)
	unsigned int nread;

    if (buff) { // Read data from input stream
        nread = (unsigned int)mp_readinto(dev->fp, buff, nbyte);
		return nread;
    }

    // Remove data from input stream if buff was NULL
    mp_seek(dev->fp, nbyte, SEEK_CUR);
    return 0;
}

//
// User defined output function
//

static int out_fast (       // 1:Ok, 0:Aborted
    JDEC* jd,               // Decompression object
    void* bitmap,           // Bitmap data to be output
    JRECT* rect )           // Rectangular region of output image
{
    IODEV *dev = (IODEV*)jd->device;
    uint8_t *src, *dst;
    uint16_t y, bws, bwd;

    // Copy the decompressed RGB rectanglar to the frame buffer (assuming RGB565)
    src = (uint8_t*)bitmap;
    dst = dev->fbuf + 2 * (rect->top * dev->wfbuf + rect->left);    // Left-top of destination rectangular
    bws = 2 * (rect->right - rect->left + 1);                       // Width of source rectangular [byte]
    bwd = 2 * dev->wfbuf;                                           // Width of frame buffer [byte]
    for (y = rect->top; y <= rect->bottom; y++) {
        memcpy(dst, src, bws);                                      // Copy a line
        src += bws; dst += bwd;                                     // Next line
    }

    return 1;    // Continue to decompress
}

//
// User defined output function
//

static int out_slow (       // 1:Ok, 0:Aborted
    JDEC* jd,               // Decompression object
    void* bitmap,           // Bitmap data to be output
    JRECT* rect )           // Rectangular region of output image
{
	IODEV *dev = (IODEV*)jd->device;
    ili9342c_ILI9342C_obj_t *self = dev->self;

    uint8_t *src, *dst;
    uint16_t y;
	uint16_t wx2 = (rect->right-rect->left+1) *2;
	uint16_t h = rect->bottom-rect->top+1;

    // if the rect is outside of the screen then return
    if ((rect->bottom + jd->y_offs > self->height)  || (rect->right + jd->x_offs > self->width)) {
        return 1;
    }

    // Copy the decompressed RGB rectanglar to the frame buffer (assuming RGB565)
    src = (uint8_t*)bitmap;
    dst = dev->fbuf;    							// Left-top of destination rectangular
	for (y = rect->top; y <= rect->bottom; y++) {
        memcpy(dst, src, wx2);                      // Copy a line
        src += wx2; dst += wx2;                     // Next line
    }

	// blit buffer to display

    set_window(
        self,
        rect->left + jd->x_offs,
        rect->top + jd->y_offs,
        rect->right + jd->x_offs,
        rect->bottom + jd->y_offs);

	DC_HIGH();
	CS_LOW();
	write_spi(self->spi_obj, (uint8_t *) dev->fbuf, wx2 * h);
	CS_HIGH();

    return 1;    // Continue to decompress
}


static mp_obj_t ili9342c_ILI9342C_jpg(size_t n_args, const mp_obj_t *args) {
	ili9342c_ILI9342C_obj_t *self = MP_OBJ_TO_PTR(args[0]);

	const char *filename = mp_obj_str_get_str(args[1]);
	mp_int_t x	  		 = mp_obj_get_int(args[2]);
	mp_int_t y	  		 = mp_obj_get_int(args[3]);

	mp_int_t mode;

	if (n_args > 4)
		mode = mp_obj_get_int(args[4]);
	else
		mode = JPG_MODE_FAST;

	int (*outfunc)(JDEC*,void*,JRECT*);

    JRESULT res;                        // Result code of TJpgDec API
    JDEC jdec;                          // Decompression object
    void *work = malloc(3100);          // Pointer to the work area
    IODEV devid;                        // User defined device identifier
    size_t bufsize;

    self->fp = mp_open(filename, "rb");
	devid.fp = self->fp;
    if (devid.fp) {
		// Prepare to decompress
		res = jd_prepare(&jdec, in_func, work, 3100, &devid);
		if (res == JDR_OK) {
			// Initialize output device
			if (mode == JPG_MODE_FAST) {
				bufsize = 2 * jdec.width * jdec.height;
				outfunc = out_fast;
			} else {
				bufsize = 2 * jdec.msx*8 * jdec.msy*8;
				outfunc = out_slow;
                jdec.x_offs = x;
                jdec.y_offs = y;
			}
			if (self->buffer_size && bufsize > self->buffer_size)
				mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("buffer too small"));

			if (self->buffer_size == 0) {
				self->i2c_buffer = m_malloc(bufsize);
			}

			if (!self->i2c_buffer)
				mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("out of memory"));

			devid.fbuf = (uint8_t *) self->i2c_buffer;
			devid.wfbuf = jdec.width;
			devid.self = self;
			res = jd_decomp(&jdec, outfunc, 0);        // Start to decompress with 1/1 scaling
			if (res == JDR_OK) {
				if (mode == JPG_MODE_FAST) {
					set_window(self, x, y, x + jdec.width - 1, y + jdec.height - 1);
					DC_HIGH();
					CS_LOW();
					write_spi(self->spi_obj, (uint8_t *) self->i2c_buffer, bufsize);
					CS_HIGH();
				}
			} else {
				mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("jpg decompress failed."));
			}
			if (self->buffer_size == 0) {
				m_free(self->i2c_buffer);       // Discard frame buffer
				self->i2c_buffer = MP_OBJ_NULL;
			}
			devid.fbuf = MP_OBJ_NULL;
		} else {
 			mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("jpg prepare failed."));
		}
	    mp_close(devid.fp);
	}
	free(work);                 // Discard work area
	return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ili9342c_ILI9342C_jpg_obj, 4, 5, ili9342c_ILI9342C_jpg);

//
// Dictionary Table
//

static const mp_rom_map_elem_t ili9342c_ILI9342C_locals_dict_table[] = {
	{MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&ili9342c_ILI9342C_write_obj)},
	{MP_ROM_QSTR(MP_QSTR_write_len), MP_ROM_PTR(&ili9342c_ILI9342C_write_len_obj)},
	{MP_ROM_QSTR(MP_QSTR_hard_reset), MP_ROM_PTR(&ili9342c_ILI9342C_hard_reset_obj)},
	{MP_ROM_QSTR(MP_QSTR_soft_reset), MP_ROM_PTR(&ili9342c_ILI9342C_soft_reset_obj)},
	{MP_ROM_QSTR(MP_QSTR_sleep_mode), MP_ROM_PTR(&ili9342c_ILI9342C_sleep_mode_obj)},
	{MP_ROM_QSTR(MP_QSTR_inversion_mode), MP_ROM_PTR(&ili9342c_ILI9342C_inversion_mode_obj)},
	{MP_ROM_QSTR(MP_QSTR_set_window), MP_ROM_PTR(&ili9342c_ILI9342C_set_window_obj)},
	{MP_ROM_QSTR(MP_QSTR_map_bitarray_to_rgb565), MP_ROM_PTR(&ili9342c_map_bitarray_to_rgb565_obj)},
	{MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&ili9342c_ILI9342C_init_obj)},
	{MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&ili9342c_ILI9342C_pixel_obj)},
	{MP_ROM_QSTR(MP_QSTR_line), MP_ROM_PTR(&ili9342c_ILI9342C_line_obj)},
	{MP_ROM_QSTR(MP_QSTR_blit_buffer), MP_ROM_PTR(&ili9342c_ILI9342C_blit_buffer_obj)},
	{MP_ROM_QSTR(MP_QSTR_draw), MP_ROM_PTR(&ili9342c_ILI9342C_draw_obj)},
	{MP_ROM_QSTR(MP_QSTR_bitmap), MP_ROM_PTR(&ili9342c_ILI9342C_bitmap_obj)},
	{MP_ROM_QSTR(MP_QSTR_fill_rect), MP_ROM_PTR(&ili9342c_ILI9342C_fill_rect_obj)},
	{MP_ROM_QSTR(MP_QSTR_fill), MP_ROM_PTR(&ili9342c_ILI9342C_fill_obj)},
	{MP_ROM_QSTR(MP_QSTR_hline), MP_ROM_PTR(&ili9342c_ILI9342C_hline_obj)},
	{MP_ROM_QSTR(MP_QSTR_vline), MP_ROM_PTR(&ili9342c_ILI9342C_vline_obj)},
	{MP_ROM_QSTR(MP_QSTR_rect), MP_ROM_PTR(&ili9342c_ILI9342C_rect_obj)},
	{MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&ili9342c_ILI9342C_text_obj)},
	{MP_ROM_QSTR(MP_QSTR_rotation), MP_ROM_PTR(&ili9342c_ILI9342C_rotation_obj)},
	{MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&ili9342c_ILI9342C_width_obj)},
	{MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&ili9342c_ILI9342C_height_obj)},
	{MP_ROM_QSTR(MP_QSTR_vscrdef), MP_ROM_PTR(&ili9342c_ILI9342C_vscrdef_obj)},
	{MP_ROM_QSTR(MP_QSTR_vscsad), MP_ROM_PTR(&ili9342c_ILI9342C_vscsad_obj)},
	{MP_ROM_QSTR(MP_QSTR_jpg), MP_ROM_PTR(&ili9342c_ILI9342C_jpg_obj)},
};

static MP_DEFINE_CONST_DICT(ili9342c_ILI9342C_locals_dict, ili9342c_ILI9342C_locals_dict_table);
/* methods end */

#ifdef MP_OBJ_TYPE_GET_SLOT

MP_DEFINE_CONST_OBJ_TYPE(
    ili9342c_ILI9342C_type,
    MP_QSTR_ILI9342C,
    MP_TYPE_FLAG_NONE,
    print, ili9342c_ILI9342C_print,
    make_new, ili9342c_ILI9342C_make_new,
    locals_dict, (mp_obj_dict_t *)&ili9342c_ILI9342C_locals_dict);

#else

const mp_obj_type_t ili9342c_ILI9342C_type = {
	{&mp_type_type},
	.name		 = MP_QSTR_ILI9342C,
	.print		 = ili9342c_ILI9342C_print,
	.make_new	 = ili9342c_ILI9342C_make_new,
	.locals_dict = (mp_obj_dict_t *) &ili9342c_ILI9342C_locals_dict,
};

#endif

mp_obj_t ili9342c_ILI9342C_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
	enum {
		ARG_spi,
		ARG_width,
		ARG_height,
		ARG_reset,
		ARG_dc,
		ARG_cs,
		ARG_backlight,
		ARG_rotation,
		ARG_buffer_size,
	};

	static const mp_arg_t allowed_args[] = {
		{MP_QSTR_spi, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_OBJ_NULL}},
		{MP_QSTR_width, MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0}},
		{MP_QSTR_height, MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0}},
		{MP_QSTR_reset, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
		{MP_QSTR_dc, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
		{MP_QSTR_cs, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
		{MP_QSTR_backlight, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
		{MP_QSTR_rotation, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0}},
		{MP_QSTR_buffer_size, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0}},
	};

	mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
	mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

	// create new object
	ili9342c_ILI9342C_obj_t *self = m_new_obj(ili9342c_ILI9342C_obj_t);

	self->base.type = &ili9342c_ILI9342C_type;

	// set parameters
	mp_obj_base_t *spi_obj = (mp_obj_base_t *) MP_OBJ_TO_PTR(args[ARG_spi].u_obj);
	self->spi_obj		   = spi_obj;
	self->display_width	   = args[ARG_width].u_int;
	self->width			   = args[ARG_width].u_int;
	self->display_height   = args[ARG_height].u_int;
	self->height		   = args[ARG_height].u_int;
	self->rotation		   = args[ARG_rotation].u_int % 8;
	self->buffer_size	   = args[ARG_buffer_size].u_int;

	if (self->buffer_size) {
		self->i2c_buffer = m_malloc(self->buffer_size);
	}

	if ((self->display_height != 240 && self->display_width != 320) &&
		(self->display_height != 320 && self->display_width != 240)) {
		mp_raise_ValueError(MP_ERROR_TEXT("Unsupported display. Only 320x240 and 240x320 displays are supported"));
	}

	if (args[ARG_dc].u_obj == MP_OBJ_NULL) {
		mp_raise_ValueError(MP_ERROR_TEXT("must specify dc pin"));
	}

	if (args[ARG_reset].u_obj != MP_OBJ_NULL) {
		self->reset = mp_hal_get_pin_obj(args[ARG_reset].u_obj);
	}

	self->dc = mp_hal_get_pin_obj(args[ARG_dc].u_obj);

	if (args[ARG_cs].u_obj != MP_OBJ_NULL) {
		self->cs = mp_hal_get_pin_obj(args[ARG_cs].u_obj);
	}

	if (args[ARG_backlight].u_obj != MP_OBJ_NULL) {
		self->backlight = mp_hal_get_pin_obj(args[ARG_backlight].u_obj);
	}

	return MP_OBJ_FROM_PTR(self);
}

static const mp_map_elem_t ili9342c_module_globals_table[] = {
	{MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_ili9342c)},
	{MP_ROM_QSTR(MP_QSTR_color565), (mp_obj_t) &ili9342c_color565_obj},
	{MP_ROM_QSTR(MP_QSTR_map_bitarray_to_rgb565), (mp_obj_t) &ili9342c_map_bitarray_to_rgb565_obj},
	{MP_ROM_QSTR(MP_QSTR_ILI9342C), (mp_obj_t) &ili9342c_ILI9342C_type},
	{MP_ROM_QSTR(MP_QSTR_BLACK), MP_ROM_INT(BLACK)},
	{MP_ROM_QSTR(MP_QSTR_BLUE), MP_ROM_INT(BLUE)},
	{MP_ROM_QSTR(MP_QSTR_RED), MP_ROM_INT(RED)},
	{MP_ROM_QSTR(MP_QSTR_GREEN), MP_ROM_INT(GREEN)},
	{MP_ROM_QSTR(MP_QSTR_CYAN), MP_ROM_INT(CYAN)},
	{MP_ROM_QSTR(MP_QSTR_MAGENTA), MP_ROM_INT(MAGENTA)},
	{MP_ROM_QSTR(MP_QSTR_YELLOW), MP_ROM_INT(YELLOW)},
	{MP_ROM_QSTR(MP_QSTR_WHITE), MP_ROM_INT(WHITE)},
	{MP_ROM_QSTR(MP_QSTR_FAST), MP_ROM_INT(JPG_MODE_FAST)},
	{MP_ROM_QSTR(MP_QSTR_SLOW), MP_ROM_INT(JPG_MODE_SLOW)},
};

static MP_DEFINE_CONST_DICT(mp_module_ili9342c_globals, ili9342c_module_globals_table);

const mp_obj_module_t mp_module_ili9342c = {
	.base	 = {&mp_type_module},
	.globals = (mp_obj_dict_t *) &mp_module_ili9342c_globals,
};

#if !defined(MICROPY_VERSION) || MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_ili9342c, mp_module_ili9342c, MODULE_ILI9342C_ENABLE);
#else
MP_REGISTER_MODULE(MP_QSTR_ili9342c, mp_module_ili9342c);
#endif
