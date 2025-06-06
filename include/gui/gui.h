//
// Created by Robert on 24/05/2025.
//

#ifndef GUI_H
#define GUI_H

#include <stdint.h>

#include "gui_element.h"

typedef enum {
    AAF_GUI_LAYOUT_COLUMNAR,
    AAF_GUI_LAYOUT_ROW,
} AafGuiLayoutMode;

typedef struct {
    uint8_t background_color[4];
    uint8_t text_color[4];
    uint8_t button_color[4];
    uint8_t button_hover_color[4];
    uint8_t button_pressed_color[4];
    uint8_t textbox_background_color[4];
    uint8_t textbox_border_color[4];
    uint8_t textbox_border_focus_color[4];
    float corner_radius;
    float window_padding; // Margin around the window
    float button_padding; // Padding inside buttons
    float textbox_padding;// Padding inside textboxes
} AafGuiTheme;

typedef struct {
    AafGuiElement *elements;
    size_t element_count;
    int font_size;
    void *font;
    const char *font_path;
    AafGuiElement *focus;// TODO: Focus for text input etc. when implemented
    AafGuiTheme theme;
    AafGuiLayoutMode layout_mode;
} AafGuiContext;

void aaf_gui_realloc_elements_if_needed(AafGuiContext *ctx);

AafGuiElement *aaf_gui_label(AafGuiContext *ctx, const char *text, int x, int y);
AafGuiElement *aaf_gui_button(AafGuiContext *ctx, const char *text, int x, int y);
AafGuiElement *aaf_gui_text_input(AafGuiContext *ctx, const char *text, bool multiline, int x, int y, float w, float h);

AafGuiTheme aaf_gui_theme_default();
AafGuiContext aaf_gui_context_create();
void aaf_gui_set_font(AafGuiContext *ctx, const char *path, int size);
void aaf_gui_reload_font(AafGuiContext *ctx);

void aaf_gui_draw_text_input(AafGuiContext *ctx, AafGuiElement *self);
void aaf_gui_draw_button(AafGuiContext *ctx, AafGuiElement *self);
void aaf_gui_draw_label(AafGuiContext *ctx, AafGuiElement *self);

void aaf_gui_update_text_input(AafGuiContext *ctx, AafGuiElement *self);

void aaf_calculate_layout(AafGuiContext *ctx);

#endif//GUI_H
