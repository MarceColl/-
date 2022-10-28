#ifndef KAIMEN_H_
#define KAIMEN_H_

#include "main.h"

#define MAX_CTX_STACK 5

#define TODO ASDF
#define SET_BIT(B, V) TODO
#define GET_BIT(B, V) TODO

#define BLOCK_HEIGHT 20
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 128

namespace K {
    enum Layout {
        ONE_COLUMN,
        TWO_COLUMNS,
    };

    enum Align {
        LEFT,
        CENTER,
        RIGHT,
    };

    struct vec {
        int8_t x;
        int8_t y;
    };

    struct ctx {
        /*
         * Index for the cursor
         */
        vec idx;
    };

    struct region {
        int8_t x;
        int8_t y;
        uint8_t w;
        uint8_t h;
    };

    struct region_hints {
        uint8_t w; // if 0, automatic
        uint8_t h; // if 0, automatic
    };

    enum Color {
        TRANSPARENT = 0x0,
        RED = 0xFB6D,
        ORANGE = 0xFD29,
        PURPLE = 0xED1F,
        GREEN = 0x8FEA,
        YELLOW = 0xFFED,
        BLUE = 0xA7FF,
    };

    /**
     * Open the given window defined in a given function pointer.
     */
    int open(int (*next)());

    /*
     * Shows the error screen with the given text.
     */
    void error_screen(char *text);

    /*
     * Move selection idx
     */
    void move_right();
    void move_left();
    void move_down();
    void move_up();

    void set_layout(Layout l);

    /**
     * Go back one stack
     */
    void back();

    /**
     * Start a screen, this clears the background and handles input.
     */
    void screen();
    void set_spacing(uint8_t s);

    void text(char *str);

    /**
    * Draw a button with the given text and given color.
    *
    * Returns true if it was pressed.
    */
    bool button(char *text, Color bgc, int fgc);

    /*
     * Given the current layout settings return the region
     * where the next item will be drawn.
     */
    void get_layout_space(region *r, region_hints *h);

    void advance_idx();
    void clear_screen();
}

#endif // KAIMEN_H_
