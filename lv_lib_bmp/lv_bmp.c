#include <stdio.h>
#include "lvgl/lvgl.h"
#include "lv_bmp.h"

#define LOADBMP_IMPLEMENTATION
#include "loadbmp.h"

static lv_res_t decoder_info(struct _lv_img_decoder * decoder, const void * src, lv_img_header_t * header);
static lv_res_t decoder_open(lv_img_decoder_t * dec, lv_img_decoder_dsc_t * dsc);
static void decoder_close(lv_img_decoder_t * dec, lv_img_decoder_dsc_t * dsc);

void lv_bmp_init(void)
{
	lv_img_decoder_t * dec = lv_img_decoder_create();
	lv_img_decoder_set_info_cb(dec, decoder_info);
	lv_img_decoder_set_open_cb(dec, decoder_open);
	lv_img_decoder_set_close_cb(dec, decoder_close);
}

static lv_res_t decoder_info(struct _lv_img_decoder * decoder, const void * src, lv_img_header_t * header)
{
	(void) decoder;
	lv_img_src_t src_type = lv_img_src_get_type(src);

	const char * filename = src;
	if (src_type != LV_IMG_SRC_FILE || strcmp(&filename[strlen(filename) - 3], "bmp") != 0) {
		return LV_RES_INV;
	}

	unsigned char *pixels = NULL;
	unsigned int err;
	unsigned int width, height;

	err = loadbmp_decode_file(filename, &pixels, &width, &height, LOADBMP_RGBA);
	free(pixels); // meh...

	if (err) {
		fprintf(stderr, "LoadBMP Load Error: (%s) %u\n", filename, err);

		return LV_RES_INV;
	}

	header->always_zero = 0;
	header->cf = LV_IMG_CF_RAW_ALPHA;
	header->w = width;
	header->h = height;

	return LV_RES_OK;
}

static lv_res_t decoder_open(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc)
{
	unsigned int err;
	unsigned int width, height;
	(void) decoder;

	const char * filename = dsc->src;
	if (dsc->src_type != LV_IMG_SRC_FILE || strcmp(&filename[strlen(filename) - 3], "bmp") != 0) {
		return LV_RES_INV;
	}

	err = loadbmp_decode_file(filename, (unsigned char**)&(dsc->img_data), &width, &height, LOADBMP_RGBA);

	if (err) {
		fprintf(stderr, "LoadBMP Load Error: (%s) %u\n", filename, err);
		free(dsc->img_data);

		return LV_RES_INV;
	}

	return LV_RES_OK;
}

static void decoder_close(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc)
{
	(void) decoder;
	(void) dsc;
}
