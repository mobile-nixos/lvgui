
/**
 * @file lv_kb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_kb.h"
#if LV_USE_KB != 0

#include "../lv_core/lv_debug.h"
#include "../lv_themes/lv_theme.h"
#include "lv_ta.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_kb"

#define LV_KB_CTRL_BTN_FLAGS (LV_BTNM_CTRL_NO_REPEAT | LV_BTNM_CTRL_CLICK_TRIG)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_kb_signal(lv_obj_t * kb, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;
static bool lv_kb_capslock = false;
/* clang-format off */

#define LOWER_ROW_KEYS "[dummy]", " ", LV_SYMBOL_LEFT, LV_SYMBOL_RIGHT, ""
#define LOWER_ROW_CTRL LV_BTNM_CTRL_HIDDEN | 2, 7, 1, 1

// TODO: icons for caps and shift

static const char * kb_map_lc[] = {
	"`", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", LV_SYMBOL_BACKSPACE, "\n",
	"TAB", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\\", "\n",
	"CAPS", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "Enter", "\n",
	"SHIFT", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "SHIFT", "\n",
	LOWER_ROW_KEYS
};

static const lv_btnm_ctrl_t kb_ctrl_lc_map[] = {
	/* 012335 */ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7,
	/* qwerty */ LV_BTNM_CTRL_HIDDEN | 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 5,
	/* asdfgh */ LV_KB_CTRL_BTN_FLAGS | 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7,
	/* zxcvbn */ LV_KB_CTRL_BTN_FLAGS | 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, LV_KB_CTRL_BTN_FLAGS | 3,
	LOWER_ROW_CTRL
};

static const char * kb_map_uc[] = {
	"~", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+", LV_SYMBOL_BACKSPACE, "\n",
	"TAB", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "|", "\n",
	"caps", "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "\"", "Enter", "\n",
	"shift", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "shift", "\n",
	LOWER_ROW_KEYS
};

static const lv_btnm_ctrl_t kb_ctrl_uc_map[] = {
	/* 012335 */ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7,
	/* qwerty */ LV_BTNM_CTRL_HIDDEN | 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 5,
	/* asdfgh */ LV_KB_CTRL_BTN_FLAGS | 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7,
	/* zxcvbn */ LV_KB_CTRL_BTN_FLAGS | 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, LV_KB_CTRL_BTN_FLAGS | 3,
	LOWER_ROW_CTRL
};

// Special characters keyboard (unused for the time being) [1#]
// FIXME: instead, 3rd layer (Alt Gr) support?

static const char * kb_map_spec[] = {
	"0", "1", "2", "3", "4" ,"5", "6", "7", "8", "9", LV_SYMBOL_BACKSPACE, "\n",
	"abc", "+", "-", "/", "*", "=", "%", "!", "?", "#", "<", ">", "\n",
	"\\",  "@", "$", "(", ")", "{", "}", "[", "]", ";", "\"", "'", "\n",
	LV_SYMBOL_CLOSE, LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""
};

static const lv_btnm_ctrl_t kb_ctrl_spec_map[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, LV_KB_CTRL_BTN_FLAGS | 2,
	LV_KB_CTRL_BTN_FLAGS | 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	LV_KB_CTRL_BTN_FLAGS | 2, 2, 6, 2, LV_KB_CTRL_BTN_FLAGS | 2
};


// Numeric-only keypad

static const char * kb_map_num[] = {
	"1", "2", "3", LV_SYMBOL_CLOSE, "\n",
	"4", "5", "6", LV_SYMBOL_OK, "\n",
	"7", "8", "9", LV_SYMBOL_BACKSPACE, "\n",
	"+/-", "0", ".", LV_SYMBOL_LEFT, LV_SYMBOL_RIGHT, ""
};

static const lv_btnm_ctrl_t kb_ctrl_num_map[] = {
	1, 1, 1, LV_KB_CTRL_BTN_FLAGS | 2,
	1, 1, 1, LV_KB_CTRL_BTN_FLAGS | 2,
	1, 1, 1, 2,
	1, 1, 1, 1, 1
};
/* clang-format on */

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a keyboard objects
 * @param par pointer to an object, it will be the parent of the new keyboard
 * @param copy pointer to a keyboard object, if not NULL then the new object will be copied from it
 * @return pointer to the created keyboard
 */
