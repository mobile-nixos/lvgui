/**
 * @file lv_color.h
 *
 */

#ifndef LV_COLOR_H
#define LV_COLOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif

/*Error checking*/
#if LV_COLOR_DEPTH == 24
#error "LV_COLOR_DEPTH  24 is deprecated. Use LV_COLOR_DEPTH  32 instead (lv_conf.h)"
#endif

#if LV_COLOR_DEPTH != 32 && LV_COLOR_SCREEN_TRANSP != 0
#error "LV_COLOR_SCREEN_TRANSP requires LV_COLOR_DEPTH == 32. Set it in lv_conf.h"
#endif

#if LV_COLOR_DEPTH != 16 && LV_COLOR_16_SWAP != 0
#error "LV_COLOR_16_SWAP requires LV_COLOR_DEPTH == 16. Set it in lv_conf.h"
#endif

#include <stdint.h>

/*********************
 *      DEFINES
 *********************/
#define LV_COLOR_WHITE LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)
#define LV_COLOR_SILVER LV_COLOR_MAKE(0xC0, 0xC0, 0xC0)
#define LV_COLOR_GRAY LV_COLOR_MAKE(0x80, 0x80, 0x80)
#define LV_COLOR_BLACK LV_COLOR_MAKE(0x00, 0x00, 0x00)
#define LV_COLOR_RED LV_COLOR_MAKE(0xFF, 0x00, 0x00)
#define LV_COLOR_MAROON LV_COLOR_MAKE(0x80, 0x00, 0x00)
#define LV_COLOR_YELLOW LV_COLOR_MAKE(0xFF, 0xFF, 0x00)
#define LV_COLOR_OLIVE LV_COLOR_MAKE(0x80, 0x80, 0x00)
#define LV_COLOR_LIME LV_COLOR_MAKE(0x00, 0xFF, 0x00)
#define LV_COLOR_GREEN LV_COLOR_MAKE(0x00, 0x80, 0x00)
#define LV_COLOR_CYAN LV_COLOR_MAKE(0x00, 0xFF, 0xFF)
#define LV_COLOR_AQUA LV_COLOR_CYAN
#define LV_COLOR_TEAL LV_COLOR_MAKE(0x00, 0x80, 0x80)
#define LV_COLOR_BLUE LV_COLOR_MAKE(0x00, 0x00, 0xFF)
#define LV_COLOR_NAVY LV_COLOR_MAKE(0x00, 0x00, 0x80)
#define LV_COLOR_MAGENTA LV_COLOR_MAKE(0xFF, 0x00, 0xFF)
#define LV_COLOR_PURPLE LV_COLOR_MAKE(0x80, 0x00, 0x80)
#define LV_COLOR_ORANGE LV_COLOR_MAKE(0xFF, 0xA5, 0x00)

/**
 * Opacity percentages.
 */
enum {
    LV_OPA_TRANSP = 0,
    LV_OPA_0      = 0,
    LV_OPA_10     = 25,
    LV_OPA_20     = 51,
    LV_OPA_30     = 76,
    LV_OPA_40     = 102,
    LV_OPA_50     = 127,
    LV_OPA_60     = 153,
    LV_OPA_70     = 178,
    LV_OPA_80     = 204,
    LV_OPA_90     = 229,
    LV_OPA_100    = 255,
    LV_OPA_COVER  = 255,
};

#define LV_OPA_MIN 16  /*Opacities below this will be transparent*/
#define LV_OPA_MAX 251 /*Opacities above this will fully cover*/

#if LV_COLOR_DEPTH == 1
#define LV_COLOR_SIZE 8
#elif LV_COLOR_DEPTH == 8
#define LV_COLOR_SIZE 8
#elif LV_COLOR_DEPTH == 16
#define LV_COLOR_SIZE 16
#elif LV_COLOR_DEPTH == 32
#define LV_COLOR_SIZE 32
#else
#error "Invalid LV_COLOR_DEPTH in lv_conf.h! Set it to 1, 8, 16 or 32!"
#endif

