#include "img_conf.h"
#include "lv_conf.h"

#if USE_IMG_SHUT_DOWN != 0 || LV_APP_USE_INTERNAL_ICONS == 2

#include <stdint.h> 
#include "misc/others/color.h"

const color_int_t img_shut_down [] = { /*Width = 28, Height = 31*/ 
28,	/*Width*/
31,	/*Heigth*/
16,	/*Color depth = 16*/
1,	/*Flags: Transp = 1*/
2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 29614, 29582, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 
2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 27469, 10597, 10565, 23275, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 
2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 19017, 0, 0, 14791, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 
2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 14823, 0, 0, 10565, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 
2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 14823, 0, 0, 10597, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 
2016, 2016, 2016, 2016, 2016, 2016, 21130, 23275, 2016, 2016, 2016, 2016, 16904, 0, 0, 12678, 2016, 2016, 2016, 2016, 25356, 19049, 2016, 2016, 2016, 2016, 2016, 2016, 
2016, 2016, 2016, 2016, 31695, 12710, 2113, 12710, 2016, 2016, 2016, 2016, 16904, 0, 0, 12678, 2016, 2016, 2016, 2016, 16936, 2145, 10565, 27501, 2016, 2016, 2016, 2016, 
2016, 2016, 2016, 2016, 12710, 0, 0, 10597, 2016, 2016, 2016, 2016, 16904, 0, 0, 12678, 2016, 2016, 2016, 2016, 16904, 0, 0, 10565, 2016, 2016, 2016, 2016, 
2016, 2016, 2016, 19017, 4226, 0, 0, 16936, 2016, 2016, 2016, 2016, 16904, 0, 0, 12678, 2016, 2016, 2016, 2016, 23243, 4226, 0, 2113, 12710, 31727, 2016, 2016, 
2016, 2016, 21162, 4226, 0, 2113, 14823, 33808, 2016, 2016, 2016, 2016, 16904, 0, 0, 12678, 2016, 2016, 2016, 2016, 2016, 19049, 4258, 0, 0, 16936, 2016, 2016, 
2016, 2016, 12678, 0, 0, 10597, 2016, 2016, 2016, 2016, 2016, 2016, 16904, 0, 0, 12678, 2016, 2016, 2016, 2016, 2016, 2016, 14823, 0, 0, 10565, 2016, 2016, 
2016, 31695, 6339, 0, 4226, 23275, 2016, 2016, 2016, 2016, 2016, 2016, 14823, 0, 0, 10597, 2016, 2016, 2016, 2016, 2016, 2016, 27501, 8452, 0, 4226, 25356, 2016, 
2016, 19017, 2113, 2113, 19049, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 12710, 0, 0, 8484, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 25356, 4226, 0, 12678, 31695, 
29582, 10565, 0, 6339, 31695, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 23243, 4258, 2145, 19017, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 8452, 0, 2145, 21162, 
25356, 4258, 0, 8484, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 31727, 31695, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 10597, 0, 0, 16904, 
21162, 2113, 0, 10565, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 12710, 0, 0, 12710, 
19017, 0, 0, 10597, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 14791, 0, 0, 12678, 
16936, 0, 0, 10597, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 14791, 0, 0, 12710, 
21130, 32, 0, 10565, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 12678, 0, 0, 14823, 
25388, 6339, 0, 8452, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 8484, 0, 0, 19049, 
31727, 12710, 32, 4226, 25356, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 31695, 6339, 0, 8452, 27469, 
2016, 23275, 4226, 0, 12710, 31727, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 19017, 2113, 2113, 16936, 2016, 
2016, 2016, 6371, 0, 0, 16936, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 21130, 2145, 0, 4258, 29614, 2016, 
2016, 2016, 14823, 0, 0, 6339, 29614, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 8452, 0, 0, 12710, 2016, 2016, 
2016, 2016, 29614, 10597, 0, 0, 6339, 21130, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 23275, 8484, 0, 0, 6371, 25356, 2016, 2016, 
2016, 2016, 2016, 27501, 10565, 0, 0, 2145, 21162, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 25388, 6339, 0, 0, 6371, 23243, 2016, 2016, 2016, 
2016, 2016, 2016, 2016, 25388, 8484, 0, 0, 2145, 8484, 14791, 21130, 27501, 31695, 31695, 27501, 23243, 16904, 10565, 4226, 0, 0, 4258, 21130, 2016, 2016, 2016, 2016, 
2016, 2016, 2016, 2016, 2016, 27469, 12678, 2145, 0, 0, 0, 32, 8452, 12678, 12678, 8484, 2113, 0, 0, 0, 2113, 10565, 23243, 2016, 2016, 2016, 2016, 2016, 
2016, 2016, 2016, 2016, 2016, 2016, 2016, 23243, 6371, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4226, 19049, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 
2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 27469, 16904, 10597, 8452, 6371, 6339, 6339, 6371, 8452, 10565, 14791, 23275, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 
2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 33808, 33808, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 
};

#endif
