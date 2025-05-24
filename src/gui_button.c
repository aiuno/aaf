//
// Created by Robert on 24/05/2025.
//

#include "gui.h"

#include <math.h>
#include <raylib.h>

AafGuiElement *aaf_gui_button(AafGuiContext *ctx, const char *text, int x, int y) {
    if (ctx == NULL) {
        return NULL;
    }

    Vector2 wh = MeasureTextEx(*(Font*)ctx->font, text, 20, 1);

    AafGuiButton button = {text};
    AafGuiElement element = {GUI_BUTTON, .as_button = button, .w = wh.x + ctx->theme.button_padding * 2, .h = wh.y + ctx->theme.button_padding * 2};

    _aaf_realloc_gui_elements_if_needed(ctx);

    AafGuiElement *new_element = &ctx->elements[ctx->element_count];
    *new_element = element;
    ctx->element_count += 1;

    return new_element;
}

void draw_gui_button(AafGuiContext *ctx, AafGuiElement *self) {
    if (self == NULL) {
        return;
    }

    const char *text = self->as_button.text;
    Rectangle self_rect = {
        .x = self->x,
        .y = self->y,
        .width = self->w,
        .height = self->h,
    };

    float roundness = ctx->theme.corner_radius / fminf(self->w, self->h);

    if (self->event & AAF_GUI_EVENT_HOVER) {
        if (self->event & AAF_GUI_EVENT_CLICK) {
            DrawRectangleRounded(self_rect, roundness, 20, *(Color*)ctx->theme.button_pressed_color);
        } else {
            DrawRectangleRounded(self_rect, roundness, 20, *(Color*)ctx->theme.button_hover_color);
        }
    } else {
        DrawRectangleRounded(self_rect, roundness, 20, *(Color*)ctx->theme.button_color);
    }
    DrawRectangleRoundedLinesEx(self_rect, roundness, 20, 2, *(Color*)ctx->theme.button_pressed_color);
    DrawTextEx(*(Font*)ctx->font, text, (Vector2){self->x + ctx->theme.button_padding, self->y + ctx->theme.button_padding}, 20, 1, *(Color*)ctx->theme.text_color);
}
