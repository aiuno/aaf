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

    aaf_gui_realloc_elements_if_needed(ctx);

    AafGuiElement *new_element = &ctx->elements[ctx->element_count];
    *new_element = element;
    ctx->element_count += 1;

    aaf_calculate_layout(ctx);

    return new_element;
}

Vector2 calculate_cursor_pos(AafGuiContext *ctx, AafGuiElement *self) {
    Vector2 cursor_pos = {0};
    if (self == NULL || self->as_text_input.buffer == NULL) {
        return cursor_pos;
    }
    size_t cursor_pos_index = self->as_text_input.cursor_pos;
    size_t line_start = 0;
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
    size_t line_end = line_start;
    while (line_end < self->as_text_input.buffer_size && self->as_text_input.buffer[line_end] != '\n' && self->as_text_input.buffer[line_end] != '\0') {
        line_end++;
    }
    char text_up_to_cursor[1024] = {0};
    strncpy(text_up_to_cursor, self->as_text_input.buffer + line_start, cursor_pos_index - line_start);
    text_up_to_cursor[cursor_pos_index] = '\0'; // Null-terminate the string
    Vector2 text_size = MeasureTextEx(*(Font *) ctx->font, text_up_to_cursor, 20, 1);
    cursor_pos.x = self->x + ctx->theme.textbox_padding + text_size.x;
    cursor_pos.y = self->y + ctx->theme.textbox_padding + (float)(line_count * (ctx->font_size + 2)); // Assuming 20 is the line height
    return cursor_pos;
}

void draw_cursor(AafGuiContext *ctx, AafGuiElement *self) {
    if (self == NULL || self->as_text_input.buffer == NULL) {
        return;
    }

    Vector2 cursor_pos = calculate_cursor_pos(ctx, self);
    Color cursor_color = (Color) {
            (*(Color *) ctx->theme.text_color).r,
            (*(Color *) ctx->theme.text_color).g,
            (*(Color *) ctx->theme.text_color).b,
            self->as_text_input.cursor_anim_step > 0.5f ? 255 : 0 // Blink effect
    };
    DrawLineEx(cursor_pos, (Vector2){cursor_pos.x, cursor_pos.y + 20}, 2.0f, cursor_color); // Draw cursor line
}

void aaf_draw_gui_text_input(AafGuiContext *ctx, AafGuiElement *self) {
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

    DrawRectangleRounded(self_rect, roundness, 20, *(Color *) ctx->theme.textbox_background_color);
    if (self == ctx->focus) {
        DrawRectangleRoundedLinesEx(self_rect, roundness, 20, 2, *(Color *) ctx->theme.textbox_border_focus_color);
    } else {
        DrawRectangleRoundedLinesEx(self_rect, roundness, 20, 2, *(Color *) ctx->theme.textbox_border_color);
    }

    DrawTextEx(*(Font *) ctx->font, text, (Vector2) {self->x + ctx->theme.textbox_padding, self->y + ctx->theme.textbox_padding}, 20, 1, *(Color *) ctx->theme.text_color);

    if (self == ctx->focus) {
        draw_cursor(ctx, self); // Draw cursor only if this element is focused
    }
}

size_t get_char_width(const char *buffer, size_t pos) {
    if (buffer == NULL || pos == 0) {
        return 0;
    }
    // Find the width of the UTF-8 character at the given position
    size_t i = pos - 1;
    while (i > 0 && (buffer[i] & 0xc0) == 0x80) {
        i--; // Move back until we find the start of the character
    }
    return pos - i; // Return the length of the character
}

size_t get_char_width_forward(const char *buffer, size_t pos) {
    if (buffer == NULL || pos >= strlen(buffer)) {
        return 0;
    }
    // Find the width of the UTF-8 character at the given position
    size_t i = pos + 1;
    while (i < strlen(buffer) && (buffer[i] & 0xc0) == 0x80) {
        i++; // Move forward until we find the end of the character
    }
    return i - pos; // Return the length of the character
}

