#include "../include/aaf.h"

#include <stdio.h>

#include <math.h>
#include <raylib.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW_MARGIN 10
#define BUTTON_PADDING 10

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
    };
    return theme;
}

void aaf_set_font(AafContext *ctx, const char *path, int size) {
    if (ctx == NULL || path == NULL) {
        fprintf(stderr, "Error: AafContext or path is NULL\n");
        return;
    }

    Font *font = malloc(sizeof(Font));
    *font = LoadFontEx(path, size * 10, NULL, 250);

    ctx->gui_context.font = font;
    ctx->gui_context.font_size = (float)size;
}

void aaf_init(AafContext *ctx, int argc, char *argv[]) {
    if (ctx == NULL) {
        fprintf(stderr, "Error: AafContext is NULL\n");
        return;
    }

    SetConfigFlags(FLAG_VSYNC_HINT);
    SetWindowState(FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "");
    SetTargetFPS(60);
    SetExitKey(0);
    SetWindowPosition(100, 100);

    ctx->should_close = false;

    ctx->gui_context = (AafGuiContext){0};
    ctx->gui_context.elements = (AafGuiElement *) malloc(sizeof(AafGuiElement) * 100);
    if (ctx->gui_context.elements == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for GUI elements\n");
        return;
    }
    ctx->gui_context.theme = aaf_gui_theme_default();

    ctx->gui_context.font = malloc(sizeof(Font));
    Font font = GetFontDefault();
    ctx->gui_context.font_size = 20;
    memcpy(ctx->gui_context.font, &font, sizeof(Font));
    ctx->gui_context.element_count = 0;
    ctx->gui_context.layout_mode = AAF_GUI_LAYOUT_COLUMNAR;

    printf("AAF context initialized successfully.\n");
}

void aaf_set_window_title(const char *title) {
    if (title == NULL) {
        fprintf(stderr, "Error: Title is NULL\n");
        return;
    }
    SetWindowTitle(title);
}

void draw_gui_label(AafContext *ctx, AafGuiElement *self) {
    if (self == NULL) {
        return;
    }

    const char *text = self->as_label.text;

    DrawTextEx(*(Font*)ctx->gui_context.font, text, (Vector2){self->x, self->y}, 20, 1, *(Color*)ctx->gui_context.theme.text_color);
}

void draw_gui_button(AafContext *ctx, AafGuiElement *self) {
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

    float roundness = ctx->gui_context.theme.corner_radius / fminf(self->w, self->h);

    if (self->event & AAF_GUI_EVENT_HOVER) {
        if (self->event & AAF_GUI_EVENT_CLICK) {
            DrawRectangleRounded(self_rect, roundness, 20, *(Color*)ctx->gui_context.theme.button_pressed_color);
        } else {
            DrawRectangleRounded(self_rect, roundness, 20, *(Color*)ctx->gui_context.theme.button_hover_color);
        }
    } else {
        DrawRectangleRounded(self_rect, roundness, 20, *(Color*)ctx->gui_context.theme.button_color);
    }
    DrawRectangleRoundedLinesEx(self_rect, roundness, 20, 2, *(Color*)ctx->gui_context.theme.button_pressed_color);
    DrawTextEx(*(Font*)ctx->gui_context.font, text, (Vector2){self->x + BUTTON_PADDING, self->y + BUTTON_PADDING}, 20, 1, *(Color*)ctx->gui_context.theme.text_color);
}

void draw_gui_text_input(AafContext *ctx, AafGuiElement *self) {
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

    float roundness = ctx->gui_context.theme.corner_radius / fminf(self->w, self->h);

    DrawRectangleRounded(self_rect, roundness, 20, *(Color*)ctx->gui_context.theme.textbox_background_color);
    if (self == ctx->gui_context.focus) {
        DrawRectangleRoundedLinesEx(self_rect, roundness, 20, 2, *(Color*)ctx->gui_context.theme.textbox_border_focus_color);
    } else {
        DrawRectangleRoundedLinesEx(self_rect, roundness, 20, 2, *(Color*)ctx->gui_context.theme.textbox_border_color);
    }
    DrawTextEx(*(Font*)ctx->gui_context.font, text, (Vector2){self->x + BUTTON_PADDING, self->y + BUTTON_PADDING}, 20, 1, *(Color*)ctx->gui_context.theme.text_color);
}

void draw_gui_elements(AafContext *ctx) {
    for (size_t i = 0; i < ctx->gui_context.element_count; ++i) {
        AafGuiElement *element = &ctx->gui_context.elements[i];
        if (element->type == GUI_LABEL) {
            draw_gui_label(ctx, element);
        } else if (element->type == GUI_BUTTON) {
            draw_gui_button(ctx, element);
        } else if (element->type == GUI_TEXT_INPUT) {
            draw_gui_text_input(ctx, element);
        }
    }
}

