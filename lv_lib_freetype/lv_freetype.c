/**
 * @file lv_freetype.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_freetype.h"
#include "../hal.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
 
 /**********************
 *  STATIC VARIABLES
 **********************/
static FT_Library library;

// Fallback font
static uint16_t fallback_last_size = 0;
static FT_Face fallback_face;

static void refresh_fallback_size(uint16_t size)
{
    FT_Error error;

	if (fallback_last_size != size) {
		error = FT_Set_Pixel_Sizes(fallback_face, 0, size);
		if ( error ) {
			printf("Error in FT_Set_Char_Size for fallback font: %d\n", error);
			return;
		}

		fallback_last_size = size;
	}
}

static bool get_glyph_dsc_cb(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next)
{
    if(unicode_letter < 0x20) {
        dsc_out->adv_w = 0;
        dsc_out->box_h = 0;
        dsc_out->box_w = 0;
        dsc_out->ofs_x = 0;
        dsc_out->ofs_y = 0;
        dsc_out->bpp = 0;
        return true;
    }

    int error;
    FT_Face face;
    lv_font_fmt_freetype_dsc_t * dsc = (lv_font_fmt_freetype_dsc_t *)(font->user_data);
    face = dsc->face;

	FT_UInt glyph_index = FT_Get_Char_Index( face, unicode_letter );

	if (glyph_index == 0) {
		refresh_fallback_size(dsc->font_size);

		face = fallback_face;
		glyph_index = FT_Get_Char_Index( face, unicode_letter );

		if (glyph_index == 0) {
			printf("Warning: missing glyph 0x%x in main font and fallback font.\n", unicode_letter);
			return false;
		}
	}

	error = FT_Load_Glyph(
			face,          /* handle to face object */
			glyph_index,   /* glyph index           */
			FT_LOAD_DEFAULT );  /* load flags, see below *///FT_LOAD_MONOCHROME|FT_LOAD_NO_AUTOHINTING
	if ( error )
	{
		printf("Error in FT_Load_Glyph: %d\n", error);
		return error;
	}

	error = FT_Render_Glyph( face->glyph,   /* glyph slot  */
			FT_RENDER_MODE_NORMAL ); /* render mode */ //

	if ( error )
	{
		printf("Error in FT_Render_Glyph: %d\n", error);
		return error;
	}

    dsc_out->adv_w = (face->glyph->metrics.horiAdvance >> 6);
    dsc_out->box_h = face->glyph->bitmap.rows;         /*Height of the bitmap in [px]*/
    dsc_out->box_w = face->glyph->bitmap.width;         /*Width of the bitmap in [px]*/
    dsc_out->ofs_x = face->glyph->bitmap_left;         /*X offset of the bitmap in [pf]*/
    dsc_out->ofs_y = face->glyph->bitmap_top - face->glyph->bitmap.rows;         /*Y offset of the bitmap measured from the as line*/
    dsc_out->bpp = 8;         /*Bit per pixel: 1/2/4/8*/

    return true;
}

/* Get the bitmap of `unicode_letter` from `font`. */
static const uint8_t * get_glyph_bitmap_cb(const lv_font_t * font, uint32_t unicode_letter)
{
	FT_Face face;
	lv_font_fmt_freetype_dsc_t * dsc = (lv_font_fmt_freetype_dsc_t *)(font->user_data);
	face = dsc->face;

	FT_UInt glyph_index = FT_Get_Char_Index( face, unicode_letter );
	if (glyph_index == 0) {
		refresh_fallback_size(dsc->font_size);

		face = fallback_face;
		glyph_index = FT_Get_Char_Index( face, unicode_letter );

		if (glyph_index == 0) {
			printf("Warning: missing glyph 0x%x in main font and fallback font.\n", unicode_letter);
			return false;
		}
	}

	return (const uint8_t *)(face->glyph->bitmap.buffer);
}


/**********************
 *   GLOBAL FUNCTIONS
 **********************/
 
/**
* init freetype library
* @return FT_Error
*/
int lv_freetype_init()
{
    FT_Error error;
    error = FT_Init_FreeType(&library);
    if ( error )
	{
		printf("Error in FT_Init_FreeType: %d\n", error);
		return error;
	}

	error = FT_New_Face(library, hal_asset_path("fonts/fallback.ttf"), 0, &fallback_face);
	if (error) {
		printf("WARNING: Error loading fallback font in FT_New_Face: %d\n", error);
	}

    return FT_Err_Ok;
}

/**
* init lv_font_t struct
* @param font pointer to a font
* @param font_path the font path
* @param font_size the height of font
* @return FT_Error
*/
int lv_freetype_font_init(lv_font_t * font, const char * font_path, uint16_t font_size)
{
    FT_Error error;

    lv_font_fmt_freetype_dsc_t * dsc = lv_mem_alloc(sizeof(lv_font_fmt_freetype_dsc_t));
    LV_ASSERT_MEM(dsc);
    if(dsc == NULL) return FT_Err_Out_Of_Memory;

    dsc->font_size = font_size;

    error = FT_New_Face(library, font_path, 0, &dsc->face);
    if ( error ) {
		printf("Error in FT_New_Face: %d\n", error);
		return error;
	}
    error = FT_Set_Pixel_Sizes(dsc->face, 0,font_size);
    if ( error ) {
		printf("Error in FT_Set_Char_Size: %d\n", error);
		return error;
	}

	font->get_glyph_dsc = get_glyph_dsc_cb;        /*Set a callback to get info about gylphs*/
	font->get_glyph_bitmap = get_glyph_bitmap_cb;  /*Set a callback to get bitmap of a glyp*/

#ifndef LV_USE_USER_DATA
#error "lv_freetype : user_data is required.Enable it lv_conf.h(LV_USE_USER_DATA 1)"
#endif
    font->user_data = dsc;
	font->line_height = (dsc->face->size->metrics.height >> 6);
    font->base_line = -(dsc->face->size->metrics.descender >> 6);  /*Base line measured from the top of line_height*/
	font->subpx = LV_FONT_SUBPX_NONE;
    
    return FT_Err_Ok;
}

