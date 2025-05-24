//
// Created by Robert on 24/05/2025.
//

#ifndef GUI_ELEMENT_H
#define GUI_ELEMENT_H

#include "gui_label.h"
#include "gui_button.h"
#include "gui_text_input.h"

typedef enum {
    GUI_LABEL,
    GUI_BUTTON,
    GUI_TEXT_INPUT,
} AafGuiElementType;

typedef enum {
    AAF_GUI_EVENT_CLICK = 1 << 1,
    AAF_GUI_EVENT_HOVER = 1 << 2,
    AAF_GUI_EVENT_SUBMIT = 1 << 3,
} AafGuiElementEvent;

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

#endif //GUI_ELEMENT_H
