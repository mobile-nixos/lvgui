#include <stdio.h>
#include "lvgl/lvgl.h"
#include "lv_nanosvg.h"
#include "url.h"

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

static lv_res_t decoder_info(struct _lv_img_decoder * decoder, const void * src, lv_img_header_t * header);
static lv_res_t decoder_open(lv_img_decoder_t * dec, lv_img_decoder_dsc_t * dsc);
static void decoder_close(lv_img_decoder_t * dec, lv_img_decoder_dsc_t * dsc);

void lv_nanosvg_init(void)
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
	int height = 0;
	int width = 0;

	(void) height;
	(void) width;

	if (src_type == LV_IMG_SRC_FILE) {
		const char * path = url_get_path(src);
		NSVGimage *image = NULL;

		image = nsvgParseFromFile(path, "px", 96.0f);
		if (image == NULL) {
			fprintf(stderr, "Could not open SVG image '%s' for information.\n", path);
			nsvgDelete(image);

			free(path);
			return LV_RES_INV;
		}

		header->always_zero = 0;
		header->cf = LV_IMG_CF_RAW_ALPHA;

		// The internal width/height of the SVG
		width = (int)image->width;
		height = (int)image->height;

		// The user might have requested specific dimensions via URL params
		bool gave_width = url_get_int_param((char*)src, "width", &width);
		bool gave_height = url_get_int_param((char*)src, "height", &height);

		// User gave only one part of the component; scale accordingly
		if ((gave_width || gave_height) && !(gave_width && gave_height)) {
			if (gave_height) {
				width = width * height / (int)image->height;
			}
			if (gave_width) {
				height = height * width / (int)image->width;
			}
		}

		header->w = width;
		header->h = height;

		nsvgDelete(image);
		free(path);
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

	const char * filename = url_get_path(dsc->src);
	NSVGimage *image = NULL;
	NSVGrasterizer *rast = NULL;

	image = nsvgParseFromFile(filename, "px", 96.0f);
	if (image == NULL) {
		fprintf(stderr, "Could not open SVG image '%s' for rasterizing.\n", filename);
		nsvgDelete(image);

		free(filename);
		return LV_RES_INV;
	}

	rast = nsvgCreateRasterizer();
	if (rast == NULL) {
		fprintf(stderr, "Could not init rasterizer.\n");
		nsvgDeleteRasterizer(rast);
		nsvgDelete(image);

		free(filename);
		return LV_RES_INV;
	}

	dsc->img_data = lv_mem_alloc(dsc->header.w * dsc->header.h * 4);
	LV_ASSERT_MEM(dsc->img_data);
	if (dsc->img_data == NULL) {
		free(filename);
		return LV_RES_INV;
	}
	nsvgRasterize(rast, image, 0,0, ((float)dsc->header.w/(float)image->width), dsc->img_data, dsc->header.w, dsc->header.h, dsc->header.w*4);

	nsvgDeleteRasterizer(rast);
	nsvgDelete(image);

	free(filename);
	return LV_RES_OK;
}

static void decoder_close(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc)
{
	(void) decoder;
	(void) dsc;
}
