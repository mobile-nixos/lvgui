/**
 * @file lv_theme_night.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"
#include "lv_theme.h"

#include "lv_conf.h"

#if USE_LV_THEME_NIGHT

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_theme_t theme;
static lv_style_t def;

/*Static style definitions*/
static lv_style_t bg;
static lv_style_t prim, sec, hint;
static lv_style_t btn_rel, btn_pr, btn_tgl_rel, btn_tgl_pr, btn_ina;
static lv_style_t bar_bg, bar_indic;
static lv_style_t slider_knob;
static lv_style_t ddlist_bg, ddlist_sel;
static lv_style_t lmeter_bg;

/*Saved input parameters*/
static uint16_t _hue;
static font_t * _font;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void basic_init(void)
{
    lv_style_copy(&def, &lv_style_pretty);  /*Initialize the default style*/

    lv_style_copy(&bg, &lv_style_plain);
    bg.body.main_color = color_hsv_to_rgb(_hue, 11, 30);
    bg.body.grad_color = color_hsv_to_rgb(_hue, 11, 30);
    bg.text.color = color_hsv_to_rgb(_hue, 5, 95);

    theme.bg = &bg;
    theme.panel = &def;
}

static void btn_init(void)
{
#if USE_LV_BTN != 0

    lv_style_copy(&btn_rel, &lv_style_pretty);
    btn_rel.body.main_color = color_hsv_to_rgb(_hue, 10, 40);
    btn_rel.body.grad_color = color_hsv_to_rgb(_hue, 10, 20);
    btn_rel.body.border.color = COLOR_HEX3(0x111);
    btn_rel.body.border.width = 1 << LV_AA;
    btn_rel.body.border.opa = OPA_70;
    btn_rel.body.padding.hor = LV_DPI / 4;
    btn_rel.body.padding.ver = LV_DPI / 8;
    btn_rel.body.shadow.type = LV_SHADOW_BOTTOM;
    btn_rel.body.shadow.color = COLOR_HEX3(0x111);
    btn_rel.body.shadow.width = LV_DPI / 30;
    btn_rel.text.color = COLOR_HEX3(0xeee);

    lv_style_copy(&btn_pr, &btn_rel);
    btn_pr.body.main_color = color_hsv_to_rgb(_hue, 10, 30);
    btn_pr.body.grad_color = color_hsv_to_rgb(_hue, 10, 10);

    lv_style_copy(&btn_tgl_rel, &btn_rel);
    btn_tgl_rel.body.main_color = color_hsv_to_rgb(_hue, 10, 20);
    btn_tgl_rel.body.grad_color = color_hsv_to_rgb(_hue, 10, 40);
    btn_tgl_rel.body.shadow.width = LV_DPI / 40;
    btn_tgl_rel.text.color = COLOR_HEX3(0xddd);

    lv_style_copy(&btn_tgl_pr, &btn_rel);
    btn_tgl_pr.body.main_color = color_hsv_to_rgb(_hue, 10, 10);
    btn_tgl_pr.body.grad_color = color_hsv_to_rgb(_hue, 10, 30);
    btn_tgl_pr.body.shadow.width = LV_DPI / 30;
    btn_tgl_pr.text.color = COLOR_HEX3(0xddd);

    lv_style_copy(&btn_ina, &btn_rel);
    btn_ina.body.main_color = color_hsv_to_rgb(_hue, 10, 20);
    btn_ina.body.grad_color = color_hsv_to_rgb(_hue, 10, 20);
    btn_ina.text.color = COLOR_HEX3(0xaaa);
    btn_ina.body.shadow.width = 0;

    theme.btn.rel = &btn_rel;
    theme.btn.pr = &btn_pr;
    theme.btn.tgl_rel = &btn_tgl_rel;
    theme.btn.tgl_pr =  &btn_tgl_pr;
    theme.btn.ina =  &btn_ina;
#endif
}


