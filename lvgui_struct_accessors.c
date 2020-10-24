#include "lvgui_struct_accessors.h"
#include <stdlib.h>

lv_style_t * lvgui_allocate_lv_style() {
  return malloc(sizeof(lv_style_t));
}

uint8_t lvgui_get_lv_style__glass(lv_style_t * s) {
  return s->glass;
}
void lvgui_set_lv_style__glass(lv_style_t * s, uint8_t value) {
  s->glass = value;
}

lv_color_t lvgui_get_lv_style__body_main_color(lv_style_t * s) {
  return s->body.main_color;
}
void lvgui_set_lv_style__body_main_color(lv_style_t * s, lv_color_t value) {
  s->body.main_color = value;
}

lv_color_t lvgui_get_lv_style__body_grad_color(lv_style_t * s) {
  return s->body.grad_color;
}
void lvgui_set_lv_style__body_grad_color(lv_style_t * s, lv_color_t value) {
  s->body.grad_color = value;
}

lv_coord_t lvgui_get_lv_style__body_radius(lv_style_t * s) {
  return s->body.radius;
}
void lvgui_set_lv_style__body_radius(lv_style_t * s, lv_coord_t value) {
  s->body.radius = value;
}

lv_opa_t lvgui_get_lv_style__body_opa(lv_style_t * s) {
  return s->body.opa;
}
void lvgui_set_lv_style__body_opa(lv_style_t * s, lv_opa_t value) {
  s->body.opa = value;
}

lv_color_t lvgui_get_lv_style__body_border_color(lv_style_t * s) {
  return s->body.border.color;
}
void lvgui_set_lv_style__body_border_color(lv_style_t * s, lv_color_t value) {
  s->body.border.color = value;
}

lv_coord_t lvgui_get_lv_style__body_border_width(lv_style_t * s) {
  return s->body.border.width;
}
void lvgui_set_lv_style__body_border_width(lv_style_t * s, lv_coord_t value) {
  s->body.border.width = value;
}

lv_border_part_t lvgui_get_lv_style__body_border_part(lv_style_t * s) {
  return s->body.border.part;
}
void lvgui_set_lv_style__body_border_part(lv_style_t * s, lv_border_part_t value) {
  s->body.border.part = value;
}

lv_opa_t lvgui_get_lv_style__body_border_opa(lv_style_t * s) {
  return s->body.border.opa;
}
void lvgui_set_lv_style__body_border_opa(lv_style_t * s, lv_opa_t value) {
  s->body.border.opa = value;
}

lv_color_t lvgui_get_lv_style__body_shadow_color(lv_style_t * s) {
  return s->body.shadow.color;
}
void lvgui_set_lv_style__body_shadow_color(lv_style_t * s, lv_color_t value) {
  s->body.shadow.color = value;
}

lv_coord_t lvgui_get_lv_style__body_shadow_width(lv_style_t * s) {
  return s->body.shadow.width;
}
void lvgui_set_lv_style__body_shadow_width(lv_style_t * s, lv_coord_t value) {
  s->body.shadow.width = value;
}

lv_shadow_type_t lvgui_get_lv_style__body_shadow_type(lv_style_t * s) {
  return s->body.shadow.type;
}
void lvgui_set_lv_style__body_shadow_type(lv_style_t * s, lv_shadow_type_t value) {
  s->body.shadow.type = value;
}

lv_coord_t lvgui_get_lv_style__body_padding_top(lv_style_t * s) {
  return s->body.padding.top;
}
void lvgui_set_lv_style__body_padding_top(lv_style_t * s, lv_coord_t value) {
  s->body.padding.top = value;
}

lv_coord_t lvgui_get_lv_style__body_padding_bottom(lv_style_t * s) {
  return s->body.padding.bottom;
}
void lvgui_set_lv_style__body_padding_bottom(lv_style_t * s, lv_coord_t value) {
  s->body.padding.bottom = value;
}

