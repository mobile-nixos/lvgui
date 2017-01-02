/**
 * @file lv_rect.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_LABEL != 0

#include "misc/others/color.h"
#include "misc/math/math_base.h"
#include "lv_label.h"
#include "../lv_obj/lv_obj.h"
#include "../lv_misc/text.h"
#include "../lv_misc/anim.h"
#include "../lv_draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/
#define LV_LABEL_DOT_NUM	3
#define LV_LABEL_DOT_END_INV 0xFFFF

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_label_design(lv_obj_t * label, const area_t * mask, lv_design_mode_t mode);
static void lv_label_refr_text(lv_obj_t * label);
static void lv_labels_init(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_labels_t lv_labels_def;
static lv_labels_t lv_labels_btn;
static lv_labels_t lv_labels_title;
static lv_labels_t lv_labels_txt;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a label objects
 * @param par pointer to an object, it will be the parent of the new label
 * @param copy pointer to a button object, if not NULL then the new object will be copied from it
 * @return pointer to the created button
 */
lv_obj_t * lv_label_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create a basic object*/
    lv_obj_t * new_label = lv_obj_create(par, copy);
    dm_assert(new_label);
    
    /*Extend the basic object to a label object*/
    lv_obj_alloc_ext(new_label, sizeof(lv_label_ext_t));
    
    lv_label_ext_t * ext = lv_obj_get_ext(new_label);
    dm_assert(ext);
    ext->txt = NULL;
    ext->static_txt = 0;
    ext->dot_end = LV_LABEL_DOT_END_INV;
    ext->long_mode = LV_LABEL_LONG_EXPAND;

	lv_obj_set_design_f(new_label, lv_label_design);
	lv_obj_set_signal_f(new_label, lv_label_signal);

    /*Init the new label*/
    if(copy == NULL) {
		lv_obj_set_opa(new_label, OPA_COVER);
		lv_obj_set_click(new_label, false);
		lv_obj_set_style(new_label, lv_labels_get(LV_LABELS_DEF, NULL));
		lv_label_set_long_mode(new_label, LV_LABEL_LONG_EXPAND);
		lv_label_set_text(new_label, "Text");
    }
    /*Copy 'copy' if not NULL*/
    else {
        lv_label_ext_t * copy_ext = lv_obj_get_ext(copy);
        lv_label_set_long_mode(new_label, lv_label_get_long_mode(copy));
        if(copy_ext->static_txt == 0) lv_label_set_text(new_label, lv_label_get_text(copy));
        else lv_label_set_text_static(new_label, lv_label_get_text(copy));

        /*Set the style of 'copy' and isolate it if it is necessary*/
        if(lv_obj_get_style_iso(new_label) == false) {
            lv_obj_set_style(new_label, lv_obj_get_style(copy));
        } else {
            lv_obj_set_style(new_label, lv_obj_get_style(copy));
            lv_obj_iso_style(new_label, sizeof(lv_labels_t));
        }
    }
    return new_label;
}


/**
 * Signal function of the label
 * @param label pointer to a label object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_label_signal(lv_obj_t * label, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_obj_signal(label, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
        lv_label_ext_t * ext = lv_obj_get_ext(label);
        /*No signal handling*/
    	switch(sign) {
            case LV_SIGNAL_CLEANUP:
                if(ext->static_txt == 0) {
                    dm_free(ext->txt);
                    ext->txt = NULL;
                }
                break;
            case LV_SIGNAL_STYLE_CHG:
            	lv_label_set_text(label, NULL);
            	break;

			default:
				break;
    	}
    }
    
    return valid;
}

/*=====================
 * Setter functions 
 *====================*/

/**
 * Set a new text for a label. Memory will be allocated to store the text by the label.
 * @param label pointer to a label object
 * @param text '\0' terminated character string. NULL to refresh with the current text.
 */
