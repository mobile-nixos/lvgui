/*
 * Original author:
 * © 2020 Neil Armstrong <narmstrong@baylibre.com>
 *
 * Continued development:
 * © 2021 Samuel Dionne-Riel <samuel@dionne-riel.com>
 *
 * Re-based fresh on top of:
 * https://github.com/dvdhrm/docs/blob/master/drm-howto/
 * (Public Domain)
 */

/* Implementation notes:
 *
 *  - This does not cleanup the driver memory.
 *    The operating system can do this for us.
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "drm.h"
#if USE_DRM

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include <xf86drm.h>
#include <xf86drmMode.h>

#include "lv_drivers/display/fbdev.h"

// #define DRV_DEBUG

#define err(msg, ...)   fprintf(stderr, "[display/drm_drv]: error: " msg "\n", ##__VA_ARGS__);
#define print(msg, ...)	fprintf(stdout, "[display/drm_drv]: " msg, ##__VA_ARGS__);
#define info(msg, ...)    print(msg "\n", ##__VA_ARGS__)
#ifdef DRV_DEBUG
#define dbg(msg, ...)     print("(debug) " msg "\n", ##__VA_ARGS__)
#else
#define dbg(msg, ...) {}
#endif

struct modeset_dev;
static int modeset_find_crtc(int fd, drmModeRes *res, drmModeConnector *conn, struct modeset_dev *dev);
static int modeset_create_fb(int fd, struct modeset_dev *dev);
static int modeset_setup_dev(int fd, drmModeRes *res, drmModeConnector *conn, struct modeset_dev *dev);
static int modeset_open(int *out, const char *node);
static int modeset_prepare(int fd);

static void dbg_fill_buffer(struct modeset_dev *iter, uint8_t r, uint8_t g, uint8_t b);

struct modeset_dev {
	struct modeset_dev *next;

	uint32_t width;
	uint32_t height;
	uint32_t stride;
	uint32_t size;
	uint32_t handle;
	uint8_t *map;

	drmModeModeInfo mode;
	uint32_t fb;
	uint32_t conn;
	uint32_t crtc;
	drmModeCrtc *saved_crtc;

	int fd;
};

static struct modeset_dev *modeset_list = NULL;

/****************************************
 *        PUBLIC IMPLEMENTATION         *
 ***************************************/

void drm_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
	// Pick the first device
	struct modeset_dev* dev = modeset_list;

	if (modeset_list->fd < 0) {
		err("drm_flush called when DRM device is not initialized properly.");

		return;
	}

	lv_coord_t w = (area->x2 - area->x1 + 1);
	lv_coord_t h = (area->y2 - area->y1 + 1);
	int i, y;

	dbg("drm_flush() x %d:%d y %d:%d w %d h %d", area->x1, area->x2, area->y1, area->y2, w, h);

	// TODO: for a partial update, copy current framebuffer into back buffer before doing the partial update.
	// Partial update, so we need to update the back buffer with the front buffer content first.
	// if ((w != modeset_list->width || h != modeset_list->height) && modeset_list->cur_bufs[1])
	// 	memcpy(dev->map, modeset_list->cur_bufs[1]->map, dev->size);
	(int) h; // temporary until ^ is added back...

	// Just in case, this is most likely a BUG in this driver.
	if (area->y2 > modeset_list->height) {
		err("drm_flush() too large to fit in buffer!!!! [BUG!!]");
		return;
	}

	for (y = 0, i = area->y1 ; i <= area->y2 ; ++i, ++y) {
		memcpy(
			(uint8_t *)dev->map + (area->x1 * (LV_COLOR_SIZE/8)) + (dev->stride * i),
			(uint8_t *)color_p + (w * (LV_COLOR_SIZE/8) * y),
			w * (LV_COLOR_SIZE/8)
		);
	}

	lv_disp_flush_ready(disp_drv);
}

