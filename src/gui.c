//
// Created by Robert on 24/05/2025.
//

#include "aaf.h"

#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

AafGuiTheme aaf_gui_theme_default() {
    AafGuiTheme theme = {
        .button_color = { 255, 255, 255, 255 },
        .button_hover_color = { 200, 200, 200, 255 },
        .background_color = { 255, 255, 255, 255 },
        .text_color = { 0, 0, 0, 255 },
        .button_pressed_color = { 150, 150, 150, 255 },
        .textbox_background_color = { 255, 255, 255, 255 },
        .textbox_border_color = { 150, 150, 150, 255 },
        .textbox_border_focus_color = { 0, 0, 255, 255 },
        .corner_radius = 15.0f,
        .window_padding = 10.0f, // Margin around the window
        .button_padding = 10.0f, // Padding inside buttons
        .textbox_padding = 10.0f, // Padding inside textboxes
    };
    return theme;
}

void aaf_gui_set_font(AafGuiContext *ctx, const char *path, int size) {
    if (ctx == NULL || path == NULL) {
        fprintf(stderr, "Error: AafContext or path is NULL\n");
        return;
    }

    Font *font = malloc(sizeof(Font));
    *font = LoadFontEx(path, size, NULL, 250);

    ctx->font = font;
    ctx->font_size = (float)size;
}

void _aaf_realloc_gui_elements_if_needed(AafGuiContext *ctx) {
    if (ctx->element_count >= 100) {
        size_t new_size = ctx->element_count * 2;
        AafGuiElement *new_elements = realloc(ctx->elements, sizeof(AafGuiElement) * new_size);
        if (new_elements == NULL) {
            fprintf(stderr, "Error: Failed to reallocate memory for GUI elements\n");
            return;
        }
        ctx->elements = new_elements;
    }
}