#include "../include/aaf.h"

#include <stdio.h>

#include <math.h>
#include <raylib.h>
#include <stdlib.h>
#include <string.h>

void aaf_init(AafContext *ctx, int argc, char *argv[]) {
    if (ctx == NULL) {
        fprintf(stderr, "Error: AafContext is NULL\n");
        return;
    }

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(1280, 720, "");
    SetTargetFPS(60);
    SetExitKey(0);
    SetWindowPosition(100, 100);

    ctx->monitor = GetCurrentMonitor();

    ctx->accumulator = 0.0;
    ctx->update_rate = 1.0 / 60.0; // 60 FPS update rate

    ctx->should_close = false;

    ctx->gui_context = aaf_gui_context_create();

    printf("AAF context initialized successfully.\n");
}

void aaf_set_window_title(const char *title) {
    if (title == NULL) {
        fprintf(stderr, "Error: Title is NULL\n");
        return;
    }
    SetWindowTitle(title);
}

void draw_gui_elements(AafContext *ctx) {
    for (size_t i = 0; i < ctx->gui_context.element_count; ++i) {
        AafGuiElement *element = &ctx->gui_context.elements[i];
        if (element->type == GUI_LABEL) {
            aaf_gui_draw_label(&ctx->gui_context, element);
        } else if (element->type == GUI_BUTTON) {
            aaf_gui_draw_button(&ctx->gui_context, element);
        } else if (element->type == GUI_TEXT_INPUT) {
            aaf_gui_draw_text_input(&ctx->gui_context, element);
        }
    }
}

void update_gui_elements(AafGuiContext *ctx) {
    if (ctx == NULL) {
        return;
    }

    // check for mouse events and update element events
    Vector2 mouse_pos = GetMousePosition();
    for (size_t i = 0; i < ctx->element_count; ++i) {
        AafGuiElement *element = &ctx->elements[i];
        if (element->type == GUI_BUTTON) {
            // recalculate the as_button rectangle
            // and check for mouse collision
            Vector2 text_size = MeasureTextEx(*(Font *) ctx->font, element->as_button.text, ctx->font_size, 1);
            element->w = text_size.x + ctx->theme.button_padding * 2;
            element->h = text_size.y + ctx->theme.button_padding * 2;

            if (CheckCollisionPointRec(mouse_pos, (Rectangle) {element->x, element->y, element->w, element->h})) {
                element->event |= AAF_GUI_EVENT_HOVER;
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    element->event |= AAF_GUI_EVENT_CLICK;
                }
            }
        } else if (element->type == GUI_TEXT_INPUT) {
            // recalculate the as_label rectangle
            // and check for mouse collision
            if (CheckCollisionPointRec(mouse_pos, (Rectangle) {element->x, element->y, element->w, element->h})) {
                element->event |= AAF_GUI_EVENT_HOVER;
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    element->event |= AAF_GUI_EVENT_CLICK;
                    ctx->focus = element;
                }
            } else {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    // if the mouse is clicked outside the element, remove focus
                    if (ctx->focus == element) {
                        ctx->focus = NULL;
                    }
                }
            }
        }
    }

    // check for keyboard events
    if (ctx->focus != NULL) {
        if (ctx->focus->type != GUI_TEXT_INPUT) {
            return;// TODO: Handle focus for other types, where applicable
        }

        aaf_gui_update_text_input(ctx, ctx->focus);
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

void aaf_begin(AafContext *ctx) {
    if (ctx == NULL) {
        fprintf(stderr, "Error: AafContext is NULL\n");
        return;
    }

    if (ctx->monitor != GetCurrentMonitor()) {
        ctx->monitor = GetCurrentMonitor();
        aaf_gui_reload_font(&ctx->gui_context);
    }

    if (IsWindowResized()) {
        aaf_calculate_layout(&ctx->gui_context);
    }
    ctx->accumulator += GetFrameTime();

    if (ctx->accumulator >= ctx->update_rate) {
        ctx->accumulator -= ctx->update_rate;
        // Update GUI elements
        update_gui_elements(&ctx->gui_context);
    }

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