void update_gui_elements(AafContext *ctx) {
    if (ctx == NULL) {
        return;
    }

    // check for mouse events and update element events
    Vector2 mouse_pos = GetMousePosition();
    for (size_t i = 0; i < ctx->gui_context.element_count; ++i) {
        AafGuiElement *element = &ctx->gui_context.elements[i];
        if (element->type == GUI_BUTTON) {
            // recalculate the as_button rectangle
            // and check for mouse collision
            Vector2 text_size = MeasureTextEx(*(Font*)ctx->gui_context.font, element->as_button.text, ctx->gui_context.font_size, 1);
            element->w = text_size.x + BUTTON_PADDING * 2;
            element->h = text_size.y + BUTTON_PADDING * 2;

            if (CheckCollisionPointRec(mouse_pos, (Rectangle){element->x, element->y, element->w, element->h})) {
                element->event |= AAF_GUI_EVENT_HOVER;
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    element->event |= AAF_GUI_EVENT_CLICK;
                }
            }
        } else if (element->type == GUI_TEXT_INPUT) {
            // recalculate the as_label rectangle
            // and check for mouse collision
            Vector2 text_size = MeasureTextEx(*(Font*)ctx->gui_context.font, element->as_label.text, ctx->gui_context.font_size, 1);
            element->w = text_size.x + BUTTON_PADDING * 2;
            element->h = text_size.y + BUTTON_PADDING * 2;

            if (CheckCollisionPointRec(mouse_pos, (Rectangle){element->x, element->y, element->w, element->h})) {
                element->event |= AAF_GUI_EVENT_HOVER;
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    element->event |= AAF_GUI_EVENT_CLICK;
                    ctx->gui_context.focus = element;
                }
            } else {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    // if the mouse is clicked outside the element, remove focus
                    if (ctx->gui_context.focus == element) {
                        ctx->gui_context.focus = NULL;
                    }
                }
            }
        }
    }

    // check for keyboard events
    if (ctx->gui_context.focus != NULL) {
        if (ctx->gui_context.focus->type != GUI_TEXT_INPUT) {
            return; // TODO: Handle focus for other types, where applicable
        }

        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
            // get the utf-8 character length of the last character
            size_t len = strlen(ctx->gui_context.focus->as_text_input.buffer);
            for (size_t i = len - 1; 1; --i) {
                if ((ctx->gui_context.focus->as_text_input.buffer[i] & 0xc0) != 0x80) {
                    // found the start of the last character
                    ctx->gui_context.focus->as_text_input.buffer[i] = '\0';
                    break;
                }
            }
        } else if (IsKeyPressed(KEY_ENTER) || IsKeyPressedRepeat(KEY_ENTER)) {
            // submit the text input
            if (ctx->gui_context.focus->as_text_input.multiline) {
                // add a newline character
                size_t len = strlen(ctx->gui_context.focus->as_text_input.buffer);
                size_t new_size = len + 2; // 2 bytes for newline
                if (new_size > sizeof(ctx->gui_context.focus->as_text_input.buffer)) {
                    ctx->gui_context.focus->as_text_input.buffer = realloc(ctx->gui_context.focus->as_text_input.buffer, new_size);
                }
                ctx->gui_context.focus->as_text_input.buffer[len] = '\n';
                ctx->gui_context.focus->as_text_input.buffer[len + 1] = '\0';
            } else {
                ctx->gui_context.focus->event |= AAF_GUI_EVENT_SUBMIT;
            }
        }  else if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) {
            // move the cursor left
            if (ctx->gui_context.focus->as_text_input.cursor_pos > 0) {
                ctx->gui_context.focus->as_text_input.cursor_pos--;
            }
        } else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) {
            // move the cursor right
            if (ctx->gui_context.focus->as_text_input.cursor_pos < strlen(ctx->gui_context.focus->as_text_input.buffer)) {
                ctx->gui_context.focus->as_text_input.cursor_pos++;
            }
        } else {
            // add the character to the text input
            int key;
            while ((key = GetCharPressed()) != 0) {
                size_t len = strlen(ctx->gui_context.focus->as_text_input.buffer);
                size_t new_size = len + 4; // 4 bytes for UTF-8 encoding
                if (new_size > sizeof(ctx->gui_context.focus->as_text_input.buffer)) {
                    ctx->gui_context.focus->as_text_input.buffer = realloc(ctx->gui_context.focus->as_text_input.buffer, new_size);
                }
                // Encode the character to UTF-8
                if (key < 0x80) {
                    ctx->gui_context.focus->as_text_input.buffer[len] = (char)key;
                } else if (key < 0x800) {
                    ctx->gui_context.focus->as_text_input.buffer[len] = (char)(0xc0 | ((key >> 6) & 0x1f));
                    ctx->gui_context.focus->as_text_input.buffer[len + 1] = (char)(0x80 | (key & 0x3f));
                    len++;
                } else {
                    ctx->gui_context.focus->as_text_input.buffer[len] = (char)(0xe0 | ((key >> 12) & 0x0f));
                    ctx->gui_context.focus->as_text_input.buffer[len + 1] = (char)(0x80 | ((key >> 6) & 0x3f));
                    ctx->gui_context.focus->as_text_input.buffer[len + 2] = (char)(0x80 | (key & 0x3f));
                    len += 2;
                }
                ctx->gui_context.focus->as_text_input.buffer[len + 1] = '\0';
            }
        }
    }
}

