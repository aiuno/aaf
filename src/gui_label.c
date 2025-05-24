//
// Created by Robert on 24/05/2025.
//

#include "gui.h"

#include <raylib.h>

AafGuiElement *aaf_gui_label(AafGuiContext *ctx, const char *text, int x, int y) {
    if (ctx == NULL) {
        return NULL;
    }

    Vector2 wh = MeasureTextEx(*(Font *) ctx->font, text, ctx->font_size, 1);

    AafGuiLabel label = {text};
    AafGuiElement element = {GUI_LABEL, .as_label = label, .w = wh.x, .h = wh.y};

    _aaf_realloc_gui_elements_if_needed(ctx);

    AafGuiElement *new_element = &ctx->elements[ctx->element_count];
    *new_element = element;
    ctx->element_count += 1;

    aaf_calculate_layout(ctx);

    return new_element;
}

void aaf_draw_gui_label(AafGuiContext *ctx, AafGuiElement *self) {
    if (self == NULL) {
        return;
    }

    const char *text = self->as_label.text;

    DrawTextEx(*(Font *) ctx->font, text, (Vector2) {self->x, self->y}, 20, 1, *(Color *) ctx->theme.text_color);
}