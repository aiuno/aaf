#ifndef AAF_C_AAF_H
#define AAF_C_AAF_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
    GUI_LABEL,
    GUI_BUTTON,
    GUI_TEXT_INPUT,
} AafGuiElementType;

typedef struct {
    const char *text;
} AafGuiLabel;

typedef struct {
    const char *text;
} AafGuiButton;

typedef struct {
    char *buffer;
    size_t buffer_size;
    size_t cursor_pos;
    bool multiline;
} AafGuiTextInput;

typedef enum {
    AAF_GUI_EVENT_CLICK = 1 << 1,
    AAF_GUI_EVENT_HOVER = 1 << 2,
    AAF_GUI_EVENT_SUBMIT = 1 << 3,
} AafGuiElementEvent;

typedef enum {
    AAF_GUI_LAYOUT_COLUMNAR,
    AAF_GUI_LAYOUT_ROW,
} AafGuiLayoutMode;

typedef struct {
    AafGuiElementType type;
    AafGuiElementEvent event;
    float x, y, w, h;
    union {
        AafGuiLabel as_label;
        AafGuiButton as_button;
        AafGuiTextInput as_text_input;
    };
} AafGuiElement;

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
} AafGuiTheme;

typedef struct {
    AafGuiElement *elements;
    size_t element_count;
    float font_size;
    void *font;
    AafGuiElement *focus; // TODO: Focus for text input etc. when implemented
    AafGuiTheme theme;
    AafGuiLayoutMode layout_mode;
} AafGuiContext;

typedef struct {
    bool should_close;
    AafGuiContext gui_context;
} AafContext;

void aaf_init(AafContext *ctx, int argc, char *argv[]);
void aaf_begin(AafContext *ctx);
void aaf_end(AafContext *ctx);

void aaf_set_window_title(const char *title);
void aaf_set_font(AafContext *ctx, const char *path, int size);

AafGuiElement *aaf_gui_label(AafContext *ctx, const char *text, int x, int y);
AafGuiElement *aaf_gui_button(AafContext *ctx, const char *text, int x, int y);
AafGuiElement *aaf_gui_text_input(AafContext *ctx, const char *text, bool multiline, int x, int y, float w, float h);

AafGuiTheme aaf_gui_theme_default();

#endif//AAF_C_AAF_H