void lv_label_set_text(lv_obj_t * label, const char * text)
{
    lv_obj_inv(label);
    
    lv_label_ext_t * ext = lv_obj_get_ext(label);

    /*If trying to set its own text or the text is NULL then refresh */
    if(text == ext->txt || text == NULL) {
        lv_label_refr_text(label);
        return;
    }

    /*Allocate space for the new text*/
    uint32_t len = strlen(text) + 1;
    if(ext->txt != NULL && ext->static_txt == 0) {
        dm_free(ext->txt);
    }
    ext->txt = dm_alloc(len);
    strcpy(ext->txt, text);
    ext->static_txt = 0;    /*Now the text is dynamically allocated*/
    
    lv_label_refr_text(label);
}
/**
 * Set a new text for a label from a character array. The array don't has to be '\0' terminated.
 * Memory will be allocated to store the array by the label.
 * @param label pointer to a label object
 * @param array array of characters or NULL to refresh the label
 * @param size the size of 'array' in bytes
 */
void lv_label_set_text_array(lv_obj_t * label, const char * array, uint16_t size)
{
    lv_obj_inv(label);

    lv_label_ext_t * ext = lv_obj_get_ext(label);

    /*If trying to set its own text or the array is NULL then refresh */
    if(array == ext->txt || array == NULL) {
        lv_label_refr_text(label);
        return;
    }

    /*Allocate space for the new text*/
    if(ext->txt != NULL && ext->static_txt == 0) {
        dm_free(ext->txt);
    }
    ext->txt = dm_alloc(size + 1);
    memcpy(ext->txt, array, size);
    ext->txt[size] = '\0';
    ext->static_txt = 0;    /*Now the text is dynamically allocated*/

    lv_label_refr_text(label);
}

/**
 * Set a static text. It will not be saved by the label so the 'text' variable
 * has to be 'alive' while the label exist.
 * @param label pointer to a label object
 * @param text pointer to a text. NULL to refresh with the current text.
 */
void lv_label_set_text_static(lv_obj_t * label, const char * text)
{
    lv_label_ext_t * ext = lv_obj_get_ext(label);
    if(ext->static_txt == 0 && ext->txt != NULL) {
        dm_free(ext->txt);
    }

    if(text != NULL) {
        ext->static_txt = 1;
        ext->txt = (char *) text;
    }

    lv_label_refr_text(label);
}
/**
 * Set the behavior of the label with longer text then the object size
 * @param label pointer to a label object
 * @param long_mode the new mode from 'lv_label_long_mode' enum.
 */
void lv_label_set_long_mode(lv_obj_t * label, lv_label_long_mode_t long_mode)
{
    lv_label_ext_t * ext = lv_obj_get_ext(label);

    /*When changing from dot mode reload the characters replaced by dots*/
    if(ext->long_mode == LV_LABEL_LONG_DOTS &&
       ext->dot_end != LV_LABEL_DOT_END_INV) {
    	uint8_t i;
    	for(i = 0; i < LV_LABEL_DOT_NUM + 1; i++) {
    		ext->txt[ext->dot_end - LV_LABEL_DOT_NUM + i] = ext->dot_tmp[i];
    	}
    }

    /*Delete the scroller animations*/
    if(ext->long_mode == LV_LABEL_LONG_SCROLL) {
    	anim_del(label, (anim_fp_t) lv_obj_set_x);
    	anim_del(label, (anim_fp_t) lv_obj_set_y);
    }

    ext->long_mode = long_mode;
    lv_label_refr_text(label);
}

/*=====================
 * Getter functions 
 *====================*/

/**
 * Get the text of a label
 * @param label pointer to a label object
 * @return the text of the label
 */
const char * lv_label_get_text(lv_obj_t * label)
{
    lv_label_ext_t * ext = lv_obj_get_ext(label);
    
    return ext->txt;
}

/**
 * Get the fix width attribute of a label
 * @param label pointer to a label object
 * @return true: fix width is enabled
 */
lv_label_long_mode_t lv_label_get_long_mode(lv_obj_t * label)
{
    lv_label_ext_t * ext = lv_obj_get_ext(label);
    return ext->long_mode;
}