lv_coord_t lvgui_get_lv_style__body_padding_left(lv_style_t * s) {
  return s->body.padding.left;
}
void lvgui_set_lv_style__body_padding_left(lv_style_t * s, lv_coord_t value) {
  s->body.padding.left = value;
}

lv_coord_t lvgui_get_lv_style__body_padding_right(lv_style_t * s) {
  return s->body.padding.right;
}
void lvgui_set_lv_style__body_padding_right(lv_style_t * s, lv_coord_t value) {
  s->body.padding.right = value;
}

lv_coord_t lvgui_get_lv_style__body_padding_inner(lv_style_t * s) {
  return s->body.padding.inner;
}
void lvgui_set_lv_style__body_padding_inner(lv_style_t * s, lv_coord_t value) {
  s->body.padding.inner = value;
}

lv_color_t lvgui_get_lv_style__text_color(lv_style_t * s) {
  return s->text.color;
}
void lvgui_set_lv_style__text_color(lv_style_t * s, lv_color_t value) {
  s->text.color = value;
}

lv_color_t lvgui_get_lv_style__text_sel_color(lv_style_t * s) {
  return s->text.sel_color;
}
void lvgui_set_lv_style__text_sel_color(lv_style_t * s, lv_color_t value) {
  s->text.sel_color = value;
}

lv_font_t * lvgui_get_lv_style__text_font(lv_style_t * s) {
  return s->text.font;
}
void lvgui_set_lv_style__text_font(lv_style_t * s, lv_font_t * value) {
  s->text.font = value;
}

lv_coord_t lvgui_get_lv_style__text_letter_space(lv_style_t * s) {
  return s->text.letter_space;
}
void lvgui_set_lv_style__text_letter_space(lv_style_t * s, lv_coord_t value) {
  s->text.letter_space = value;
}

lv_coord_t lvgui_get_lv_style__text_line_space(lv_style_t * s) {
  return s->text.line_space;
}
void lvgui_set_lv_style__text_line_space(lv_style_t * s, lv_coord_t value) {
  s->text.line_space = value;
}

lv_opa_t lvgui_get_lv_style__text_opa(lv_style_t * s) {
  return s->text.opa;
}
void lvgui_set_lv_style__text_opa(lv_style_t * s, lv_opa_t value) {
  s->text.opa = value;
}

lv_color_t lvgui_get_lv_style__image_color(lv_style_t * s) {
  return s->image.color;
}
void lvgui_set_lv_style__image_color(lv_style_t * s, lv_color_t value) {
  s->image.color = value;
}

lv_opa_t lvgui_get_lv_style__image_intense(lv_style_t * s) {
  return s->image.intense;
}
void lvgui_set_lv_style__image_intense(lv_style_t * s, lv_opa_t value) {
  s->image.intense = value;
}

lv_opa_t lvgui_get_lv_style__image_opa(lv_style_t * s) {
  return s->image.opa;
}
void lvgui_set_lv_style__image_opa(lv_style_t * s, lv_opa_t value) {
  s->image.opa = value;
}

lv_color_t lvgui_get_lv_style__line_color(lv_style_t * s) {
  return s->line.color;
}
void lvgui_set_lv_style__line_color(lv_style_t * s, lv_color_t value) {
  s->line.color = value;
}

lv_coord_t lvgui_get_lv_style__line_width(lv_style_t * s) {
  return s->line.width;
}
void lvgui_set_lv_style__line_width(lv_style_t * s, lv_coord_t value) {
  s->line.width = value;
}

lv_opa_t lvgui_get_lv_style__line_opa(lv_style_t * s) {
  return s->line.opa;
}
void lvgui_set_lv_style__line_opa(lv_style_t * s, lv_opa_t value) {
  s->line.opa = value;
}

uint8_t lvgui_get_lv_style__line_rounded(lv_style_t * s) {
  return s->line.rounded;
}
void lvgui_set_lv_style__line_rounded(lv_style_t * s, uint8_t value) {
  s->line.rounded = value;
}

lv_anim_t * lvgui_allocate_lv_anim() {
  return malloc(sizeof(lv_anim_t));
}
