//
// Created by Robert on 24/05/2025.
//

#ifndef GUI_TEXT_INPUT_H
#define GUI_TEXT_INPUT_H
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    char *buffer;
    size_t buffer_size;
    size_t cursor_pos;
    bool multiline;
} AafGuiTextInput;

#endif //GUI_TEXT_INPUT_H
