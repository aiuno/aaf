//
// Created by Robert on 24/05/2025.
//

#include "aaf.h"

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

AafGuiTheme aaf_gui_theme_default() {
    AafGuiTheme theme = {
            .button_color = {255, 255, 255, 255},
            .button_hover_color = {200, 200, 200, 255},
            .background_color = {255, 255, 255, 255},
            .text_color = {0, 0, 0, 255},
            .button_pressed_color = {150, 150, 150, 255},
            .textbox_background_color = {255, 255, 255, 255},
            .textbox_border_color = {150, 150, 150, 255},
            .textbox_border_focus_color = {0, 0, 255, 255},
            .corner_radius = 15.0f,
            .window_padding = 10.0f, // Margin around the window
            .button_padding = 10.0f, // Padding inside buttons
            .textbox_padding = 10.0f,// Padding inside textboxes
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
    ctx->font_size = (float) size;
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

AafGuiContext aaf_gui_context_create() {
    AafGuiContext ctx = {
            .elements = malloc(sizeof(AafGuiElement) * 100),
            .element_count = 0,
            .font_size = 20.0f,
            .font = NULL,
            .focus = NULL,
            .theme = aaf_gui_theme_default(),
            .layout_mode = AAF_GUI_LAYOUT_COLUMNAR,
    };

    if (ctx.elements == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for GUI elements\n");
        exit(EXIT_FAILURE);
    }

    return ctx;
}

void aaf_calculate_layout(AafGuiContext *ctx) {// TODO: Make more smarter
    for (size_t i = 0; i < ctx->element_count; ++i) {
        AafGuiElement *prev_element = (i > 0) ? &ctx->elements[i - 1] : NULL;
        AafGuiElement *element = &ctx->elements[i];

        if (prev_element != NULL) {
            if (ctx->layout_mode == AAF_GUI_LAYOUT_COLUMNAR) {
                element->x = prev_element->x;
                element->y = prev_element->y + prev_element->h + ctx->theme.window_padding;
            } else if (ctx->layout_mode == AAF_GUI_LAYOUT_ROW) {
                element->x = prev_element->x + prev_element->w + ctx->theme.window_padding;
                element->y = prev_element->y;
            } else {
                element->x = ctx->theme.window_padding;
                element->y = ctx->theme.window_padding;
            }
        } else {
            element->x = ctx->theme.window_padding;
            element->y = ctx->theme.window_padding;
        }
    }
}