/**
 * Get the relative x and y coordinates of a letter
 * @param label pointer to a label object
 * @param index index of the letter (0 ... text length)
 * @param pos store the result here (E.g. index = 0 gives 0;0 coordinates)
 */
void lv_label_get_letter_pos(lv_obj_t * label, uint16_t index, point_t * pos)
{
	const char * text = lv_label_get_text(label);
    lv_label_ext_t * ext = lv_obj_get_ext(label);
    uint32_t line_start = 0;
    uint32_t new_line_start = 0;
    cord_t max_w = lv_obj_get_width(label);
    lv_labels_t * labels = lv_obj_get_style(label);
    const font_t * font = font_get(labels->font);
    uint8_t letter_height = font_get_height(font);
    cord_t y = 0;

    /*If the width will be expanded  the set the max length to very big */
    if(ext->long_mode == LV_LABEL_LONG_EXPAND || ext->long_mode == LV_LABEL_LONG_SCROLL) {
        max_w = LV_CORD_MAX;
    }

    /*Search the line of the index letter */;
    while (text[new_line_start] != '\0') {
        new_line_start += txt_get_next_line(&text[line_start], font, labels->letter_space, max_w);
        if(index < new_line_start || text[new_line_start] == '\0') break; /*The line of 'index' letter begins at 'line_start'*/

        y += letter_height + labels->line_space;
        line_start = new_line_start;
    }

    if((text[index - 1] == '\n' || text[index - 1] == '\r') && text[index] == '\0') {
        y += letter_height + labels->line_space;
        line_start = index;
    }

    /*Calculate the x coordinate*/
    cord_t x = 0;
	uint32_t i;
	for(i = line_start; i < index; i++) {
		x += font_get_width(font, text[i]) + labels->letter_space;
	}

	if(labels->mid != 0) {
		cord_t line_w;
        line_w = txt_get_width(&text[line_start], new_line_start - line_start,
                               font, labels->letter_space);
		x += lv_obj_get_width(label) / 2 - line_w / 2;
    }

    pos->x = x;
    pos->y = y;

}

/**
 * Get the index of letter on a relative point of a label
 * @param label pointer to label object
 * @param pos pointer to point with coordinates on a the label
 * @return the index of the letter on the 'pos_p' point (E.g. on 0;0 is the 0. letter)
 */
uint16_t lv_label_get_letter_on(lv_obj_t * label, point_t * pos)
{
	const char * text = lv_label_get_text(label);
    lv_label_ext_t * ext = lv_obj_get_ext(label);
    uint32_t line_start = 0;
    uint32_t new_line_start = 0;
    cord_t max_w = lv_obj_get_width(label);
    lv_labels_t * style = lv_obj_get_style(label);
    const font_t * font = font_get(style->font);
    uint8_t letter_height = font_get_height(font);
    cord_t y = 0;

    /*If the width will be expanded set the max length to very big */
    if(ext->long_mode == LV_LABEL_LONG_EXPAND || ext->long_mode == LV_LABEL_LONG_SCROLL) {
        max_w = LV_CORD_MAX;
    }

    /*Search the line of the index letter */;
    while (text[line_start] != '\0') {
    	new_line_start += txt_get_next_line(&text[line_start], font, style->letter_space, max_w);
    	if(pos->y <= y + letter_height + style->line_space) break; /*The line is found ('line_start')*/
    	y += letter_height + style->line_space;
        line_start = new_line_start;
    }

    /*Calculate the x coordinate*/
    cord_t x = 0;
	if(style->mid != 0) {
		cord_t line_w;
        line_w = txt_get_width(&text[line_start], new_line_start - line_start,
                               font, style->letter_space);
		x += lv_obj_get_width(label) / 2 - line_w / 2;
    }

	uint16_t i;
	for(i = line_start; i < new_line_start-1; i++) {
		x += font_get_width(font, text[i]) + style->letter_space;
		if(pos->x < x) break;
	}


	return i;
}

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_labels_builtin_t enum
 * @param copy copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_labels_t style
 */