lv_obj_t * lv_kb_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("keyboard create started");

    /*Create the ancestor of keyboard*/
    lv_obj_t * new_kb = lv_btnm_create(par, copy);
    LV_ASSERT_MEM(new_kb);
    if(new_kb == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_kb);

    /*Allocate the keyboard type specific extended data*/
    lv_kb_ext_t * ext = lv_obj_allocate_ext_attr(new_kb, sizeof(lv_kb_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) return NULL;

    /*Initialize the allocated 'ext' */

    ext->ta         = NULL;
    ext->mode       = LV_KB_MODE_TEXT;
    ext->cursor_mng = 0;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(new_kb, lv_kb_signal);

    /*Init the new keyboard keyboard*/
    if(copy == NULL) {
        /* Set a size which fits into the parent.
         * Don't use `par` directly because if the window is created on a page it is moved to the
         * scrollable so the parent has changed */
        lv_obj_set_size(new_kb, lv_obj_get_width_fit(lv_obj_get_parent(new_kb)),
                        lv_obj_get_height_fit(lv_obj_get_parent(new_kb)) / 2);

        lv_obj_align(new_kb, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
        lv_obj_set_event_cb(new_kb, lv_kb_def_event_cb);
        lv_btnm_set_map(new_kb, kb_map_lc);
        lv_btnm_set_ctrl_map(new_kb, kb_ctrl_lc_map);
        lv_obj_set_base_dir(new_kb, LV_BIDI_DIR_LTR);

        /*Set the default styles*/
        lv_theme_t * th = lv_theme_get_current();
        if(th) {
            lv_kb_set_style(new_kb, LV_KB_STYLE_BG, th->style.kb.bg);
            lv_kb_set_style(new_kb, LV_KB_STYLE_BTN_REL, th->style.kb.btn.rel);
            lv_kb_set_style(new_kb, LV_KB_STYLE_BTN_PR, th->style.kb.btn.pr);
            lv_kb_set_style(new_kb, LV_KB_STYLE_BTN_TGL_REL, th->style.kb.btn.tgl_rel);
            lv_kb_set_style(new_kb, LV_KB_STYLE_BTN_TGL_PR, th->style.kb.btn.tgl_pr);
            lv_kb_set_style(new_kb, LV_KB_STYLE_BTN_INA, th->style.kb.btn.ina);
        } else {
            /*Let the button matrix's styles*/
        }
    }
    /*Copy an existing keyboard*/
    else {
        lv_kb_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->ta                = NULL;
        ext->ta                = copy_ext->ta;
        ext->mode              = copy_ext->mode;
        ext->cursor_mng        = copy_ext->cursor_mng;

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_kb);
    }

    LV_LOG_INFO("keyboard created");

    return new_kb;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Assign a Text Area to the Keyboard. The pressed characters will be put there.
 * @param kb pointer to a Keyboard object
 * @param ta pointer to a Text Area object to write there
 */
void lv_kb_set_ta(lv_obj_t * kb, lv_obj_t * ta)
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);
    if(ta) LV_ASSERT_OBJ(ta, "lv_ta");

    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    lv_cursor_type_t cur_type;

    /*Hide the cursor of the old Text area if cursor management is enabled*/
    if(ext->ta && ext->cursor_mng) {
        cur_type = lv_ta_get_cursor_type(ext->ta);
        lv_ta_set_cursor_type(ext->ta, cur_type | LV_CURSOR_HIDDEN);
    }

    ext->ta = ta;

    /*Show the cursor of the new Text area if cursor management is enabled*/
    if(ext->ta && ext->cursor_mng) {
        cur_type = lv_ta_get_cursor_type(ext->ta);
        lv_ta_set_cursor_type(ext->ta, cur_type & (~LV_CURSOR_HIDDEN));
    }
}

/**
 * Set a new a mode (text or number map)
 * @param kb pointer to a Keyboard object
 * @param mode the mode from 'lv_kb_mode_t'
 */
