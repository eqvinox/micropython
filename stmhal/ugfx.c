/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include <stdio.h>
#include <string.h>
#include <math.h>

#include STM32_HAL_H

#include "py/nlr.h"
#include "py/runtime.h"

#if MICROPY_HW_HAS_UGFX

#include "pin.h"
#include "ugfx.h"
#include "gfx.h"

#include "extmod/vfs_fat_file.h"

#include "genhdr/pins.h"
#include "bufhelper.h"

#include "ugfx_logo_banner.h"

/// \moduleref pyb
/// \class UGFX - uGFX based driver for LCDs
///
/// The UGFX class is used to control a wide range of LCDs, as defined in 
/// the config headers.
///
/// The connections must also be configured in the configuration 
/// header files
///
///     lcd = pyb.UGFX()     # if pyskin is in the Y position
///
/// Then you can use:
///
///     lcd.light(True)                       # turn the backlight on
///     lcd.write('Hello world!\n',10,10)     # print text to the screen
///

font_t default_font;

systemticks_t gfxSystemTicks(void)
{
	return HAL_GetTick();
}
 
systemticks_t gfxMillisecondsToTicks(delaytime_t ms)
{
	return ms;
}

typedef struct _pyb_ugfx_obj_t {
    mp_obj_base_t base;
	
	

    // hardware control for the LCD
	// configured in headers - makes things faster
    //SPI_HandleTypeDef *spi;
    //const pin_obj_t *pin_cs1;
    //const pin_obj_t *pin_rst;
    //const pin_obj_t *pin_a0;
    //const pin_obj_t *pin_bl;

} pyb_ugfx_obj_t;



/// \classmethod \constructor(skin_position)
///
/// Construct an UGFX object.
STATIC mp_obj_t pyb_ugfx_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // get LCD position
    //const char *lcd_id = mp_obj_str_get_str(args[0]);

    // create lcd object
    pyb_ugfx_obj_t *ugfx = m_new_obj(pyb_ugfx_obj_t);
    ugfx->base.type = &pyb_ugfx_type;
	
	default_font = gdispOpenFont("ui2");  //TODO: allow to be changed
	//gdispCloseFont(font);
	
	gfxInit();

	return ugfx;
}
/*
/// \method contrast(value)
///
/// Set the contrast of the LCD.  Valid values are between 0 and 47.
STATIC mp_obj_t pyb_lcd_contrast(mp_obj_t self_in, mp_obj_t contrast_in) {
    pyb_lcd_obj_t *self = self_in;
    int contrast = mp_obj_get_int(contrast_in);
    if (contrast < 0) {
        contrast = 0;
    } else if (contrast > 0x2f) {
        contrast = 0x2f;
    }
    lcd_out(self, LCD_INSTR, 0x81); // electronic volume mode set
    lcd_out(self, LCD_INSTR, contrast); // electronic volume register set
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_lcd_contrast_obj, pyb_lcd_contrast);
*/
/*
/// \method light(value)
///
/// Turn the backlight on/off.  True or 1 turns it on, False or 0 turns it off.
STATIC mp_obj_t pyb_lcd_light(mp_obj_t self_in, mp_obj_t value) {
    pyb_lcd_obj_t *self = self_in;
    if (mp_obj_is_true(value)) {
        self->pin_bl->gpio->BSRRL = self->pin_bl->pin_mask; // set pin high to turn backlight on
    } else {
        self->pin_bl->gpio->BSRRH = self->pin_bl->pin_mask; // set pin low to turn backlight off
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_lcd_light_obj, pyb_lcd_light);
*/
/*
/// \method write(str)
///
/// Write the string `str` to the screen.  It will appear immediately.
STATIC mp_obj_t pyb_lcd_write(mp_obj_t self_in, mp_obj_t str) {
    pyb_lcd_obj_t *self = self_in;
    mp_uint_t len;
    const char *data = mp_obj_str_get_data(str, &len);
    //lcd_write_strn(self, data, len);
	//gwinPrintf(GW1,data);
	font_t font12 = gdispOpenFont("DejaVuSans12");
	gdispDrawString(60, 60, data, font12, HTML2COLOR(0xFFFF00));
	gdispCloseFont(font12);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_lcd_write_obj, pyb_lcd_write);
*/
/*
/// \method fill(colour)
///
/// Fill the screen with the given colour (0 or 1 for white or black).
///
/// This method writes to the hidden buffer.  Use `show()` to show the buffer.
STATIC mp_obj_t pyb_lcd_fill(mp_obj_t self_in, mp_obj_t col_in) {
    pyb_lcd_obj_t *self = self_in;
    int col = mp_obj_get_int(col_in);
    if (col) {
        col = 0xff;
    }
    memset(self->pix_buf, col, LCD_PIX_BUF_BYTE_SIZE);
    memset(self->pix_buf2, col, LCD_PIX_BUF_BYTE_SIZE);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_lcd_fill_obj, pyb_lcd_fill);
*/

