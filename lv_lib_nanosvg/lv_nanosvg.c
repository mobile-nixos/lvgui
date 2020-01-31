#include <stdio.h>
#include "lvgl/lvgl.h"
#include "lv_nanosvg.h"

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

// Struct used to hold data for special operation to apply on the next image load.
typedef struct {
	int width;
	int height;
} lv_nanosvg_next_data_t;

static lv_nanosvg_next_data_t next_data;

void lv_nanosvg_next_reset(void);

static lv_res_t decoder_info(struct _lv_img_decoder * decoder, const void * src, lv_img_header_t * header);
static lv_res_t decoder_open(lv_img_decoder_t * dec, lv_img_decoder_dsc_t * dsc);
static void decoder_close(lv_img_decoder_t * dec, lv_img_decoder_dsc_t * dsc);

void lv_nanosvg_init(void)
{
	lv_nanosvg_next_reset();
	lv_img_decoder_t * dec = lv_img_decoder_create();
	lv_img_decoder_set_info_cb(dec, decoder_info);
	lv_img_decoder_set_open_cb(dec, decoder_open);
	lv_img_decoder_set_close_cb(dec, decoder_close);
}

static lv_res_t decoder_info(struct _lv_img_decoder * decoder, const void * src, lv_img_header_t * header)
{
	(void) decoder;
	lv_img_src_t src_type = lv_img_src_get_type(src);
	int height = 0;
	int width = 0;

	(void) height;
	(void) width;
	(void) src_type;

	if (src_type == LV_IMG_SRC_FILE) {
		const char * filename = src;
		NSVGimage *image = NULL;

		image = nsvgParseFromFile(filename, "px", 96.0f);
		if (image == NULL) {
			fprintf(stderr, "Could not open SVG image for information.\n");
			nsvgDelete(image);

			return LV_RES_INV;
		}

		header->always_zero = 0;
		header->cf = LV_IMG_CF_RAW_ALPHA;

		width = (int)image->width;
		height = (int)image->height;

		if (next_data.width > 0 || next_data.height > 0) {
			if (next_data.width == 0) {
				next_data.width = width * next_data.height / height;
			}
			if (next_data.height == 0) {
				next_data.height = height * next_data.width / width;
			}
			width = next_data.width;
			height = next_data.height;
		}

		header->w = width;
		header->h = height;

		nsvgDelete(image);

		return LV_RES_OK;
	}

	return LV_RES_INV;
}

static lv_res_t decoder_open(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc)
{
	if (dsc->src_type != LV_IMG_SRC_FILE) {
		return LV_RES_INV;
	}

	(void) decoder;

	const char * filename = dsc->src;
	NSVGimage *image = NULL;
	NSVGrasterizer *rast = NULL;

	image = nsvgParseFromFile(filename, "px", 96.0f);
	if (image == NULL) {
		fprintf(stderr, "Could not open SVG image for rasterizing.\n");
		nsvgDelete(image);

		return LV_RES_INV;
	}

	rast = nsvgCreateRasterizer();
	if (rast == NULL) {
		fprintf(stderr, "Could not init rasterizer.\n");
		nsvgDeleteRasterizer(rast);
		nsvgDelete(image);

		return LV_RES_INV;
	}

	dsc->img_data = malloc(dsc->header.w * dsc->header.h * 4);
	nsvgRasterize(rast, image, 0,0, ((float)dsc->header.w/(float)image->width), dsc->img_data, dsc->header.w, dsc->header.h, dsc->header.w*4);

	nsvgDelete(image);

	return LV_RES_OK;
}

static void decoder_close(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc)
{
	(void) decoder;
	free(dsc->img_data);
}

void lv_nanosvg_next_reset()
{
	next_data.width = 0;
	next_data.height = 0;
}

void lv_nanosvg_resize_next_width(int width)
{
	next_data.width = width;
}

void lv_nanosvg_resize_next_height(int height)
{
	next_data.height = height;
}

