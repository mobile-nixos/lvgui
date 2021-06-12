/**
 * @file lv_freetype.h
 *
 */
#ifndef _LV_FONT_TTF_H
#define _LV_FONT_TTF_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifndef LV_DRV_NO_CONF
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_drv_conf.h"
#else
#include "../../lv_drv_conf.h"
#endif
#endif

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_CACHE_H

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
 
typedef struct {
    FT_Face     face;      /* handle to face object */
    uint16_t font_size;		/*font height size */
}lv_font_fmt_freetype_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
int lv_freetype_init();
int lv_freetype_font_init(lv_font_t * font, const char * font_path, uint16_t font_size);

/**********************
 *      MACROS
 **********************/
 
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