/*---------------------------------------
 * Macros for all existing  color depths
 * to set/get values of the color channels
 *------------------------------------------*/
# define LV_COLOR_SET_R1(c, v) (c).ch.red = (uint8_t)((v) & 0x1);
# define LV_COLOR_SET_G1(c, v) (c).ch.green = (uint8_t)((v) & 0x1);
# define LV_COLOR_SET_B1(c, v) (c).ch.blue = (uint8_t)((v) & 0x1);
# define LV_COLOR_SET_A1(c, v)

# define LV_COLOR_GET_R1(c) (c).ch.red
# define LV_COLOR_GET_G1(c) (c).ch.green
# define LV_COLOR_GET_B1(c) (c).ch.blue
# define LV_COLOR_GET_A1(c) 1

# define LV_COLOR_SET_R8(c, v) (c).ch.red = (uint8_t)(v) & 0x7U;
# define LV_COLOR_SET_G8(c, v) (c).ch.green = (uint8_t)(v) & 0x7U;
# define LV_COLOR_SET_B8(c, v) (c).ch.blue = (uint8_t)(v) & 0x3U;
# define LV_COLOR_SET_A8(c, v) do {} while(0)

# define LV_COLOR_GET_R8(c) (c).ch.red
# define LV_COLOR_GET_G8(c) (c).ch.green
# define LV_COLOR_GET_B8(c) (c).ch.blue
# define LV_COLOR_GET_A8(c) 0xFF

# define LV_COLOR_SET_R16(c, v) (c).ch.red = (uint8_t)(v) & 0x1FU;
# define LV_COLOR_SET_G16(c, v) (c).ch.green = (uint8_t)(v) & 0x3FU;
# define LV_COLOR_SET_G16_SWAP(c, v) {(c).ch.green_h = (uint8_t)(((v) >> 3) & 0x7); (c).ch.green_l = (uint8_t)((v) & 0x7);}
# define LV_COLOR_SET_B16(c, v) (c).ch.blue = (uint8_t)(v) & 0x1FU;
# define LV_COLOR_SET_A16(c, v) do {} while(0)

# define LV_COLOR_GET_R16(c) (c).ch.red
# define LV_COLOR_GET_G16(c) (c).ch.green
# define LV_COLOR_GET_G16_SWAP(c) (((c).ch.green_h << 3) + (c).ch.green_l)
# define LV_COLOR_GET_B16(c) (c).ch.blue
# define LV_COLOR_GET_A16(c) 0xFF

# define LV_COLOR_SET_R32(c, v) (c).ch.red = (uint32_t)((v) & 0xFF);
# define LV_COLOR_SET_G32(c, v) (c).ch.green = (uint32_t)((v) & 0xFF);
# define LV_COLOR_SET_B32(c, v) (c).ch.blue = (uint32_t)((v) & 0xFF);
# define LV_COLOR_SET_A32(c, v) (c).ch.alpha = (uint32_t)((v) & 0xFF);

# define LV_COLOR_GET_R32(c) (c).ch.red
# define LV_COLOR_GET_G32(c) (c).ch.green
# define LV_COLOR_GET_B32(c) (c).ch.blue
# define LV_COLOR_GET_A32(c) (c).ch.alpha


/*---------------------------------------
 * Macros for the current color depth
 * to set/get values of the color channels
 *------------------------------------------*/
#if LV_COLOR_DEPTH == 1
# define LV_COLOR_SET_R(c, v) LV_COLOR_SET_R1(c,v)
# define LV_COLOR_SET_G(c, v) LV_COLOR_SET_G1(c,v)
# define LV_COLOR_SET_B(c, v) LV_COLOR_SET_B1(c,v)
# define LV_COLOR_SET_A(c, v) LV_COLOR_SET_A1(c,v)

# define LV_COLOR_GET_R(c) LV_COLOR_GET_R1(c)
# define LV_COLOR_GET_G(c) LV_COLOR_GET_G1(c)
# define LV_COLOR_GET_B(c) LV_COLOR_GET_B1(c)
# define LV_COLOR_GET_A(c) LV_COLOR_GET_A1(c)

