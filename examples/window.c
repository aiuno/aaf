//
// Created by Robert Moen Hafsten on 21/05/2025.
//

#include <aaf.h>
#include <stddef.h>
#include <stdio.h>

int main() {
    AafContext ctx;
    aaf_init(&ctx, 0, NULL);
    aaf_set_window_title("Hello, AAF!");
    aaf_set_font(&ctx, "assets/Roboto-Regular.ttf", 20);

    AafGuiElement *label = aaf_gui_label(&ctx, "Hello, AAF!", -1, -1);
    AafGuiElement *button = aaf_gui_button(&ctx, "Click Me!", -1, -1);
    AafGuiElement *text_input = aaf_gui_text_input(&ctx, "Type here... ÆØÅ", true, -1, -1, 200, 500);

    while (!ctx.should_close) {
        aaf_begin(&ctx);

        if (button->event & AAF_GUI_EVENT_CLICK) {
            label->as_label.text = "Made you click!";
        }

        aaf_end(&ctx);
    }

    return 0;
}