void lv_kb_set_mode(lv_obj_t * kb, lv_kb_mode_t mode)
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);

    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    if(ext->mode == mode) return;

    ext->mode = mode;
    if(mode == LV_KB_MODE_TEXT) {
        lv_btnm_set_map(kb, kb_map_lc);
        lv_btnm_set_ctrl_map(kb, kb_ctrl_lc_map);
    } else if(mode == LV_KB_MODE_NUM) {
        lv_btnm_set_map(kb, kb_map_num);
        lv_btnm_set_ctrl_map(kb, kb_ctrl_num_map);
    } else if(mode == LV_KB_MODE_TEXT_UPPER) {
        lv_btnm_set_map(kb, kb_map_uc);
        lv_btnm_set_ctrl_map(kb, kb_ctrl_uc_map);
    }
}

/**
 * Automatically hide or show the cursor of Text Area
 * @param kb pointer to a Keyboard object
 * @param en true: show cursor on the current text area, false: hide cursor
 */
void lv_kb_set_cursor_manage(lv_obj_t * kb, bool en)
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);

    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    if(ext->cursor_mng == en) return;

    ext->cursor_mng = en == false ? 0 : 1;

    if(ext->ta) {
        lv_cursor_type_t cur_type;
        cur_type = lv_ta_get_cursor_type(ext->ta);

        if(ext->cursor_mng) {
            lv_ta_set_cursor_type(ext->ta, cur_type & (~LV_CURSOR_HIDDEN));
        } else {
            lv_ta_set_cursor_type(ext->ta, cur_type | LV_CURSOR_HIDDEN);
        }
    }
}

/**
 * Set a new map for the keyboard
 * @param kb pointer to a Keyboard object
 * @param map pointer to a string array to describe the map.
 *            See 'lv_btnm_set_map()' for more info.
 */
void lv_kb_set_map(lv_obj_t * kb, const char * map[])
{
    lv_btnm_set_map(kb, map);
}

/**
 * Set the button control map (hidden, disabled etc.) for the keyboard. The
 * control map array will be copied and so may be deallocated after this
 * function returns.
 * @param kb pointer to a keyboard object
 * @param ctrl_map pointer to an array of `lv_btn_ctrl_t` control bytes.
 *                 See: `lv_btnm_set_ctrl_map` for more details.
 */
void lv_kb_set_ctrl_map(lv_obj_t * kb, const lv_btnm_ctrl_t ctrl_map[])
{
    lv_btnm_set_ctrl_map(kb, ctrl_map);
}

/**
 * Set a style of a keyboard
 * @param kb pointer to a keyboard object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_kb_set_style(lv_obj_t * kb, lv_kb_style_t type, const lv_style_t * style)
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);

    switch(type) {
        case LV_KB_STYLE_BG: lv_btnm_set_style(kb, LV_BTNM_STYLE_BG, style); break;
        case LV_KB_STYLE_BTN_REL: lv_btnm_set_style(kb, LV_BTNM_STYLE_BTN_REL, style); break;
        case LV_KB_STYLE_BTN_PR: lv_btnm_set_style(kb, LV_BTNM_STYLE_BTN_PR, style); break;
        case LV_KB_STYLE_BTN_TGL_REL: lv_btnm_set_style(kb, LV_BTNM_STYLE_BTN_TGL_REL, style); break;
        case LV_KB_STYLE_BTN_TGL_PR: lv_btnm_set_style(kb, LV_BTNM_STYLE_BTN_TGL_PR, style); break;
        case LV_KB_STYLE_BTN_INA: lv_btnm_set_style(kb, LV_BTNM_STYLE_BTN_INA, style); break;
    }
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Assign a Text Area to the Keyboard. The pressed characters will be put there.
 * @param kb pointer to a Keyboard object
 * @return pointer to the assigned Text Area object
 */
lv_obj_t * lv_kb_get_ta(const lv_obj_t * kb)
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);

    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    return ext->ta;
}

/**
 * Set a new a mode (text or number map)
 * @param kb pointer to a Keyboard object
 * @return the current mode from 'lv_kb_mode_t'
 */