static void label_init(void)
{
#if USE_LV_LABEL != 0

    lv_style_copy(&prim, &bg);
    prim.text.color = color_hsv_to_rgb(_hue, 5, 95);

    lv_style_copy(&sec, &bg);
    sec.text.color = color_hsv_to_rgb(_hue, 15, 65);

    lv_style_copy(&hint, &bg);
    hint.text.color = color_hsv_to_rgb(_hue, 20, 55);

    theme.label.prim = &prim;
    theme.label.sec = &sec;
    theme.label.hint = &hint;
#endif
}

static void img_init(void)
{
#if USE_LV_IMG != 0


    theme.img.light = &def;
    theme.img.dark = &def;
#endif
}

static void line_init(void)
{
#if USE_LV_LINE != 0


    theme.line.decor = &def;
#endif
}

static void led_init(void)
{
#if USE_LV_LED != 0


    theme.led = &def;
#endif
}

static void bar_init(void)
{
#if USE_LV_BAR
    lv_style_copy(&bar_bg, &lv_style_pretty);
    bar_bg.body.main_color = color_hsv_to_rgb(_hue, 10, 15);
    bar_bg.body.grad_color = color_hsv_to_rgb(_hue, 10, 15);
    bar_bg.body.border.color = color_hsv_to_rgb(_hue, 10, 10);
    bar_bg.body.border.width = 1 << LV_AA;
    bar_bg.body.border.opa = OPA_COVER;
    bar_bg.body.radius = LV_RADIUS_CIRCLE;

    lv_style_copy(&bar_indic, &lv_style_pretty);
    bar_indic.body.main_color = color_hsv_to_rgb(_hue, 80, 70);
    bar_indic.body.grad_color = color_hsv_to_rgb(_hue, 80, 70);
    bar_indic.body.border.color = color_hsv_to_rgb(_hue, 20, 15);
    bar_indic.body.border.width = 1 << LV_AA;
    bar_indic.body.border.opa = OPA_COVER;
    bar_indic.body.radius = LV_RADIUS_CIRCLE;
    bar_indic.body.padding.hor = 0;
    bar_indic.body.padding.ver = 0;

    theme.bar.bg = &bar_bg;
    theme.bar.indic = &bar_indic;
#endif
}

static void slider_init(void)
{
#if USE_LV_SLIDER != 0

    lv_style_copy(&slider_knob, &btn_rel);
    slider_knob.body.radius = LV_RADIUS_CIRCLE;

    theme.slider.bg = &bar_bg;
    theme.slider.indic = &bar_indic;
    theme.slider.knob = &slider_knob;
#endif
}

static void sw_init(void)
{
#if USE_LV_SW != 0


    theme.sw.bg = &bar_bg;
    theme.sw.indic = &bar_indic;
    theme.sw.knob_off = &slider_knob;
    theme.sw.knob_on = &slider_knob;
#endif
}


static void lmeter_init(void)
{
#if USE_LV_LMETER != 0
    lv_style_copy(&lmeter_bg, &def);
    lmeter_bg.body.main_color = color_hsv_to_rgb(_hue, 10, 70);
    lmeter_bg.body.grad_color = color_hsv_to_rgb(_hue, 80, 80);
    lmeter_bg.body.padding.hor = LV_DPI / 10;           /*Scale line length*/
    lmeter_bg.body.padding.inner = LV_DPI / 10;         /*Text padding*/
    lmeter_bg.body.border.color = COLOR_HEX3(0x333);
    lmeter_bg.line.color = COLOR_HEX3(0x555);
    lmeter_bg.line.width = 2 << LV_ANTIALIAS;
    lmeter_bg.text.color = COLOR_HEX3(0xddd);

    theme.lmeter = &lmeter_bg;
#endif
}

static void gauge_init(void)
{
#if USE_LV_GAUGE != 0


    theme.gauge = &lmeter_bg;
#endif
}

static void chart_init(void)
{
#if USE_LV_CHART

    theme.chart = &def;
#endif
}

static void cb_init(void)
{
#if USE_LV_CB != 0

    theme.cb.bg = &lv_style_transp;
    theme.cb.box.rel = &def;
    theme.cb.box.pr = &def;
    theme.cb.box.tgl_rel = &def;
    theme.cb.box.tgl_pr = &def;
    theme.cb.box.ina = &def;
#endif
}