/// \method text(str, x, y, colour)
///
/// Draw the given text to the position `(x, y)` using the given colour.
///
STATIC mp_obj_t pyb_ugfx_text(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //pyb_ugfx_obj_t *self = args[0];
    mp_uint_t len;
    const char *data = mp_obj_str_get_data(args[1], &len);
    int x0 = mp_obj_get_int(args[2]);
    int y0 = mp_obj_get_int(args[3]);
    int col = mp_obj_get_int(args[4]);

    //font_t font = gdispOpenFont("ui2");  //TODO: save fotn handle globally or in lcd object
	gdispDrawString(x0, y0, data, default_font, col);	
	//gdispCloseFont(font);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_ugfx_text_obj, 5, 5, pyb_ugfx_text);


/// \method line(x1, y1, x2, y2, colour)
///
/// Draw a line from (x1,y1) to (x2,y2) using the given colour.
///
STATIC mp_obj_t pyb_ugfx_line(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //pyb_ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[1]);
    int y0 = mp_obj_get_int(args[2]);
	int x1 = mp_obj_get_int(args[3]);
    int y1 = mp_obj_get_int(args[4]);
    int col = mp_obj_get_int(args[5]);

	gdispDrawLine(x0, y0, x1, y1, col);	

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_ugfx_line_obj, 6, 6, pyb_ugfx_line);


/// \method thickline(x1, y1, x2, y2, colour, width, round)
///
/// Draw a line with a given thickness from (x1,y1) to (x2,y2) using the given colour.
/// Option to round the ends
///
STATIC mp_obj_t pyb_ugfx_thickline(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //pyb_ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[1]);
    int y0 = mp_obj_get_int(args[2]);
	int x1 = mp_obj_get_int(args[3]);
    int y1 = mp_obj_get_int(args[4]);
    int col = mp_obj_get_int(args[5]);
    int width = mp_obj_get_int(args[6]);
    bool rnd = (mp_obj_get_int(args[7]) != 0);

	gdispDrawThickLine(x0, y0, x1, y1, col, width, rnd);	

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_ugfx_thickline_obj, 8, 8, pyb_ugfx_thickline);


/// \method circle(x1, y1, r, colour)
///
/// Draw a circle having a centre point at (x1,y1), radius r, using the given colour.
/// Option to round the ends
///
STATIC mp_obj_t pyb_ugfx_circle(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //pyb_ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[1]);
    int y0 = mp_obj_get_int(args[2]);
	int r = mp_obj_get_int(args[3]);
    int col = mp_obj_get_int(args[4]);


	gdispDrawCircle(x0, y0, r, col);	

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_ugfx_circle_obj, 5, 5, pyb_ugfx_circle);

/// \method fill_circle(x1, y1, r, colour)
///
/// Fill a circle having a centre point at (x1,y1), radius r, using the given colour.
/// Option to round the ends
///
STATIC mp_obj_t pyb_ugfx_fill_circle(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //pyb_ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[1]);
    int y0 = mp_obj_get_int(args[2]);
	int r = mp_obj_get_int(args[3]);
    int col = mp_obj_get_int(args[4]);


	gdispFillCircle(x0, y0, r, col);	

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_ugfx_fill_circle_obj, 5, 5, pyb_ugfx_fill_circle);



/// \method ellipse(x1, y1, a, b, colour)
///
/// Draw a ellipse having a centre point at (x1,y1), lengths a,b, using the given colour.
/// Option to round the ends
///
STATIC mp_obj_t pyb_ugfx_ellipse(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //pyb_ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[1]);
    int y0 = mp_obj_get_int(args[2]);
	int a = mp_obj_get_int(args[3]);
	int b = mp_obj_get_int(args[4]);
    int col = mp_obj_get_int(args[5]);


	gdispDrawEllipse(x0, y0, a, b, col);	

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_ugfx_ellipse_obj, 6, 6, pyb_ugfx_ellipse);

/// \method fill_ellipse(x1, y1, a, b, colour)
///
/// Fill a ellipse having a centre point at (x1,y1), lengths a,b, using the given colour.
/// Option to round the ends
///
STATIC mp_obj_t pyb_ugfx_fill_ellipse(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //pyb_ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[1]);
    int y0 = mp_obj_get_int(args[2]);
	int a = mp_obj_get_int(args[3]);
	int b = mp_obj_get_int(args[4]);
    int col = mp_obj_get_int(args[5]);


	gdispFillEllipse(x0, y0, a, b, col);	

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_ugfx_fill_ellipse_obj, 6, 6, pyb_ugfx_fill_ellipse);

/// \method area(x1, y1, a, b, colour)
///
/// Fill area from (x,y), with lengths x1,y1, using the given colour.
///
STATIC mp_obj_t pyb_ugfx_area(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //pyb_ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[1]);
    int y0 = mp_obj_get_int(args[2]);
	int a = mp_obj_get_int(args[3]);
	int b = mp_obj_get_int(args[4]);
    int col = mp_obj_get_int(args[5]);


	gdispFillArea(x0, y0, a, b, col);	

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_ugfx_area_obj, 6, 6, pyb_ugfx_area);

/// \method box(x1, y1, a, b, colour)
///
/// Draw a box from (x,y), with lengths x1,y1, using the given colour.
///
STATIC mp_obj_t pyb_ugfx_box(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //pyb_ugfx_obj_t *self = args[0];
    int x0 = mp_obj_get_int(args[1]);
    int y0 = mp_obj_get_int(args[2]);
	int a = mp_obj_get_int(args[3]);
	int b = mp_obj_get_int(args[4]);
    int col = mp_obj_get_int(args[5]);


	gdispDrawBox(x0, y0, a, b, col);	

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_ugfx_box_obj, 6, 6, pyb_ugfx_box);


/// \method ball_demo()
///
/// BALL DEMO!!!
///
STATIC mp_obj_t pyb_ugfx_ball_demo(mp_obj_t self_in) {
    // extract arguments
    //pyb_ugfx_obj_t *self = args[0];
	
	/*
	 * Copyright (c) 2012, 2013, Joel Bodenmann aka Tectu <joel@unormal.org>
	 * Copyright (c) 2012, 2013, Andrew Hannam aka inmarket
	 * Derived from the 2011 IOCCC submission by peter.eastman@gmail.com
	 *
	 */
	 
 #define BALLCOLOR1		Red
#define BALLCOLOR2		Yellow
#define WALLCOLOR		HTML2COLOR(0x303030)
#define BACKCOLOR		HTML2COLOR(0xC0C0C0)
#define FLOORCOLOR		HTML2COLOR(0x606060)
#define SHADOWALPHA		(255-255*0.2)
	
	coord_t		width, height, x, y, radius, ballx, bally, dx, floor;
	coord_t		minx, miny, maxx, maxy;
	coord_t		ballcx, ballcy;
	color_t		colour;
	float		ii, spin, dy, spinspeed, h, f, g;

	width = gdispGetWidth();
	height = gdispGetHeight();

	radius=height/5+height%2+1;	// The ball radius
	ii = 1.0/radius;			// radius as easy math
	floor=height/5-1;			// floor position
	spin=0.0;					// current spin angle on the ball
	spinspeed=0.1;				// current spin speed of the ball
	ballx=width/2;				// ball x position (relative to the ball center)
	bally=height/4;				// ball y position (relative to the ball center)
	dx=.01*width;				// motion in the x axis
	dy=0.0;						// motion in the y axis
	ballcx = 12*radius/5;		// ball x diameter including the shadow
	ballcy = 21*radius/10;		// ball y diameter including the shadow


	minx = miny = 0; maxx = width; maxy = height;		// The clipping window for this frame.

	while(1) {
		// Draw one frame
		gdispStreamStart(minx, miny, maxx-minx, maxy-miny);
		for (y=miny; h = (bally-y)*ii, y<maxy; y++) {
			for (x=minx; x < maxx; x++) {
				g=(ballx-x)*ii;
				f=-.3*g+.954*h;
				if (g*g < 1-h*h) {
					/* The inside of the ball */
					if ((((int)(9-spin+(.954*g+.3*h)*invsqrt(1-f*f))+(int)(2+f*2))&1))
						colour = BALLCOLOR1;
					else
						colour = BALLCOLOR2;
				} else {
					// The background (walls and floor)
					if (y > height-floor) {
						if (x < height-y || height-y > width-x)
							colour = WALLCOLOR;
						else
							colour = FLOORCOLOR;
					} else if (x<floor || x>width-floor)
						colour = WALLCOLOR;
					else
						colour = BACKCOLOR;

					// The ball shadow is darker
					if (g*(g+.4)+h*(h+.1) < 1)
						colour = gdispBlendColor(colour, Black, SHADOWALPHA);
				}
				gdispStreamColor(colour);	/* pixel to the LCD */
			}
		}
		gdispStreamStop();

		// Force a display update if the controller supports it
		gdispFlush();

		// Calculate the new frame size (note this is a drawing optimisation only)
		minx = ballx - radius; miny = bally - radius;
		maxx = minx + ballcx; maxy = miny + ballcy;
		if (dx > 0) maxx += dx; else minx += dx;
		if (dy > 0) maxy += dy; else miny += dy;
		if (minx < 0) minx = 0;
		if (maxx > width) maxx = width;
		if (miny < 0) miny = 0;
		if (maxy > height) maxy = height;

		// Motion
		spin += spinspeed;
		ballx += dx; bally += dy;
		dx = ballx < radius || ballx > width-radius ? spinspeed=-spinspeed,-dx : dx;
		dy = bally > height-1.75*floor ? -.04*height : dy+.002*height;
	}
	
		

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_ugfx_ball_demo_obj, pyb_ugfx_ball_demo);


int fseek ( FILE * stream, long int offset, int origin ) {
  return (0);
}

size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream ) {
	return (0);	
}