lv_kb_mode_t lv_kb_get_mode(const lv_obj_t * kb)
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);

    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    return ext->mode;
}

/**
 * Get the current cursor manage mode.
 * @param kb pointer to a Keyboard object
 * @return true: show cursor on the current text area, false: hide cursor
 */
bool lv_kb_get_cursor_manage(const lv_obj_t * kb)
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);

    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    return ext->cursor_mng == 0 ? false : true;
}

/**
 * Get the current map of a keyboard
 * @param kb pointer to a keyboard object
 * @return the current map
 */
const char ** lv_kb_get_map_array(const lv_obj_t * kb)
{
    return lv_btnm_get_map_array(kb);
}

/**
 * Get a style of a keyboard
 * @param kb pointer to a keyboard object
 * @param type which style should be get
 * @return style pointer to a style
 */
const lv_style_t * lv_kb_get_style(const lv_obj_t * kb, lv_kb_style_t type)
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);

    const lv_style_t * style = NULL;

    switch(type) {
        case LV_KB_STYLE_BG: style = lv_btnm_get_style(kb, LV_BTNM_STYLE_BG); break;
        case LV_KB_STYLE_BTN_REL: style = lv_btnm_get_style(kb, LV_BTNM_STYLE_BTN_REL); break;
        case LV_KB_STYLE_BTN_PR: style = lv_btnm_get_style(kb, LV_BTNM_STYLE_BTN_PR); break;
        case LV_KB_STYLE_BTN_TGL_REL: style = lv_btnm_get_style(kb, LV_BTNM_STYLE_BTN_TGL_REL); break;
        case LV_KB_STYLE_BTN_TGL_PR: style = lv_btnm_get_style(kb, LV_BTNM_STYLE_BTN_TGL_PR); break;
        case LV_KB_STYLE_BTN_INA: style = lv_btnm_get_style(kb, LV_BTNM_STYLE_BTN_INA); break;
        default: style = NULL; break;
    }

    return style;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Default keyboard event to add characters to the Text area and change the map.
 * If a custom `event_cb` is added to the keyboard this function be called from it to handle the
 * button clicks
 * @param kb pointer to a  keyboard
 * @param event the triggering event
 */