void drm_init(lv_disp_drv_t* drv)
{
	int fd = 0;
	int ret = 0;
	const char *card = DRM_CARD;
	struct modeset_dev *dev;

	info("Starting DRM subsystem... (%s)", card);

	/* open the DRM device */
	ret = modeset_open(&fd, card);
	if (ret) goto handle_errno_error;

	/* open the DRM device */
	ret = modeset_prepare(fd);
	if (ret) goto handle_errno_error;

	/* perform actual modesetting on each found connector+CRTC */
	// Is what would happen, but here we're only handling the first usable one.
	//for (dev = modeset_list; dev; dev = dev->next) {
	dev = modeset_list;
	dev->saved_crtc = drmModeGetCrtc(fd, dev->crtc);
	ret = drmModeSetCrtc(fd, dev->crtc, dev->fb, 0, 0,
			&dev->conn, 1, &dev->mode);
	if (ret) {
		err("cannot set CRTC for connector %u (%d): %m", dev->conn, errno);
		goto err;
	}
	//}

#ifdef DRV_DEBUG
	dbg_fill_buffer(dev, 0x00, 0xFF, 0x00);
#endif

	drv->hor_res = modeset_list->width;
	drv->ver_res = modeset_list->height;

	goto ok;
	goto err;

handle_errno_error:
	err("modeset failed with error %d: %m\n", errno);

err:
	close(modeset_list->fd);
	modeset_list->fd = -1;

	err("(falling back to fbdev...)");
	fbdev_init(drv);
	drv->flush_cb = fbdev_flush;

ok:
	info("DRM subsystem and buffer mapped successfully");
}

void drm_exit(void)
{
	close(modeset_list->fd);
	modeset_list->fd = -1;
}


/****************************************
 *          STATIC FUNCTIONS            *
 ***************************************/

// {{{ Generic modeset handling

static int modeset_open(int *out, const char *node)
{
	int fd, ret;
	uint64_t has_dumb;

	fd = open(node, O_RDWR | O_CLOEXEC);
	if (fd < 0) {
		ret = -errno;
		err("cannot open '%s': %m", node);
		return ret;
	}

	if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 ||
	    !has_dumb) {
			err("drm device '%s' does not support dumb buffers", node);
		close(fd);
		return -EOPNOTSUPP;
	}

	*out = fd;
	return 0;
}

static int modeset_prepare(int fd)
{
	drmModeRes *res;
	drmModeConnector *conn;
	unsigned int i;
	struct modeset_dev *dev;
	int ret;

	/* retrieve resources */
	res = drmModeGetResources(fd);
	if (!res) {
		err("cannot retrieve DRM resources (%d): %m", errno);
		return -errno;
	}

	/* iterate all connectors */
	for (i = 0; i < res->count_connectors; ++i) {
		/* get information for each connector */
		conn = drmModeGetConnector(fd, res->connectors[i]);
		if (!conn) {
			err("cannot retrieve DRM connector %u:%u (%d): %m", i, res->connectors[i], errno);
			continue;
		}

		/* create a device structure */
		dev = malloc(sizeof(*dev));
		memset(dev, 0, sizeof(*dev));
		dev->fd = fd;
		dev->conn = conn->connector_id;

		/* call helper function to prepare this connector */
		ret = modeset_setup_dev(fd, res, conn, dev);
		if (ret) {
			if (ret != -ENOENT) {
				errno = -ret;
				err("cannot setup device for connector %u:%u (%d): %m", i, res->connectors[i], errno);
			}
			free(dev);
			drmModeFreeConnector(conn);
			continue;
		}

		/* free connector data and link device into global list */
		drmModeFreeConnector(conn);
		dev->next = modeset_list;
		modeset_list = dev;
	}

	/* free resources again */
	drmModeFreeResources(res);
	return 0;
}

static int modeset_setup_dev(int fd, drmModeRes *res, drmModeConnector *conn,
			     struct modeset_dev *dev)
{
	int ret;

	/* check if a monitor is connected */
	if (conn->connection != DRM_MODE_CONNECTED) {
		err("ignoring unused connector %u", conn->connector_id);
		return -ENOENT;
	}

	/* check if there is at least one valid mode */
	if (conn->count_modes == 0) {
		err("no valid mode for connector %u", conn->connector_id);
		return -EFAULT;
	}

	/* copy the mode information into our device structure */
	memcpy(&dev->mode, &conn->modes[0], sizeof(dev->mode));
	dev->width = conn->modes[0].hdisplay;
	dev->height = conn->modes[0].vdisplay;
	info("mode for connector %u is %ux%u", conn->connector_id, dev->width, dev->height);

	/* find a crtc for this connector */
	ret = modeset_find_crtc(fd, res, conn, dev);
	if (ret) {
		err("no valid crtc for connector %u", conn->connector_id);
		return ret;
	}

	/* create a framebuffer for this CRTC */
	ret = modeset_create_fb(fd, dev);
	if (ret) {
		err("cannot create framebuffer for connector %u", conn->connector_id);
		return ret;
	}

	return 0;
}

