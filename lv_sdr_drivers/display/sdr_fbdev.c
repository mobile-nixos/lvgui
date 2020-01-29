#include "sdr_fbdev.h"
#if USE_FBDEV

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/fb.h>

static struct fb_var_screeninfo vinfo;
static int fbfd = 0;

void fbdev_set_resolution(lv_disp_drv_t* disp_drv)
{
    // Open the file for reading and writing
    fbfd = open(FBDEV_PATH, O_RDWR);
    if(fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        return;
    }

    // Get variable screen information
    if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        return;
    }

	disp_drv->hor_res = vinfo.xres;
	disp_drv->ver_res = vinfo.yres;

    close(fbfd);
}

#endif