void reset_gui_elements(AafContext *ctx) {
    if (ctx == NULL) {
        return;
    }

    for (size_t i = 0; i < ctx->gui_context.element_count; ++i) {
        AafGuiElement *element = &ctx->gui_context.elements[i];
        element->event = 0;
    }
}

void calculate_layout(AafContext *ctx) { // TODO: Make more smarter
    for (size_t i = 0; i < ctx->gui_context.element_count; ++i) {
        AafGuiElement *prev_element = (i > 0) ? &ctx->gui_context.elements[i - 1] : NULL;
        AafGuiElement *element = &ctx->gui_context.elements[i];

        if (prev_element != NULL) {
            if (ctx->gui_context.layout_mode == AAF_GUI_LAYOUT_COLUMNAR) {
                element->x = prev_element->x;
                element->y = prev_element->y + prev_element->h + WINDOW_MARGIN;
            } else if (ctx->gui_context.layout_mode == AAF_GUI_LAYOUT_ROW) {
                element->x = prev_element->x + prev_element->w + WINDOW_MARGIN;
                element->y = prev_element->y;
            } else {
                element->x = WINDOW_MARGIN;
                element->y = WINDOW_MARGIN;
            }
        } else {
            element->x = WINDOW_MARGIN;
            element->y = WINDOW_MARGIN;
        }
    }
}

void aaf_begin(AafContext *ctx) {
    if (ctx == NULL) {
        fprintf(stderr, "Error: AafContext is NULL\n");
        return;
    }

    update_gui_elements(ctx);
    calculate_layout(ctx);

    BeginDrawing();
    ClearBackground(RAYWHITE);
}

void aaf_end(AafContext *ctx) {
    if (ctx == NULL) {
        fprintf(stderr, "Error: AafContext is NULL\n");
        return;
    }

    draw_gui_elements(ctx);

    EndDrawing();

    reset_gui_elements(ctx);

    if (WindowShouldClose()) {
        ctx->should_close = true;
    }
}

void realloc_gui_elements_if_needed(AafContext *ctx) {
    if (ctx->gui_context.element_count >= 100) {
        size_t new_size = ctx->gui_context.element_count * 2;
        AafGuiElement *new_elements = (AafGuiElement *) realloc(ctx->gui_context.elements, sizeof(AafGuiElement) * new_size);
        if (new_elements == NULL) {
            fprintf(stderr, "Error: Failed to reallocate memory for GUI elements\n");
            return;
        }
        ctx->gui_context.elements = new_elements;
    }
}

AafGuiElement *aaf_gui_label(AafContext *ctx, const char *text, int x, int y) {
    if (ctx == NULL) {
        return NULL;
    }

    Vector2 wh = MeasureTextEx(GetFontDefault(), text, 20, 1);

    AafGuiLabel label = {text};
    AafGuiElement element = {GUI_LABEL, .as_label = label, .w = wh.x, .h = wh.y};

    realloc_gui_elements_if_needed(ctx);

    AafGuiElement *new_element = &ctx->gui_context.elements[ctx->gui_context.element_count];
    *new_element = element;
    ctx->gui_context.element_count += 1;
    return new_element;
}

AafGuiElement *aaf_gui_button(AafContext *ctx, const char *text, int x, int y) {
    if (ctx == NULL) {
        return NULL;
    }

    Vector2 wh = MeasureTextEx(*(Font*)ctx->gui_context.font, text, 20, 1);

    AafGuiButton button = {text};
    AafGuiElement element = {GUI_BUTTON, .as_button = button, .w = wh.x + BUTTON_PADDING * 2, .h = wh.y + BUTTON_PADDING * 2};

    realloc_gui_elements_if_needed(ctx);

    AafGuiElement *new_element = &ctx->gui_context.elements[ctx->gui_context.element_count];
    *new_element = element;
    ctx->gui_context.element_count += 1;

    return new_element;
}

AafGuiElement *aaf_gui_text_input(AafContext *ctx, const char *text, bool multiline, int x, int y, float w, float h) {
    if (ctx == NULL) {
        return NULL;
    }

    AafGuiTextInput text_input = {
        .buffer = malloc(sizeof(char) * (strlen(text) + 1)),
        .buffer_size = strlen(text) + 1,
        .cursor_pos = 0,
        .multiline = multiline,
    };
    AafGuiElement element = {GUI_TEXT_INPUT, .as_text_input = text_input, .w = w + BUTTON_PADDING * 2, .h = h + BUTTON_PADDING * 2};
    strcpy(element.as_text_input.buffer, text);

    realloc_gui_elements_if_needed(ctx);

    AafGuiElement *new_element = &ctx->gui_context.elements[ctx->gui_context.element_count];
    *new_element = element;
    ctx->gui_context.element_count += 1;

    return new_element;
}