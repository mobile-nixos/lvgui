/**
 * @file lv_style.h
 *
 */

#ifndef LV_STYLE_H
#define LV_STYLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>
#include "../lv_font/lv_font.h"
#include "../lv_misc/lv_color.h"
#include "../lv_misc/lv_area.h"
#include "../lv_misc/lv_anim.h"

/*********************
 *      DEFINES
 *********************/
#define LV_RADIUS_CIRCLE (LV_COORD_MAX) /**< A very big radius to always draw as circle*/
#define LV_STYLE_DEGUG_SENTINEL_VALUE 0x12345678

LV_EXPORT_CONST_INT(LV_RADIUS_CIRCLE);

/**********************
 *      TYPEDEFS
 **********************/

/*Border types (Use 'OR'ed values)*/
enum {
    LV_BORDER_NONE     = 0x00,
    LV_BORDER_BOTTOM   = 0x01,
    LV_BORDER_TOP      = 0x02,
    LV_BORDER_LEFT     = 0x04,
    LV_BORDER_RIGHT    = 0x08,
    LV_BORDER_FULL     = 0x0F,
    LV_BORDER_INTERNAL = 0x10, /**< FOR matrix-like objects (e.g. Button matrix)*/
};
typedef uint8_t lv_border_part_t;

/*Shadow types*/
enum {
    LV_SHADOW_BOTTOM = 0, /**< Only draw bottom shadow */
    LV_SHADOW_FULL,       /**< Draw shadow on all sides */
};
typedef uint8_t lv_shadow_type_t;

/**
 * Objects in LittlevGL can be assigned a style - which holds information about
 * how the object should be drawn.
 * 
 * This allows for easy customization without having to modify the object's design
 * function.
 */
typedef struct
{
    uint8_t glass : 1; /**< 1: Do not inherit this style*/

    /** Object background. */
    struct
    {
        lv_color_t main_color; /**< Object's main background color. */
        lv_color_t grad_color; /**< Second color. If not equal to `main_color` a gradient will be drawn for the background. */
        lv_coord_t radius; /**< Object's corner radius. You can use #LV_RADIUS_CIRCLE if you want to draw a circle. */
        lv_opa_t opa; /**< Object's opacity (0-255). */

        struct
        {
            lv_color_t color; /**< Border color */
            lv_coord_t width; /**< Border width */
            lv_border_part_t part; /**< Which borders to draw */
            lv_opa_t opa; /**< Border opacity. */
        } border;

        
        struct
        {
            lv_color_t color;
            lv_coord_t width;
            lv_shadow_type_t type; /**< Which parts of the shadow to draw */
        } shadow;

        struct
        {
            lv_coord_t top;
            lv_coord_t bottom;
            lv_coord_t left;
            lv_coord_t right;
            lv_coord_t inner;
        } padding;
    } body;

    /** Style for text drawn by this object. */
    struct
    {
        lv_color_t color; /**< Text color */
        lv_color_t sel_color; /**< Text selection background color. */
        const lv_font_t * font;
        lv_coord_t letter_space; /**< Space between letters */
        lv_coord_t line_space; /**< Space between lines (vertical) */
        lv_opa_t opa; /**< Text opacity */
    } text;

    /**< Style of images. */
    struct
    {
        lv_color_t color; /**< Color to recolor the image with */
        lv_opa_t intense; /**< Opacity of recoloring (0 means no recoloring) */
        lv_opa_t opa; /**< Opacity of whole image */
    } image;

    /**< Style of lines (not borders). */
    struct
    {
        lv_color_t color;
        lv_coord_t width;
        lv_opa_t opa;
        uint8_t rounded : 1; /**< 1: rounded line endings*/
    } line;

#if LV_USE_DEBUG
#if LV_USE_ASSERT_STYLE
    uint32_t debug_sentinel; /**<Should `LV_STYLE_DEGUG_SENTINEL_VALUE` to indicate that the style is valid*/
#endif
#endif

} lv_style_t;

#if LV_USE_ANIMATION
/** Data structure for style animations. */
typedef struct
{
    lv_style_t style_start; /*Save not only pointers because can be same as 'style_anim' then it
                               will be modified too*/
    lv_style_t style_end;
    lv_style_t * style_anim;
    lv_anim_ready_cb_t ready_cb;
} lv_style_anim_dsc_t;
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 *  Init the basic styles
 */