size_t fread ( void * ptr, size_t size, size_t count, FILE * stream ) {
	
    mp_obj_t readinto_fn = mp_load_attr(stream, MP_QSTR_read);
    mp_obj_t bytearray = mp_obj_new_bytearray_by_ref(size, ptr);
    mp_obj_t bytes_read = mp_call_function_1(readinto_fn, bytearray);
	
	return mp_obj_get_int(bytes_read);
}

int fclose ( FILE * stream ) {
	return 0;
}

int remove ( const char * filename ){
	return 1; //some error, cant remove
}

FILE * fopen ( const char * filename, const char * mode ) {

    mp_obj_t filename_obj = mp_obj_new_str(filename, strlen(filename), false);
    mp_obj_t mode_obj = mp_obj_new_str(mode, strlen(mode), true);
    mp_obj_t args[2] = { filename_obj, mode_obj };
    mp_obj_t file = fatfs_builtin_open(2, args, (mp_map_t *)&mp_const_empty_map);
	return (FILE * )MP_OBJ_TO_PTR(file);	
}

int fstat(int fildes, struct stat *buf) {
	return 0;
}

int fileno(FILE *stream){
	return 0;
}

int rename(const char *old, const char *new){
	return 0;
}

int access(const char *path, int amode){
	return 0;
}