static void btnm_init(void)
{
#if USE_LV_BTNM
    static lv_style_t bg, rel, pr, tgl_rel, tgl_pr, ina;

    lv_style_copy(&bg, &btn_rel);
    bg.body.padding.hor = 2 << LV_AA;
    bg.body.padding.ver = 2 << LV_AA;
    bg.body.padding.inner = 0;
    bg.body.border.width =  1<< LV_AA;//LV_BORDER_RIGHT;

    lv_style_copy(&rel, &btn_rel);
    rel.body.border.part = LV_BORDER_RIGHT;
    rel.body.border.width = 2 << LV_AA;
    rel.body.radius = 0;

    lv_style_copy(&pr, &btn_pr);
    pr.body.border.part = LV_BORDER_RIGHT;
    pr.body.border.width = 2 << LV_AA;
    pr.body.radius = 0;

    lv_style_copy(&tgl_rel, &btn_tgl_rel);
    tgl_rel.body.border.part = LV_BORDER_RIGHT;
    tgl_rel.body.border.width = 2 << LV_AA;
    tgl_rel.body.radius = 0;

    lv_style_copy(&tgl_pr, &btn_tgl_pr);
    tgl_pr.body.border.part = LV_BORDER_RIGHT;
    tgl_pr.body.border.width = 2 << LV_AA;
    tgl_pr.body.radius = 0;

    lv_style_copy(&ina, &btn_ina);
    ina.body.border.part = LV_BORDER_RIGHT;
    ina.body.border.width = 2 << LV_AA;
    ina.body.radius = 0;

    theme.btnm.bg = &bg;
    theme.btnm.btn.rel = &rel;
    theme.btnm.btn.pr = &pr;
    theme.btnm.btn.tgl_rel = &tgl_rel;
    theme.btnm.btn.tgl_pr = &tgl_pr;
    theme.btnm.btn.ina = &ina;
#endif
}

static void kb_init(void)
{
#if USE_LV_KB


    theme.kb.bg = &def;
    theme.kb.btn.rel = &def;
    theme.kb.btn.pr = &def;
    theme.kb.btn.tgl_rel = &def;
    theme.kb.btn.tgl_pr = &def;
    theme.kb.btn.ina = &def;
#endif

}

static void mbox_init(void)
{
#if USE_LV_MBOX
    static lv_style_t mbox_bg;
    lv_style_copy(&mbox_bg, &bg);
    mbox_bg.body.main_color =  color_hsv_to_rgb(_hue, 30, 30);
    mbox_bg.body.grad_color =  color_hsv_to_rgb(_hue, 30, 30);
    mbox_bg.body.border.color =  color_hsv_to_rgb(_hue, 11, 20);
    mbox_bg.body.border.width = 1 << LV_AA;
    mbox_bg.body.shadow.width = LV_DPI / 10;
    mbox_bg.body.shadow.color = COLOR_HEX3(0x222);
    mbox_bg.body.radius = LV_DPI / 20;
    theme.mbox.bg = &mbox_bg;
    theme.mbox.btn.bg = &lv_style_transp;
    theme.mbox.btn.rel = &btn_rel;
    theme.mbox.btn.pr = &btn_pr;
#endif
}

static void page_init(void)
{
#if USE_LV_PAGE


    theme.page.bg = &def;
    theme.page.scrl = &def;
    theme.page.sb = &def;
#endif
}

static void ta_init(void)
{
#if USE_LV_TA

    static lv_style_t list_ta;

    lv_style_copy(&list_ta, &bg);
    list_ta.body.main_color = color_hsv_to_rgb(_hue, 11, 18);
    list_ta.body.grad_color = color_hsv_to_rgb(_hue, 11, 18);
    list_ta.body.radius = LV_DPI / 30;
    list_ta.body.border.color = color_hsv_to_rgb(_hue, 10, 10);
    list_ta.body.border.width = 1 << LV_AA;
    list_ta.body.border.opa = OPA_COVER;
    list_ta.body.padding.ver = LV_DPI / 10;
    list_ta.body.padding.hor = LV_DPI / 10;

    theme.ta.area = &list_ta;
    theme.ta.oneline = &list_ta;
    theme.ta.cursor = NULL;
    theme.ta.sb = &def;
#endif
}

