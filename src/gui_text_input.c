//
// Created by Robert on 24/05/2025.
//

#include "gui.h"

#include <math.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

AafGuiElement *aaf_gui_text_input(AafGuiContext *ctx, const char *text, bool multiline, int x, int y, float w, float h) {
    if (ctx == NULL) {
        return NULL;
    }

    AafGuiTextInput text_input = {
        .buffer = malloc(sizeof(char) * (strlen(text) + 1)),
        .buffer_size = strlen(text) + 1,
        .cursor_pos = 0,
        .multiline = multiline,
    };
    AafGuiElement element = {GUI_TEXT_INPUT, .as_text_input = text_input, .w = w + ctx->theme.textbox_padding * 2, .h = h + ctx->theme.textbox_padding * 2};
    strcpy(element.as_text_input.buffer, text);

    _aaf_realloc_gui_elements_if_needed(ctx);

    AafGuiElement *new_element = &ctx->elements[ctx->element_count];
    *new_element = element;
    ctx->element_count += 1;

    return new_element;
}

void draw_gui_text_input(AafGuiContext *ctx, AafGuiElement *self) {
    if (self == NULL) {
        return;
    }

    char *text = self->as_text_input.buffer;
    Rectangle self_rect = {
        .x = self->x,
        .y = self->y,
        .width = self->w,
        .height = self->h,
    };

    float roundness = ctx->theme.corner_radius / fminf(self->w, self->h);

    DrawRectangleRounded(self_rect, roundness, 20, *(Color*)ctx->theme.textbox_background_color);
    if (self == ctx->focus) {
        DrawRectangleRoundedLinesEx(self_rect, roundness, 20, 2, *(Color*)ctx->theme.textbox_border_focus_color);
    } else {
        DrawRectangleRoundedLinesEx(self_rect, roundness, 20, 2, *(Color*)ctx->theme.textbox_border_color);
    }
    DrawTextEx(*(Font*)ctx->font, text, (Vector2){self->x + ctx->theme.textbox_padding, self->y + ctx->theme.textbox_padding}, 20, 1, *(Color*)ctx->theme.text_color);
}
