#include "lvgl/lvgl.h"
#include "lv_lib_freetype/lv_freetype.h"
#include "font.h"
#include "scale.h"

lv_font_t * lvgui_get_font(char* path, uint16_t size)
{
	int ret = 0;
	lv_font_t * font = lv_mem_alloc(sizeof(lv_font_t));
	LV_ASSERT_MEM(font);

	if (font == NULL) {
		return LV_FONT_DEFAULT;
	}

	ret = lv_freetype_font_init(font, path, POINTS_SCALE(size));
	if (ret == FT_Err_Ok) {
		return font;
	}

	return LV_FONT_DEFAULT;
}