static void list_init(void)
{
#if USE_LV_LIST != 0
    static lv_style_t list_bg, list_btn_rel, list_btn_pr, list_btn_tgl_rel, list_btn_tgl_pr;

    lv_style_copy(&list_bg, &bg);
    list_bg.body.main_color = color_hsv_to_rgb(_hue, 11, 18);
    list_bg.body.grad_color = color_hsv_to_rgb(_hue, 11, 18);
    list_bg.body.radius = LV_DPI / 30;
    list_bg.body.border.color = color_hsv_to_rgb(_hue, 10, 10);
    list_bg.body.border.width = 1 << LV_AA;
    list_bg.body.border.opa = OPA_COVER;
    list_bg.body.padding.ver = 0;
    list_bg.body.padding.hor = 0;

    lv_style_copy(&list_btn_rel, &bg);
    list_btn_rel.body.empty = 1;
    list_btn_rel.body.border.part = LV_BORDER_BOTTOM;
    list_btn_rel.body.border.color = color_hsv_to_rgb(_hue, 10, 5);
    list_btn_rel.body.border.width = 1 << LV_AA;
    list_btn_rel.body.radius = LV_DPI / 10;
    list_btn_rel.text.color = color_hsv_to_rgb(_hue, 5, 80);
    list_btn_rel.body.padding.ver = LV_DPI / 10;
    list_btn_rel.body.padding.hor = LV_DPI / 10;

    lv_style_copy(&list_btn_pr, &btn_pr);
    list_btn_pr.body.main_color = btn_pr.body.grad_color;
    list_btn_pr.body.grad_color = btn_pr.body.main_color;
    list_btn_pr.body.border.color = color_hsv_to_rgb(_hue, 10, 5);
    list_btn_pr.body.border.width = 0;
    list_btn_pr.body.radius = LV_DPI / 30;
    list_btn_pr.body.padding.ver = LV_DPI / 10;
    list_btn_pr.body.padding.hor = LV_DPI / 10;
    list_btn_pr.text.color = color_hsv_to_rgb(_hue, 5, 80);

    lv_style_copy(&list_btn_tgl_rel, &list_btn_rel);
    list_btn_tgl_rel.body.empty = 0;
    list_btn_tgl_rel.body.main_color = color_hsv_to_rgb(_hue, 10, 8);
    list_btn_tgl_rel.body.grad_color = color_hsv_to_rgb(_hue, 10, 8);
    list_btn_tgl_rel.body.radius = LV_DPI / 30;

    lv_style_copy(&list_btn_tgl_pr, &list_btn_tgl_rel);
    list_btn_tgl_pr.body.main_color = btn_tgl_pr.body.main_color;
    list_btn_tgl_pr.body.grad_color = btn_tgl_pr.body.grad_color;

    theme.list.sb = &def;
    theme.list.bg = &list_bg;
    theme.list.scrl = &lv_style_transp_tight;
    theme.list.btn.rel = &list_btn_rel;
    theme.list.btn.pr = &list_btn_pr;
    theme.list.btn.tgl_rel = &list_btn_tgl_rel;
    theme.list.btn.tgl_pr = &list_btn_tgl_pr;
    theme.list.btn.ina = &def;
#endif
}

static void ddlist_init(void)
{
#if USE_LV_DDLIST != 0
    lv_style_copy(&ddlist_bg, &btn_rel);
    ddlist_bg.text.line_space = LV_DPI / 8;
    ddlist_bg.body.padding.ver = LV_DPI / 8;
    ddlist_bg.body.padding.hor = LV_DPI / 8;
    ddlist_bg.body.radius = LV_DPI / 30;

    lv_style_copy(&ddlist_sel, &btn_rel);
    ddlist_sel.body.main_color = color_hsv_to_rgb(_hue, 20, 50);
    ddlist_sel.body.grad_color = color_hsv_to_rgb(_hue, 20, 50);
    ddlist_sel.body.radius = 0;

    theme.ddlist.bg = &ddlist_bg;
    theme.ddlist.sel = &ddlist_sel;
    theme.ddlist.sb = &def;
#endif
}

