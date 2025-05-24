#ifndef AAF_C_AAF_H
#define AAF_C_AAF_H

#include <stdbool.h>

#include "gui.h"

typedef struct {
    bool should_close;
    AafGuiContext gui_context;
} AafContext;

void aaf_init(AafContext *ctx, int argc, char *argv[]);
void aaf_begin(AafContext *ctx);
void aaf_end(AafContext *ctx);

void aaf_set_window_title(const char *title);

#endif//AAF_C_AAF_H