void lv_kb_def_event_cb(lv_obj_t * kb, lv_event_t event)
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);

    if(event != LV_EVENT_VALUE_CHANGED) return;

    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    uint16_t btn_id   = lv_btnm_get_active_btn(kb);
    if(btn_id == LV_BTNM_BTN_NONE) return;
    if(lv_btnm_get_btn_ctrl(kb, btn_id, LV_BTNM_CTRL_HIDDEN | LV_BTNM_CTRL_INACTIVE)) return;
    if(lv_btnm_get_btn_ctrl(kb, btn_id, LV_BTNM_CTRL_NO_REPEAT) && event == LV_EVENT_LONG_PRESSED_REPEAT) return;

    const char * txt = lv_btnm_get_active_btn_text(kb);
    if(txt == NULL) return;

	if(strcmp(txt, "CAPS") == 0) {
		lv_kb_capslock = true;
	}
	if(strcmp(txt, "caps") == 0) {
		lv_kb_capslock = false;
	}

    /*Do the corresponding action according to the text of the button*/
    if(strcmp(txt, "caps") == 0 || strcmp(txt, "shift") == 0) {
        lv_btnm_set_map(kb, kb_map_lc);
        lv_btnm_set_ctrl_map(kb, kb_ctrl_lc_map);
        return;
    } else if(strcmp(txt, "CAPS") == 0 || strcmp(txt, "SHIFT") == 0) {
		// TODO: if capslock flag
        lv_btnm_set_map(kb, kb_map_uc);
        lv_btnm_set_ctrl_map(kb, kb_ctrl_uc_map);
        return;
    } else if(strcmp(txt, "1#") == 0) {
        lv_btnm_set_map(kb, kb_map_spec);
        lv_btnm_set_ctrl_map(kb, kb_ctrl_spec_map);
        return;
    } else if(strcmp(txt, LV_SYMBOL_CLOSE) == 0) {
        if(kb->event_cb != lv_kb_def_event_cb) {
            lv_res_t res = lv_event_send(kb, LV_EVENT_CANCEL, NULL);
            if(res != LV_RES_OK) return;
        } else {
            lv_kb_set_ta(kb, NULL); /*De-assign the text area  to hide it cursor if needed*/
            lv_obj_del(kb);
            return;
        }
        return;
    } else if(strcmp(txt, LV_SYMBOL_OK) == 0) {
        if(kb->event_cb != lv_kb_def_event_cb) {
            lv_res_t res = lv_event_send(kb, LV_EVENT_APPLY, NULL);
            if(res != LV_RES_OK) return;
        } else {
            lv_kb_set_ta(kb, NULL); /*De-assign the text area to hide it cursor if needed*/
        }
        return;
    }

    /*Add the characters to the text area if set*/
    if(ext->ta == NULL) return;

    if(strcmp(txt, "Enter") == 0 || strcmp(txt, LV_SYMBOL_NEW_LINE) == 0)
        lv_ta_add_char(ext->ta, '\n');
    else if(strcmp(txt, LV_SYMBOL_LEFT) == 0)
        lv_ta_cursor_left(ext->ta);
    else if(strcmp(txt, LV_SYMBOL_RIGHT) == 0)
        lv_ta_cursor_right(ext->ta);
    else if(strcmp(txt, LV_SYMBOL_BACKSPACE) == 0)
        lv_ta_del_char(ext->ta);
    else if(strcmp(txt, "+/-") == 0) {
        uint16_t cur        = lv_ta_get_cursor_pos(ext->ta);
        const char * ta_txt = lv_ta_get_text(ext->ta);
        if(ta_txt[0] == '-') {
            lv_ta_set_cursor_pos(ext->ta, 1);
            lv_ta_del_char(ext->ta);
            lv_ta_add_char(ext->ta, '+');
            lv_ta_set_cursor_pos(ext->ta, cur);
        } else if(ta_txt[0] == '+') {
            lv_ta_set_cursor_pos(ext->ta, 1);
            lv_ta_del_char(ext->ta);
            lv_ta_add_char(ext->ta, '-');
            lv_ta_set_cursor_pos(ext->ta, cur);
        } else {
            lv_ta_set_cursor_pos(ext->ta, 0);
            lv_ta_add_char(ext->ta, '-');
            lv_ta_set_cursor_pos(ext->ta, cur + 1);
        }
    } else {
        lv_ta_add_text(ext->ta, txt);
    }

	if (lv_kb_capslock) {
		// Capslock is on;
		// Unshift if we were shifted.
		if (lv_kb_get_map_array(kb) == kb_map_lc) {
			lv_btnm_set_map(kb, kb_map_uc);
			lv_btnm_set_ctrl_map(kb, kb_ctrl_uc_map);
		}
	}
	else {
		// Capslock flag is off, go back to small caps
		lv_btnm_set_map(kb, kb_map_lc);
		lv_btnm_set_ctrl_map(kb, kb_ctrl_lc_map);
	}
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the keyboard
 * @param kb pointer to a keyboard object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_kb_signal(lv_obj_t * kb, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(kb, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    } else if(sign == LV_SIGNAL_FOCUS) {
        lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
        /*Show the cursor of the new Text area if cursor management is enabled*/
        if(ext->ta && ext->cursor_mng) {
            lv_cursor_type_t cur_type = lv_ta_get_cursor_type(ext->ta);
            lv_ta_set_cursor_type(ext->ta, cur_type & (~LV_CURSOR_HIDDEN));
        }
    } else if(sign == LV_SIGNAL_DEFOCUS) {
        lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
        /*Show the cursor of the new Text area if cursor management is enabled*/
        if(ext->ta && ext->cursor_mng) {
            lv_cursor_type_t cur_type = lv_ta_get_cursor_type(ext->ta);
            lv_ta_set_cursor_type(ext->ta, cur_type | LV_CURSOR_HIDDEN);
        }
    }

    return res;
}

#endif