static void roller_init(void)
{
#if USE_LV_ROLLER != 0
    static lv_style_t roller_bg;

    lv_style_copy(&roller_bg, &ddlist_bg);
    roller_bg.body.main_color = color_hsv_to_rgb(_hue, 10, 20);
    roller_bg.body.grad_color = color_hsv_to_rgb(_hue, 10, 40);
    roller_bg.text.color = color_hsv_to_rgb(_hue, 5, 70);
    roller_bg.text.opa = OPA_60;

    theme.roller.bg = &roller_bg;
    theme.roller.sel = &ddlist_sel;
#endif
}

static void tabview_init(void)
{
#if USE_LV_TABVIEW != 0


    theme.tabview.bg = &bg;
    theme.tabview.sb = &def;
    theme.tabview.indic = &lv_style_transp;
    theme.tabview.btn.bg = &lv_style_transp;
    theme.tabview.btn.rel = &btn_rel;
    theme.tabview.btn.pr = &btn_pr;
    theme.tabview.btn.tgl_rel = &btn_tgl_rel;
    theme.tabview.btn.tgl_pr = &btn_tgl_pr;
#endif
}


static void win_init(void)
{
#if USE_LV_WIN != 0
    static lv_style_t win_bg;
    lv_style_copy(&win_bg, &bg);
    win_bg.body.main_color = color_hsv_to_rgb(_hue, 10, 60);
    win_bg.body.grad_color = color_hsv_to_rgb(_hue, 10, 60);
    win_bg.body.radius = LV_DPI / 20;
    win_bg.body.border.color = COLOR_HEX3(0x333);
    win_bg.body.border.width = 1 << LV_AA;
    win_bg.body.border.width = 1 << LV_AA;

    static lv_style_t win_header;
    lv_style_copy(&win_header, &win_bg);
    win_header.body.main_color = color_hsv_to_rgb(_hue, 10, 20);
    win_header.body.grad_color = color_hsv_to_rgb(_hue, 10, 20);
    win_header.body.radius = 0;
    win_header.body.padding.hor = 0;
    win_header.body.padding.ver = 0;
    win_header.body.padding.ver = 0;

    static lv_style_t win_btn_pr;
    lv_style_copy(&win_btn_pr, &lv_style_plain);
    win_btn_pr.body.main_color = color_hsv_to_rgb(_hue, 10, 10);
    win_btn_pr.body.grad_color = color_hsv_to_rgb(_hue, 10, 10);

    theme.win.bg = &win_bg;
    theme.win.sb = &def;
    theme.win.header = &win_header;
    theme.win.content.bg = &lv_style_transp;
    theme.win.content.scrl = &lv_style_transp;
    theme.win.btn.rel = &lv_style_transp;
    theme.win.btn.pr = &win_btn_pr;
#endif
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/



/**
 * Initialize the night theme
 * @param hue [0..360] hue value from HSV color space to define the theme's base color
 * @param font pointer to a font (NULL to use the default)
 * @return pointer to the initialized theme
 */
lv_theme_t * lv_theme_night_init(uint16_t hue, font_t *font)
{
    if(font == NULL) font = LV_FONT_DEFAULT;

    _hue = hue;
    _font = font;

    /*For backward compatibility initialize all theme elements with a default style */
    uint16_t i;
    lv_style_t **style_p = (lv_style_t**) &theme;
    for(i = 0; i < sizeof(lv_theme_t) / sizeof(lv_style_t*); i++) {
        *style_p = &def;
        style_p++;
    }

    basic_init();
    btn_init();
    label_init();
    img_init();
    line_init();
    led_init();
    bar_init();
    slider_init();
    sw_init();
    lmeter_init();
    gauge_init();
    chart_init();
    cb_init();
    btnm_init();
    kb_init();
    mbox_init();
    page_init();
    ta_init();
    list_init();
    ddlist_init();
    roller_init();
    tabview_init();
    win_init();

    return &theme;
}

/**
 * Get a pointer to the theme
 * @return pointer to the theme
 */
lv_theme_t * lv_theme_get_deafult(void)
{
    return &theme;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif

