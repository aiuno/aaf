//
// Created by Robert on 24/05/2025.
//

#include "gui.h"

#include <math.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AafGuiElement *aaf_gui_text_input(AafGuiContext *ctx, const char *text, bool multiline, int x, int y, float w, float h) {
    if (ctx == NULL) {
        return NULL;
    }

    AafGuiTextInput text_input = {
            .buffer = malloc(sizeof(char) * (strlen(text) + 1)),
            .buffer_size = strlen(text) + 1,
            .cursor_pos = strlen(text),
            .multiline = multiline,
    };
    strcpy(text_input.buffer, text);
    AafGuiElement element = {GUI_TEXT_INPUT, .as_text_input = text_input, .w = w + ctx->theme.textbox_padding * 2, .h = h + ctx->theme.textbox_padding * 2};

    _aaf_realloc_gui_elements_if_needed(ctx);

    AafGuiElement *new_element = &ctx->elements[ctx->element_count];
    *new_element = element;
    ctx->element_count += 1;

    return new_element;
}

Vector2 calculate_cursor_pos(AafGuiContext *ctx, AafGuiElement *self) {
    Vector2 cursor_pos = {0};
    if (self == NULL || self->as_text_input.buffer == NULL) {
        return cursor_pos;
    }
    size_t cursor_pos_index = self->as_text_input.cursor_pos;
    size_t line_start = 0;
    size_t line_end = 0;
    size_t line_count = 0;
    for (size_t i = 0; i < cursor_pos_index; ++i) {
        if (i >= self->as_text_input.buffer_size || self->as_text_input.buffer[i] == '\0') {
            break; // Prevent out-of-bounds access
        }
        if (self->as_text_input.buffer[i] == '\n') {
            line_start = i + 1;
            line_count++;
        }
    }
    line_end = line_start;
    while (line_end < self->as_text_input.buffer_size && self->as_text_input.buffer[line_end] != '\n' && self->as_text_input.buffer[line_end] != '\0') {
        line_end++;
    }
    cursor_pos.x = self->x + ctx->theme.textbox_padding + MeasureTextEx(*(Font *) ctx->font, self->as_text_input.buffer + line_start, ctx->font_size, 1).x;
    cursor_pos.y = self->y + ctx->theme.textbox_padding + ((float)line_count * ((float)(*(Font *)ctx->font).baseSize + 2));
    return cursor_pos;
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

    Vector2 cursor_pos = calculate_cursor_pos(ctx, self);

    DrawRectangleRounded(self_rect, roundness, 20, *(Color *) ctx->theme.textbox_background_color);
    if (self == ctx->focus) {
        DrawRectangleRoundedLinesEx(self_rect, roundness, 20, 2, *(Color *) ctx->theme.textbox_border_focus_color);
    } else {
        DrawRectangleRoundedLinesEx(self_rect, roundness, 20, 2, *(Color *) ctx->theme.textbox_border_color);
    }
    DrawTextEx(*(Font *) ctx->font, text, (Vector2) {self->x + ctx->theme.textbox_padding, self->y + ctx->theme.textbox_padding}, 20, 1, *(Color *) ctx->theme.text_color);

    DrawLineEx(cursor_pos, (Vector2){cursor_pos.x, cursor_pos.y + 20}, 2.0f, *(Color *) ctx->theme.text_color); // Draw cursor line
}

void update_gui_text_input(AafGuiContext *ctx) {
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
        // get the utf-8 character length of the last character
        size_t cpos = ctx->focus->as_text_input.cursor_pos;
        for (size_t i = cpos - 1; 1; --i) {
            if ((ctx->focus->as_text_input.buffer[i] & 0xc0) != 0x80) {
                // found the start of the last character
                ctx->focus->as_text_input.buffer[i] = '\0';
                ctx->focus->as_text_input.cursor_pos = i;
                break;
            }
        }
    } else if (IsKeyPressed(KEY_ENTER) || IsKeyPressedRepeat(KEY_ENTER)) {
        // submit the text input
        if (ctx->focus->as_text_input.multiline) {
            if (ctx->focus->as_text_input.cursor_pos >= ctx->focus->as_text_input.buffer_size - 1) {
                // Reallocate buffer if needed
                ctx->focus->as_text_input.buffer = realloc(ctx->focus->as_text_input.buffer, ctx->focus->as_text_input.buffer_size + 128);
                ctx->focus->as_text_input.buffer_size += 128;
            }
            ctx->focus->as_text_input.buffer[ctx->focus->as_text_input.cursor_pos] = '\n';
            ctx->focus->as_text_input.cursor_pos++;
            ctx->focus->as_text_input.buffer[ctx->focus->as_text_input.cursor_pos] = '\0';
        }
    } else if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) {
        // move the cursor left
        if (ctx->focus->as_text_input.cursor_pos > 0) {
            ctx->focus->as_text_input.cursor_pos--;
        }
    } else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) {
        // move the cursor right
        if (ctx->focus->as_text_input.cursor_pos < strlen(ctx->focus->as_text_input.buffer)) {
            ctx->focus->as_text_input.cursor_pos++;
        }
    } else {
        // add the character to the text input
        int key;
        while ((key = GetCharPressed()) != 0) {
            char utf8_char[5] = {0}; // Buffer for UTF-8 character
            size_t len = ctx->focus->as_text_input.buffer_size;
            // Encode the character to UTF-8
            if (key < 0x80) {
                utf8_char[0] = (char) key;
            } else if (key < 0x800) {
                utf8_char[0] = (char) (0xc0 | (key >> 6));
                utf8_char[1] = (char) (0x80 | (key & 0x3f));
                len += 2;
            } else {
                utf8_char[0] = (char) (0xe0 | (key >> 12));
                utf8_char[1] = (char) (0x80 | ((key >> 6) & 0x3f));
                utf8_char[2] = (char) (0x80 | (key & 0x3f));
                len += 3;
            }
            if (len >= ctx->focus->as_text_input.buffer_size) {
                // Reallocate buffer if needed
                ctx->focus->as_text_input.buffer = realloc(ctx->focus->as_text_input.buffer, len + 128);
                ctx->focus->as_text_input.buffer_size += 128;
            }
            // Insert the character at the cursor position
            size_t cursor_pos = ctx->focus->as_text_input.cursor_pos;
            memmove(ctx->focus->as_text_input.buffer + cursor_pos + strlen(utf8_char), ctx->focus->as_text_input.buffer + cursor_pos, strlen(ctx->focus->as_text_input.buffer) - cursor_pos + 1);
            memcpy(ctx->focus->as_text_input.buffer + cursor_pos, utf8_char, strlen(utf8_char));
            ctx->focus->as_text_input.cursor_pos += strlen(utf8_char);
            // Ensure the buffer is null-terminated
            ctx->focus->as_text_input.buffer[ctx->focus->as_text_input.cursor_pos] = '\0';
        }
    }
}