int stat(const char *restrict path, struct stat *restrict buf) {
	return 0;
}

#define errno (*__errno())
extern int *__errno ( void );
static struct _reent impure_data = { 0, 0};// "", 0, "C", ... };
struct _reent * _impure_ptr = &impure_data;
int * __errno (){
  return &_impure_ptr->_errno;

}



/// \method image_demo()
///
/// IMAGE DEMO!!!
///
STATIC mp_obj_t pyb_ugfx_image_demo(mp_obj_t self_in) {
    // extract arguments
    //pyb_ugfx_obj_t *self = args[0];
	
	gdispImage myImage; 
	
	coord_t	swidth, sheight;
 
 
	// Get the display dimensions
	swidth = gdispGetWidth();
	sheight = gdispGetHeight();
 
	// Set up IO for our image
	gdispImageOpenFile(&myImage, "ugfx_logo_banner.bmp");
	gdispImageDraw(&myImage, 0, 0, swidth, sheight, 0, 0);
	gdispImageClose(&myImage);
 
 

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_ugfx_image_demo_obj, pyb_ugfx_image_demo);



/// \method set_orientation(a)
///
/// Set orientation to 0, 90, 180 or 270 degrees
///
STATIC mp_obj_t pyb_ugfx_set_orientation(mp_obj_t self_in, mp_obj_t value) {
    // extract arguments
    //pyb_ugfx_obj_t *self = args[0];
    int a = mp_obj_get_int(value);

	if (a == 0)
		gdispSetOrientation(GDISP_ROTATE_0);
	else if (a == 90)
		gdispSetOrientation(GDISP_ROTATE_90);
	else if (a == 180)
		gdispSetOrientation(GDISP_ROTATE_180);
	else if (a == 270)
		gdispSetOrientation(GDISP_ROTATE_270);
		

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_ugfx_set_orientation_obj, pyb_ugfx_set_orientation);