#elif LV_COLOR_DEPTH == 8
# define LV_COLOR_SET_R(c, v) LV_COLOR_SET_R8(c,v)
# define LV_COLOR_SET_G(c, v) LV_COLOR_SET_G8(c,v)
# define LV_COLOR_SET_B(c, v) LV_COLOR_SET_B8(c,v)
# define LV_COLOR_SET_A(c, v) LV_COLOR_SET_A8(c,v)

# define LV_COLOR_GET_R(c) LV_COLOR_GET_R8(c)
# define LV_COLOR_GET_G(c) LV_COLOR_GET_G8(c)
# define LV_COLOR_GET_B(c) LV_COLOR_GET_B8(c)
# define LV_COLOR_GET_A(c) LV_COLOR_GET_A8(c)

#elif LV_COLOR_DEPTH == 16
# define LV_COLOR_SET_R(c, v) LV_COLOR_SET_R16(c,v)
# if LV_COLOR_16_SWAP == 0
#   define LV_COLOR_SET_G(c, v) LV_COLOR_SET_G16(c,v)
# else
#   define LV_COLOR_SET_G(c, v) LV_COLOR_SET_G16_SWAP(c,v)
# endif
# define LV_COLOR_SET_B(c, v) LV_COLOR_SET_B16(c,v)
# define LV_COLOR_SET_A(c, v) LV_COLOR_SET_A16(c,v)

# define LV_COLOR_GET_R(c) LV_COLOR_GET_R16(c)
# if LV_COLOR_16_SWAP == 0
#   define LV_COLOR_GET_G(c) LV_COLOR_GET_G16(c)
# else
#   define LV_COLOR_GET_G(c) LV_COLOR_GET_G16_SWAP(c)
# endif
# define LV_COLOR_GET_B(c)   LV_COLOR_GET_B16(c)
# define LV_COLOR_GET_A(c)   LV_COLOR_GET_A16(c)

#elif LV_COLOR_DEPTH == 32
# define LV_COLOR_SET_R(c, v) LV_COLOR_SET_R32(c,v)
# define LV_COLOR_SET_G(c, v) LV_COLOR_SET_G32(c,v)
# define LV_COLOR_SET_B(c, v) LV_COLOR_SET_B32(c,v)
# define LV_COLOR_SET_A(c, v) LV_COLOR_SET_A32(c,v)

# define LV_COLOR_GET_R(c) LV_COLOR_GET_R32(c)
# define LV_COLOR_GET_G(c) LV_COLOR_GET_G32(c)
# define LV_COLOR_GET_B(c) LV_COLOR_GET_B32(c)
# define LV_COLOR_GET_A(c) LV_COLOR_GET_A32(c)
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef union
{
    struct
    {
        uint8_t blue : 1;
        uint8_t green : 1;
        uint8_t red : 1;
    } ch;
    uint8_t full;
} lv_color1_t;

typedef union
{
    struct
    {
        uint8_t blue : 2;
        uint8_t green : 3;
        uint8_t red : 3;
    } ch;
    uint8_t full;
} lv_color8_t;

typedef union
{
    struct
    {
#if LV_COLOR_16_SWAP == 0
        uint16_t blue : 5;
        uint16_t green : 6;
        uint16_t red : 5;
#else
        uint16_t green_h : 3;
        uint16_t red : 5;
        uint16_t blue : 5;
        uint16_t green_l : 3;
#endif
    } ch;
    uint16_t full;
} lv_color16_t;

typedef union
{
    struct
    {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t alpha;
    } ch;
    uint32_t full;
} lv_color32_t;

#if LV_COLOR_DEPTH == 1
typedef uint8_t lv_color_int_t;
typedef lv_color1_t lv_color_t;
#elif LV_COLOR_DEPTH == 8
typedef uint8_t lv_color_int_t;
typedef lv_color8_t lv_color_t;
#elif LV_COLOR_DEPTH == 16
typedef uint16_t lv_color_int_t;
typedef lv_color16_t lv_color_t;
#elif LV_COLOR_DEPTH == 32
typedef uint32_t lv_color_int_t;
typedef lv_color32_t lv_color_t;
#else
#error "Invalid LV_COLOR_DEPTH in lv_conf.h! Set it to 1, 8, 16 or 32!"
#endif

