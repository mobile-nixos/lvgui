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
#include "../lv_draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/
#define LV_LABEL_DOT_NUM	3
#define LV_LABEL_DOT_END_INV 0xFFFF
#define LV_LABEL_SCROLL_SPEED (50 * LV_DOWNSCALE) /*Hor, or ver. scroll speed (px/sec) in 'LV_LABEL_LONG_SCROLL' mode*/
#define LV_LABEL_SCROLL_SPEED_VER  (10 * LV_DOWNSCALE) /*Ver. scroll speed if hor. scroll is applied too*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_label_design(lv_obj_t * label, const area_t * mask, lv_design_mode_t mode);
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
    ext->txt = NULL;

	lv_obj_set_design_f(new_label, lv_label_design);
	lv_obj_set_signal_f(new_label, lv_label_signal);

    /*Init the new label*/
    if(copy == NULL) {
    	ext->dot_end = LV_LABEL_DOT_END_INV;
		lv_obj_set_opa(new_label, OPA_COVER);
		lv_obj_set_click(new_label, false);
		lv_obj_set_style(new_label, lv_labels_get(LV_LABELS_DEF, NULL));
		lv_label_set_long_mode(new_label, LV_LABEL_LONG_EXPAND);
		lv_label_set_text(new_label, "Text");
    }
    /*Copy 'copy' if not NULL*/
    else {
		lv_label_set_long_mode(new_label, lv_label_get_long_mode(copy));
		lv_label_set_text(new_label, lv_label_get_text(copy));
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
        lv_label_ext_t * label_p = lv_obj_get_ext(label);
        /*No signal handling*/
    	switch(sign) {
            case LV_SIGNAL_CLEANUP:
                dm_free(label_p->txt);
                label_p->txt = NULL;
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
 * Set a new text for a label
 * @param label pointer to a label object
 * @param text '\0' terminated character string. If NULL then refresh with the current text.
 */
void lv_label_set_text(lv_obj_t * label, const char * text)
{
    lv_obj_inv(label);
    
    lv_label_ext_t * ext = lv_obj_get_ext(label);

    /*If trying to set its own text then use NULL
     * because NULL means text refresh*/
    if(text == ext->txt) text = NULL;

    /*Allocate space for the new text*/
    if(text != NULL) {
    	uint32_t len = strlen(text) + 1;
    	if(ext->txt != NULL) {
			dm_free(ext->txt);
    	}
		ext->txt = dm_alloc(len);
		strcpy(ext->txt, text);
    }
    /*Do not allocate just use the current text*/
    else {
    	text = ext->txt;
    }
    
    /*If 'text" still NULL then nothing to do: return*/
    if(text == NULL) return;
    
    uint32_t line_start = 0;
    uint32_t new_line_start = 0;
    cord_t max_length = lv_obj_get_width(label);
    lv_labels_t * labels = lv_obj_get_style(label);
    const font_t * font = font_get(labels->font);
    uint8_t letter_height = font_get_height(font);
    cord_t new_height = 0;
    cord_t longest_line = 0;
    cord_t act_line_length;
    
    ext->dot_end = LV_LABEL_DOT_END_INV;	/*Initialize the dot end index*/

    /*If the width will be expanded set the max length to very big */
    if(ext->long_mode == LV_LABEL_LONG_EXPAND || ext->long_mode == LV_LABEL_LONG_SCROLL) {
        max_length = LV_CORD_MAX;
    }
    
    /*Calc. the height and longest line*/
    while (text[line_start] != '\0')
    {
        new_line_start += txt_get_next_line(&text[line_start], font, labels->letter_space, max_length);
        new_height += letter_height;
        new_height += labels->line_space;
        
        /*Calculate the the longest line if the width will be expanded*/
        if(ext->long_mode == LV_LABEL_LONG_EXPAND || ext->long_mode == LV_LABEL_LONG_SCROLL) {
          act_line_length = txt_get_width(&text[line_start], new_line_start - line_start,
                                           font, labels->letter_space);
              longest_line = max(act_line_length, longest_line);
        }

        line_start = new_line_start;
    }
    
    /*Correction with the last line space*/
    new_height -= labels->line_space;
    /*Refresh the full size in expand mode*/
    if(ext->long_mode == LV_LABEL_LONG_EXPAND || ext->long_mode == LV_LABEL_LONG_SCROLL) {
    	lv_obj_set_size(label, longest_line, new_height);

    	/*Start scrolling if the label is greater then its parent*/
    	if(ext->long_mode == LV_LABEL_LONG_SCROLL) {
    		/*TODO create the animations*/
    	}
    }
 	/*In break mode only the height can change*/
    else if (ext->long_mode == LV_LABEL_LONG_BREAK) {
        lv_obj_set_height(label, new_height);
    }
    /*Replace the last 'LV_LABEL_DOT_NUM' characters with dots
     * and save these characters*/
    else if(ext->long_mode == LV_LABEL_LONG_DOTS) {
    	point_t point;
    	point.x = lv_obj_get_width(label) - 1;
    	point.y = lv_obj_get_height(label) - 1;
    	uint16_t index = lv_label_get_letter_on(label, &point);
    	printf("index: %d, letter: %c, %d\n", index, ext->txt[index], ext->txt[index]);

    	if(index < strlen(text) - 1) {
    		uint8_t i;
    		for(i = 0; i < LV_LABEL_DOT_NUM; i++) {
        		ext->dot_tmp[i] = ext->txt[index - LV_LABEL_DOT_NUM + i];
        		ext->txt[index - LV_LABEL_DOT_NUM + i] = '.';
    		}
    		/*The last character is '\0'*/
    		ext->dot_tmp[i] = ext->txt[index];
			ext->txt[index] = '\0';

			/*Save the dot end index*/
			ext->dot_end = index;
    	}
    }

    lv_obj_inv(label);
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

    ext->long_mode = long_mode;
    lv_label_set_text(label, NULL);
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
    cord_t max_length = lv_obj_get_width(label);
    lv_labels_t * labels = lv_obj_get_style(label);
    const font_t * font = font_get(labels->font);
    uint8_t letter_height = font_get_height(font);
    cord_t y = 0;

    /*If the width will be expanded  the set the max length to very big */
    if(ext->long_mode == LV_LABEL_LONG_EXPAND || ext->long_mode == LV_LABEL_LONG_SCROLL) {
        max_length = LV_CORD_MAX;
    }

    /*Search the line of the index letter */;
    while (text[new_line_start] != '\0') {
        new_line_start += txt_get_next_line(&text[line_start], font, labels->letter_space, max_length);
        if(index < new_line_start || text[new_line_start] == '\0') break; /*The line of 'index' letter begins at 'line_start'*/

        y += letter_height + labels->line_space;
        line_start = new_line_start;
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
    cord_t max_length = lv_obj_get_width(label);
    lv_labels_t * labels = lv_obj_get_style(label);
    const font_t * font = font_get(labels->font);
    uint8_t letter_height = font_get_height(font);
    cord_t y = 0;

    /*If the width will be expanded set the max length to very big */
    if(ext->long_mode == LV_LABEL_LONG_EXPAND || ext->long_mode == LV_LABEL_LONG_SCROLL) {
        max_length = LV_CORD_MAX;
    }

    /*Search the line of the index letter */;
    while (text[line_start] != '\0') {
    	new_line_start += txt_get_next_line(&text[line_start], font, labels->letter_space, max_length);
    	if(pos->y <= y + letter_height + labels->line_space) break; /*The line is found ('line_start')*/
    	y += letter_height + labels->line_space;
        line_start = new_line_start;
    }

    /*Calculate the x coordinate*/
    cord_t x = 0;
	if(labels->mid != 0) {
		cord_t line_w;
        line_w = txt_get_width(&text[line_start], new_line_start - line_start,
                               font, labels->letter_space);
		x += lv_obj_get_width(label) / 2 - line_w / 2;
    }

	uint16_t i;
	for(i = line_start; i < new_line_start-1; i++) {
		x += font_get_width(font, text[i]) + labels->letter_space;
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
		lv_vfill(&label->cords, mask, COLOR_LIME, OPA_COVER);

		area_t cords;
		lv_obj_get_cords(label, &cords);
		opa_t opa = lv_obj_get_opa(label);
		lv_label_ext_t * ext = lv_obj_get_ext(label);


		lv_draw_label(&cords, mask, lv_obj_get_style(label), opa, ext->txt);


    }
    return true;
}

/**
 * Initialize the label styles
 */
static void lv_labels_init(void)
{
	/*Default style*/
	lv_labels_def.font = LV_FONT_DEFAULT;
	lv_labels_def.objs.color = COLOR_MAKE(0x10, 0x18, 0x20);
	lv_labels_def.letter_space = 2 * LV_STYLE_MULT;
	lv_labels_def.line_space =  2 * LV_STYLE_MULT;
	lv_labels_def.mid =  0;

	memcpy(&lv_labels_btn, &lv_labels_def, sizeof(lv_labels_t));
	lv_labels_btn.objs.color = COLOR_MAKE(0xd0, 0xe0, 0xf0);
	lv_labels_btn.mid =  1;

	memcpy(&lv_labels_title, &lv_labels_def, sizeof(lv_labels_t));
	lv_labels_title.objs.color = COLOR_MAKE(0x10, 0x20, 0x30);
	lv_labels_title.letter_space = 4 * LV_STYLE_MULT;
	lv_labels_title.line_space =  4 * LV_STYLE_MULT;
	lv_labels_title.mid =  0;


	memcpy(&lv_labels_txt, &lv_labels_def, sizeof(lv_labels_t));
	lv_labels_txt.objs.color = COLOR_MAKE(0x16, 0x23, 0x34);
	lv_labels_txt.letter_space = 1 * LV_STYLE_MULT;
	lv_labels_txt.line_space =  2 * LV_STYLE_MULT;
	lv_labels_txt.mid =  0;

}
#endif