/// \method get_width()
///
/// Gets current width of the screen in pixels
///
STATIC mp_obj_t pyb_ugfx_get_width(mp_obj_t self_in) {
    // extract arguments
    //pyb_ugfx_obj_t *self = args[0];
    return mp_obj_new_int(gdispGetWidth());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_ugfx_get_width_obj, pyb_ugfx_get_width);



/// \method get_height()
///
/// Gets current width of the screen in pixels
///
STATIC mp_obj_t pyb_ugfx_get_height(mp_obj_t self_in) {
    // extract arguments
    //pyb_ugfx_obj_t *self = args[0];
    return mp_obj_new_int(gdispGetHeight());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_ugfx_get_height_obj, pyb_ugfx_get_height);


/// \method get_pixel()
///
/// Gets the colour of the given pixel at (x,y)
///
STATIC mp_obj_t pyb_ugfx_get_pixel(mp_obj_t self_in, mp_obj_t x_in, mp_obj_t y_in) {
    // extract arguments
    //pyb_ugfx_obj_t *self = args[0];
	int x = mp_obj_get_int(x_in);
	int y = mp_obj_get_int(y_in);
	return mp_obj_new_int(0);
	//needs sorting, currently returns somewhat dodgy values
    //return mp_obj_new_int(gdispGetPixelColor(x,y)); 
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_ugfx_get_pixel_obj, pyb_ugfx_get_pixel);


STATIC const mp_map_elem_t pyb_ugfx_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_text), (mp_obj_t)&pyb_ugfx_text_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_line), (mp_obj_t)&pyb_ugfx_line_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_box), (mp_obj_t)&pyb_ugfx_box_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_area), (mp_obj_t)&pyb_ugfx_area_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_thickline), (mp_obj_t)&pyb_ugfx_thickline_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_circle), (mp_obj_t)&pyb_ugfx_circle_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_fill_circle), (mp_obj_t)&pyb_ugfx_fill_circle_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ellipse), (mp_obj_t)&pyb_ugfx_ellipse_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_fill_ellipse), (mp_obj_t)&pyb_ugfx_fill_ellipse_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_orientation), (mp_obj_t)&pyb_ugfx_set_orientation_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_width), (mp_obj_t)&pyb_ugfx_get_width_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_height), (mp_obj_t)&pyb_ugfx_get_height_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ball_demo), (mp_obj_t)&pyb_ugfx_ball_demo_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_image_demo), (mp_obj_t)&pyb_ugfx_image_demo_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_pixel), (mp_obj_t)&pyb_ugfx_get_pixel_obj },
	
	//class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_RED),        MP_OBJ_NEW_SMALL_INT(Red) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BLUE),       MP_OBJ_NEW_SMALL_INT(Blue) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GREEN),      MP_OBJ_NEW_SMALL_INT(Green) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BLACK),      MP_OBJ_NEW_SMALL_INT(Black) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WHITE),      MP_OBJ_NEW_SMALL_INT(White) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_YELLOW),     MP_OBJ_NEW_SMALL_INT(Yellow) },

};

STATIC MP_DEFINE_CONST_DICT(pyb_ugfx_locals_dict, pyb_ugfx_locals_dict_table);

const mp_obj_type_t pyb_ugfx_type = {
    { &mp_type_type },
    .name = MP_QSTR_UGFX,
    .make_new = pyb_ugfx_make_new,
    .locals_dict = (mp_obj_t)&pyb_ugfx_locals_dict,
};

#endif // MICROPY_HW_HAS_UGFX
