#ifndef SCALE_H
#define SCALE_H

// Scales values to 1:1px at 720p
// This is the scale at which the mockups were designed.
#define PIXEL_SCALE(x) ((int)(LV_DPI * x/200))
#define POINTS_SCALE(x) ((int)(LV_DPI * x/200 * 4/3))

#endif