void lv_style_init(void);

/**
 * Copy a style to an other
 * @param dest pointer to the destination style
 * @param src pointer to the source style
 */
void lv_style_copy(lv_style_t * dest, const lv_style_t * src);

/**
 * Mix two styles according to a given ratio
 * @param start start style
 * @param end end style
 * @param res store the result style here
 * @param ratio the ratio of mix [0..256]; 0: `start` style; 256: `end` style
 */
void lv_style_mix(const lv_style_t * start, const lv_style_t * end, lv_style_t * res, uint16_t ratio);

#if LV_USE_ANIMATION

/**
 * Initialize an animation variable.
 * E.g.:
 * lv_anim_t a;
 * lv_style_anim__init(&a);
 * lv_style_anim_set_...(&a);
 * lv_style_anim_create(&a);
 * @param a pointer to an `lv_anim_t` variable to initialize
 */
void lv_style_anim_init(lv_anim_t * a);

/**
 *
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param to_anim pointer to the style to animate
 * @param start pointer to a style to animate from (start value)
 * @param end pointer to a style to animate to (end value)
 */
void lv_style_anim_set_styles(lv_anim_t * a, lv_style_t * to_anim, const lv_style_t * start, const lv_style_t * end);

/**
 * Set the duration and delay of an animation
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param duration duration of the animation in milliseconds
 * @param delay delay before the animation in milliseconds
 */
void lv_style_anim_set_time(lv_anim_t * a, uint16_t duration, int16_t delay);

/**
 * Set a function call when the animation is ready
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param ready_cb a function call when the animation is ready
 */
void lv_style_anim_set_ready_cb(lv_anim_t * a, lv_anim_ready_cb_t ready_cb);

/**
 * Make the animation to play back to when the forward direction is ready
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param wait_time time in milliseconds to wait before starting the back direction
 */
void lv_style_anim_set_playback(lv_anim_t * a, uint16_t wait_time);

/**
 * Disable playback. (Disabled after `lv_anim_init()`)
 * @param a pointer to an initialized `lv_anim_t` variable
 */
void lv_style_anim_clear_playback(lv_anim_t * a);

/**
 * Make the animation to start again when ready.
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param wait_time time in milliseconds to wait before starting the animation again
 */
void lv_style_anim_set_repeat(lv_anim_t * a, uint16_t wait_time);

/**
 * Disable repeat. (Disabled after `lv_anim_init()`)
 * @param a pointer to an initialized `lv_anim_t` variable
 */
void lv_style_anim_clear_repeat(lv_anim_t * a);

/**
 * Create an animation
 * @param a an initialized 'anim_t' variable. Not required after call.
 */
void lv_style_anim_create(lv_anim_t * a);

#endif

/*************************
 *    GLOBAL VARIABLES
 *************************/
extern lv_style_t lv_style_scr;
extern lv_style_t lv_style_transp;
extern lv_style_t lv_style_transp_fit;
extern lv_style_t lv_style_transp_tight;
extern lv_style_t lv_style_plain;
extern lv_style_t lv_style_plain_color;
extern lv_style_t lv_style_pretty;
extern lv_style_t lv_style_pretty_color;
extern lv_style_t lv_style_btn_rel;
extern lv_style_t lv_style_btn_pr;
extern lv_style_t lv_style_btn_tgl_rel;
extern lv_style_t lv_style_btn_tgl_pr;
extern lv_style_t lv_style_btn_ina;

/**********************
 *      MACROS
 **********************/

/**
 * Create and initialize a `static` style
 * Example:
 *     LV_STYLE_CREATE(my_style, &lv_style_plain);
 *   is equivalent to
 *     static lv_style_t my_style;
 *     lv_style_copy(my_style, &lv_style_plain);
 *
 * If the style to copy is `NULL` `lv_style_plain` will be used.
 */
#define LV_STYLE_CREATE(name, copy_p) static lv_style_t name; lv_style_copy(&name, copy_p == NULL ? &lv_style_plain : copy_p);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_STYLE_H*/