lv_labels_t * lv_labels_get(lv_labels_builtin_t style, lv_labels_t * copy)
{
	static bool style_inited = false;

	/*Make the style initialization if it is not done yet*/
	if(style_inited == false) {
		lv_labels_init();
		style_inited = true;
	}

	lv_labels_t * style_p;

	switch(style) {
		case LV_LABELS_DEF:
			style_p = &lv_labels_def;
			break;
		case LV_LABELS_BTN:
			style_p = &lv_labels_btn;
			break;
		case LV_LABELS_TXT:
			style_p = &lv_labels_txt;
			break;
		case LV_LABELS_TITLE:
			style_p = &lv_labels_title;
			break;
		default:
			style_p = &lv_labels_def;
	}

	if(copy != NULL) {
		if(style_p != NULL) memcpy(copy, style_p, sizeof(lv_labels_t));
		else memcpy(copy, &lv_labels_def, sizeof(lv_labels_t));
	}

	return style_p;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the labels
 * @param label pointer to a label object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_label_design(lv_obj_t * label, const area_t * mask, lv_design_mode_t mode)
{
    /* A label never covers an area */
    if(mode == LV_DESIGN_COVER_CHK) return false;
    else if(mode == LV_DESIGN_DRAW_MAIN) {
		/*TEST: draw a background for the label*/
		//lv_vfill(&label->cords, mask, COLOR_LIME, OPA_COVER);

		area_t cords;
		lv_obj_get_cords(label, &cords);
		opa_t opa = lv_obj_get_opa(label);
		lv_label_ext_t * ext = lv_obj_get_ext(label);


		lv_draw_label(&cords, mask, lv_obj_get_style(label), opa, ext->txt);


    }
    return true;
}

/**
 * Refresh the label with its text stored in its extended data
 * @param label pointer to a label object
 */
static void lv_label_refr_text(lv_obj_t * label)
{
    lv_label_ext_t * ext = lv_obj_get_ext(label);

    if(ext->txt == NULL) return;

    cord_t max_w = lv_obj_get_width(label);
    lv_labels_t * style = lv_obj_get_style(label);
    const font_t * font = font_get(style->font);

    ext->dot_end = LV_LABEL_DOT_END_INV;    /*Initialize the dot end index*/

    /*If the width will be expanded set the max length to very big */
    if(ext->long_mode == LV_LABEL_LONG_EXPAND || ext->long_mode == LV_LABEL_LONG_SCROLL) {
        max_w = LV_CORD_MAX;
    }

    /*Calc. the height and longest line*/
    point_t size;
    txt_get_size(&size, ext->txt, font, style->letter_space, style->line_space, max_w);

    /*Refresh the full size in expand mode*/
    if(ext->long_mode == LV_LABEL_LONG_EXPAND || ext->long_mode == LV_LABEL_LONG_SCROLL) {
        lv_obj_set_size(label, size.x, size.y);

        /*Start scrolling if the label is greater then its parent*/
        if(ext->long_mode == LV_LABEL_LONG_SCROLL) {
            lv_obj_t * parent = lv_obj_get_parent(label);

            /*Delete the potential previous scroller animations*/
            anim_del(label, (anim_fp_t) lv_obj_set_x);
            anim_del(label, (anim_fp_t) lv_obj_set_y);

            anim_t anim;
            anim.var = label;
            anim.repeat = 1;
            anim.playback = 1;
            anim.start = font_get_width(font, ' ');
            anim.act_time = 0;
            anim.end_cb = NULL;
            anim.path = anim_get_path(ANIM_PATH_LIN);
            anim.time = 3000;
            anim.playback_pause = LV_LABEL_SCROLL_PLAYBACK_PAUSE;
            anim.repeat_pause = LV_LABEL_SCROLL_REPEAT_PAUSE;

            bool hor_anim = false;
            if(lv_obj_get_width(label) > lv_obj_get_width(parent)) {
                anim.end =  lv_obj_get_width(parent) - lv_obj_get_width(label) - font_get_width(font, ' ');
                anim.fp = (anim_fp_t) lv_obj_set_x;
                anim.time = anim_speed_to_time(LV_LABEL_SCROLL_SPEED, anim.start, anim.end);
                anim_create(&anim);
                hor_anim = true;
            }

            if(lv_obj_get_height(label) > lv_obj_get_height(parent)) {
                anim.end =  lv_obj_get_height(parent) - lv_obj_get_height(label) - font_get_height(font);
                anim.fp = (anim_fp_t)lv_obj_set_y;

                /*Different animation speed if horizontal animation is created too*/
                if(hor_anim == false) {
                    anim.time = anim_speed_to_time(LV_LABEL_SCROLL_SPEED, anim.start, anim.end);
                } else {
                    anim.time = anim_speed_to_time(LV_LABEL_SCROLL_SPEED_VER, anim.start, anim.end);
                }
                anim_create(&anim);
            }
        }
    }
    /*In break mode only the height can change*/
    else if (ext->long_mode == LV_LABEL_LONG_BREAK) {
        lv_obj_set_height(label, size.y);
    }
    /*Replace the last 'LV_LABEL_DOT_NUM' characters with dots
     * and save these characters*/
    else if(ext->long_mode == LV_LABEL_LONG_DOTS) {
        point_t point;
        point.x = lv_obj_get_width(label) - 1;
        point.y = lv_obj_get_height(label) - 1;
        uint16_t index = lv_label_get_letter_on(label, &point);

        if(index < strlen(ext->txt) - 1) {

            /* Change the last 'LV_LABEL_DOT_NUM' to dots
             * (if there are at least 'LV_LABEL_DOT_NUM' characters*/
            if(index > LV_LABEL_DOT_NUM) {
                uint8_t i;
                for(i = 0; i < LV_LABEL_DOT_NUM; i++) {
                    ext->dot_tmp[i] = ext->txt[index - LV_LABEL_DOT_NUM + i];
                    ext->txt[index - LV_LABEL_DOT_NUM + i] = '.';
                }
                /*The last character is '\0'*/
                ext->dot_tmp[i] = ext->txt[index];
                ext->txt[index] = '\0';
            }
            /*Else with short text change all characters to dots*/
            else {
                uint8_t i;
                for(i = 0; i < LV_LABEL_DOT_NUM; i++) {
                    ext->txt[i] = '.';
                }
                ext->txt[i] = '\0';
            }
            /*Save the dot end index*/
            ext->dot_end = index;
        }
    }

    lv_obj_inv(label);
}

/**
 * Initialize the label styles
 */
static void lv_labels_init(void)
{
	/*Default style*/
	lv_labels_def.font = LV_FONT_DEFAULT;
	lv_labels_def.objs.color = COLOR_MAKE(0x10, 0x18, 0x20);
	lv_labels_def.letter_space = 2 * LV_DOWNSCALE;
	lv_labels_def.line_space =  2 * LV_DOWNSCALE;
	lv_labels_def.mid =  0;

	memcpy(&lv_labels_btn, &lv_labels_def, sizeof(lv_labels_t));
	lv_labels_btn.objs.color = COLOR_MAKE(0xd0, 0xe0, 0xf0);
	lv_labels_btn.mid =  1;

	memcpy(&lv_labels_title, &lv_labels_def, sizeof(lv_labels_t));
	lv_labels_title.objs.color = COLOR_MAKE(0x10, 0x20, 0x30);
	lv_labels_title.letter_space = 4 * LV_DOWNSCALE;
	lv_labels_title.line_space =  4 * LV_DOWNSCALE;
	lv_labels_title.mid =  0;

	memcpy(&lv_labels_txt, &lv_labels_def, sizeof(lv_labels_t));
	lv_labels_txt.objs.color = COLOR_MAKE(0x16, 0x23, 0x34);
	lv_labels_txt.letter_space = 0 * LV_DOWNSCALE;
	lv_labels_txt.line_space =  1 * LV_DOWNSCALE;
	lv_labels_txt.mid =  0;

}
#endif