void aaf_update_gui_text_input(AafGuiContext *ctx, AafGuiElement *self) {
    if (self == NULL || self->type != GUI_TEXT_INPUT) {
        return;
    }

    if (self != ctx->focus) {
        return; // Only update if this element is focused
    }

    // Update cursor animation step for blinking effect
    self->as_text_input.cursor_anim_step += GetFrameTime();
    if (self->as_text_input.cursor_anim_step >= 1.0f) {
        self->as_text_input.cursor_anim_step = 0.0f; // Reset animation step
    }

    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
        // get the utf-8 character length of the last character
        if (self->as_text_input.cursor_pos > 0) {
            size_t char_length = get_char_width(self->as_text_input.buffer, self->as_text_input.cursor_pos);
            if (char_length > 0) {
                // Move cursor back by the character length
                self->as_text_input.cursor_pos -= char_length;
                if (self->as_text_input.cursor_pos < 0) {
                    self->as_text_input.cursor_pos = 0; // Prevent negative cursor position
                }
                // Remove the character from the buffer
                memmove(self->as_text_input.buffer + self->as_text_input.cursor_pos,
                        self->as_text_input.buffer + self->as_text_input.cursor_pos + char_length,
                        strlen(self->as_text_input.buffer) - self->as_text_input.cursor_pos - char_length + 1); // +1 for null terminator
                self->event |= AAF_GUI_EVENT_CHANGED; // Mark as changed
            }
        }
    } else if (IsKeyPressed(KEY_ENTER) || IsKeyPressedRepeat(KEY_ENTER)) {
        // submit the text input
        if (self->as_text_input.multiline) {
            if (self->as_text_input.cursor_pos >= self->as_text_input.buffer_size - 1) {
                // Reallocate buffer if needed
                self->as_text_input.buffer = realloc(self->as_text_input.buffer, self->as_text_input.buffer_size + 128);
            }
            // Insert a newline character at the cursor position
            memmove(self->as_text_input.buffer + self->as_text_input.cursor_pos + 1,
                    self->as_text_input.buffer + self->as_text_input.cursor_pos,
                    strlen(self->as_text_input.buffer) - self->as_text_input.cursor_pos);
            self->as_text_input.buffer[self->as_text_input.cursor_pos] = '\n'; // Insert newline
            self->as_text_input.cursor_pos += 1; // Move cursor forward
            self->as_text_input.buffer_size = strlen(self->as_text_input.buffer) + 1; // Update buffer size
            self->event |= AAF_GUI_EVENT_CHANGED; // Mark as changed
        }
    } else if (IsKeyPressed(KEY_ESCAPE)) {
        self->as_text_input.selection_start = 0;
        self->as_text_input.selection_end = 0; // Clear selection
    } else if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) {
        // move the cursor left
        if (self->as_text_input.cursor_pos > 0) {
            size_t prev_char_length = get_char_width(self->as_text_input.buffer, self->as_text_input.cursor_pos);
            if (self->as_text_input.cursor_pos >= prev_char_length) {
                self->as_text_input.cursor_pos -= prev_char_length; // Move cursor back by the character length
            } else {
                self->as_text_input.cursor_pos = 0; // Prevent going out of bounds
            }
        }
    } else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) {
        // move the cursor right
        size_t next_char_length = get_char_width_forward(self->as_text_input.buffer, self->as_text_input.cursor_pos);
        if (self->as_text_input.cursor_pos + next_char_length < self->as_text_input.buffer_size) {
            self->as_text_input.cursor_pos += next_char_length; // Move cursor forward by the character length
        } else {
            self->as_text_input.cursor_pos = self->as_text_input.buffer_size - 1; // Prevent going out of bounds
        }
    } else {
        // add the character to the text input
        int key;
        while ((key = GetCharPressed()) != 0) {
            char utf8_char[5] = {0}; // Buffer for UTF-8 character
            size_t len = self->as_text_input.buffer_size;
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
            if (len >= self->as_text_input.buffer_size) {
                // Reallocate buffer if needed
                self->as_text_input.buffer = realloc(self->as_text_input.buffer, len + 128);
            }
            // Insert the character at the cursor position
            if (self->as_text_input.cursor_pos >= self->as_text_input.buffer_size - 1) {
                self->as_text_input.buffer[self->as_text_input.cursor_pos] = '\0'; // Null-terminate the string
            }
            memmove(self->as_text_input.buffer + self->as_text_input.cursor_pos + strlen(utf8_char),
                    self->as_text_input.buffer + self->as_text_input.cursor_pos,
                    strlen(self->as_text_input.buffer) - self->as_text_input.cursor_pos); // Move the rest of the string
            strncpy(self->as_text_input.buffer + self->as_text_input.cursor_pos, utf8_char, strlen(utf8_char)); // Insert the new character
            self->as_text_input.cursor_pos += strlen(utf8_char); // Move cursor forward
            self->as_text_input.buffer_size = strlen(self->as_text_input.buffer) + 1; // Update buffer size
            self->event |= AAF_GUI_EVENT_CHANGED; // Mark as changed
        }
    }
}