static int modeset_find_crtc(int fd, drmModeRes *res, drmModeConnector *conn,
			     struct modeset_dev *dev)
{
	drmModeEncoder *enc;
	unsigned int i, j;
	int32_t crtc;
	struct modeset_dev *iter;

	/* first try the currently conected encoder+crtc */
	if (conn->encoder_id)
		enc = drmModeGetEncoder(fd, conn->encoder_id);
	else
		enc = NULL;

	if (enc) {
		if (enc->crtc_id) {
			crtc = enc->crtc_id;
			for (iter = modeset_list; iter; iter = iter->next) {
				if (iter->crtc == crtc) {
					crtc = -1;
					break;
				}
			}

			if (crtc >= 0) {
				drmModeFreeEncoder(enc);
				dev->crtc = crtc;
				return 0;
			}
		}

		drmModeFreeEncoder(enc);
	}

	/* If the connector is not currently bound to an encoder or if the
	 * encoder+crtc is already used by another connector (actually unlikely
	 * but lets be safe), iterate all other available encoders to find a
	 * matching CRTC. */
	for (i = 0; i < conn->count_encoders; ++i) {
		enc = drmModeGetEncoder(fd, conn->encoders[i]);
		if (!enc) {
			err("cannot retrieve encoder %u:%u (%d): %m", i, conn->encoders[i], errno);
			continue;
		}

		/* iterate all global CRTCs */
		for (j = 0; j < res->count_crtcs; ++j) {
			/* check whether this CRTC works with the encoder */
			if (!(enc->possible_crtcs & (1 << j)))
				continue;

			/* check that no other device already uses this CRTC */
			crtc = res->crtcs[j];
			for (iter = modeset_list; iter; iter = iter->next) {
				if (iter->crtc == crtc) {
					crtc = -1;
					break;
				}
			}

			/* we have found a CRTC, so save it and return */
			if (crtc >= 0) {
				drmModeFreeEncoder(enc);
				dev->crtc = crtc;
				return 0;
			}
		}

		drmModeFreeEncoder(enc);
	}

	err("cannot find suitable CRTC for connector %u", conn->connector_id);
	return -ENOENT;
}

static int modeset_create_fb(int fd, struct modeset_dev *dev)
{
	struct drm_mode_create_dumb creq;
	struct drm_mode_destroy_dumb dreq;
	struct drm_mode_map_dumb mreq;
	int ret;

	/* create dumb buffer */
	memset(&creq, 0, sizeof(creq));
	creq.width = dev->width;
	creq.height = dev->height;
	creq.bpp = 32;
	ret = drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
	if (ret < 0) {
		err("cannot create dumb buffer (%d): %m", errno);
		return -errno;
	}
	dev->stride = creq.pitch;
	dev->size = creq.size;
	dev->handle = creq.handle;

	/* create framebuffer object for the dumb-buffer */
	ret = drmModeAddFB(fd, dev->width, dev->height, 24, 32, dev->stride,
			   dev->handle, &dev->fb);
	if (ret) {
		err("cannot create framebuffer (%d): %m", errno);
		ret = -errno;
		goto err_destroy;
	}

	/* prepare buffer for memory mapping */
	memset(&mreq, 0, sizeof(mreq));
	mreq.handle = dev->handle;
	ret = drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
	if (ret) {
		err("cannot map dumb buffer (%d): %m", errno);
		ret = -errno;
		goto err_fb;
	}

	/* perform actual memory mapping */
	dev->map = mmap(0, dev->size, PROT_READ | PROT_WRITE, MAP_SHARED,
		        fd, mreq.offset);
	if (dev->map == MAP_FAILED) {
		err("cannot mmap dumb buffer (%d): %m", errno);
		ret = -errno;
		goto err_fb;
	}

	/* clear the framebuffer to 0 */
	memset(dev->map, 0, dev->size);

	return 0;

err_fb:
	drmModeRmFB(fd, dev->fb);
err_destroy:
	memset(&dreq, 0, sizeof(dreq));
	dreq.handle = dev->handle;
	drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
	return ret;
}

// }}}

static void dbg_fill_buffer(struct modeset_dev *dev, uint8_t r, uint8_t g, uint8_t b)
{
	int j, k;
	int off;

	dbg("Filling framebuffer...");
	for (j = 0; j < dev->height; ++j) {
		for (k = 0; k < dev->width; ++k) {
			off = dev->stride * j + k * 4;
			*(uint32_t*)&dev->map[off] = (r << 16) | (g << 8) | b;
		}
	}
}

#endif