typedef uint8_t lv_opa_t;

typedef struct
{
    uint16_t h;
    uint8_t s;
    uint8_t v;
} lv_color_hsv_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*In color conversations:
 * - When converting to bigger color type the LSB weight of 1 LSB is calculated
 *   E.g. 16 bit Red has 5 bits
 *         8 bit Red has 2 bits
 *        ----------------------
 *        8 bit red LSB = (2^5 - 1) / (2^2 - 1) = 31 / 3 = 10
 *
 * - When calculating to smaller color type simply shift out the LSBs
 *   E.g.  8 bit Red has 2 bits
 *        16 bit Red has 5 bits
 *        ----------------------
 *         Shift right with 5 - 3 = 2
 */

uint8_t lv_color_to1(lv_color_t color);

uint8_t lv_color_to8(lv_color_t color);

uint16_t lv_color_to16(lv_color_t color);

uint32_t lv_color_to32(lv_color_t color);

lv_color_t lv_color_mix(lv_color_t c1, lv_color_t c2, uint8_t mix);

/**
 * Get the brightness of a color
 * @param color a color
 * @return the brightness [0..255]
 */
uint8_t lv_color_brightness(lv_color_t color);

/* The most simple macro to create a color from R,G and B values */
#if LV_COLOR_DEPTH == 1
#define LV_COLOR_MAKE(r8, g8, b8) ((lv_color_t){.full = (uint8_t)((b8 >> 7) | (g8 >> 7) | (r8 >> 7))})
#elif LV_COLOR_DEPTH == 8
#define LV_COLOR_MAKE(r8, g8, b8) ((lv_color_t){{(uint8_t)((b8 >> 6) & 0x3U), (uint8_t)((g8 >> 5) & 0x7U), (uint8_t)((r8 >> 5) & 0x7U)}})
#elif LV_COLOR_DEPTH == 16
#if LV_COLOR_16_SWAP == 0
#define LV_COLOR_MAKE(r8, g8, b8) ((lv_color_t){{(uint16_t)((b8 >> 3) & 0x1FU), (uint16_t)((g8 >> 2) & 0x3FU), (uint16_t)((r8 >> 3) & 0x1FU)}})
#else
#define LV_COLOR_MAKE(r8, g8, b8) ((lv_color_t){{(uint16_t)((g8 >> 5) & 0x7U), (uint16_t)((r8 >> 3) & 0x1FU), (uint16_t)((b8 >> 3) & 0x1FU), (uint16_t)((g8 >> 2) & 0x7U)}})
#endif
#elif LV_COLOR_DEPTH == 32
#define LV_COLOR_MAKE(r8, g8, b8) ((lv_color_t){{b8, g8, r8, 0xff}}) /*Fix 0xff alpha*/
#endif

lv_color_t lv_color_make(uint8_t r, uint8_t g, uint8_t b);

lv_color_t lv_color_hex(uint32_t c);

lv_color_t lv_color_hex3(uint32_t c);

/**
 * Convert a HSV color to RGB
 * @param h hue [0..359]
 * @param s saturation [0..100]
 * @param v value [0..100]
 * @return the given RGB color in RGB (with LV_COLOR_DEPTH depth)
 */
lv_color_t lv_color_hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v);

/**
 * Convert a 32-bit RGB color to HSV
 * @param r8 8-bit red
 * @param g8 8-bit green
 * @param b8 8-bit blue
 * @return the given RGB color in HSV
 */
lv_color_hsv_t lv_color_rgb_to_hsv(uint8_t r8, uint8_t g8, uint8_t b8);

/**
 * Convert a color to HSV
 * @param color color
 * @return the given color in HSV
 */
lv_color_hsv_t lv_color_to_hsv(lv_color_t color);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*USE_COLOR*/
