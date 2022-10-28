#include "kaimen.h"

namespace K {
    uint16_t y_offset;
    uint8_t curr_col;
    vec draw_idx;
    Layout curr_layout;
    ctx ctx_stack[MAX_CTX_STACK];
    uint8_t curr_ctx = -1;
    bool confirmed;
    bool blink;
    bool some_fit;
    uint8_t max_curr;

    int open(int (*next)()) {
        int res = -1;
        curr_ctx += 1;
        uint32_t blink_ms = millis(), curr_ms;

        if (curr_ctx == MAX_CTX_STACK) {
            // TODO(Marce): Error screen
            goto exit;
        }

        // Initialize the cursor to the first item
        ctx_stack[curr_ctx].idx.x = 0;
        ctx_stack[curr_ctx].idx.y = 0;

        res = 0;
        blink = false;

        while(!mp.update());
        // IMPROVEMENT(Marce): Only re-render on input change or
        // on interrupt (timer, etc)
        while (1) {
            mp.update();

            some_fit = false;
            confirmed = false;
            curr_layout = Layout::ONE_COLUMN;
            curr_col = 0;
            draw_idx.x = 0;
            draw_idx.y = 0;
            y_offset = 0;
            max_curr = 0;
            curr_ms = millis();

            if (curr_ms - blink_ms > 350) {
                blink = !blink;
                blink_ms = curr_ms;
            }

            if (mp.buttons.released(BTN_A)) {
                confirmed = true;
            }

            if (mp.buttons.pressed(BTN_UP)) {
                ctx_stack[curr_ctx].idx.y -= 1;
            }

            if (mp.buttons.pressed(BTN_DOWN)) {
                ctx_stack[curr_ctx].idx.y += 1;
            }

            if (mp.buttons.pressed(BTN_RIGHT)) {
                ctx_stack[curr_ctx].idx.x += 1;
            }

            if (mp.buttons.pressed(BTN_LEFT)) {
                ctx_stack[curr_ctx].idx.x -= 1;
            }

            if (mp.buttons.released(BTN_HOME)) {
                mp.exitedLockscreen = true;
                mp.lockscreen();
            }

            clear_screen();

            res = next();

            if (ctx_stack[curr_ctx].idx.y >= max_curr) {
                ctx_stack[curr_ctx].idx.y = 0;
            } else if (ctx_stack[curr_ctx].idx.y < 0) {
                ctx_stack[curr_ctx].idx.y = max_curr - 1;
            }

            if (res != 0 || mp.buttons.released(BTN_B)) {
                while(!mp.update());
                goto exit;
            }
        }

    exit:
        // This may underflow, should be fine since at the start of open
        // we increment it again, which will overflow to 0
        curr_ctx -= 1;
        return res;
    }

    void clear_screen() {
        mp.display.fillScreen(TFT_BLACK);
    }

    void set_layout(Layout l) {
        curr_layout = l;
    }

    /*
     * Gives you the bounds of the next draw space so
     * each widget doesn't have to compute these by itself.
     */
    void get_layout_space(region *r, region_hints *h) {
        r->y = y_offset;

        if (curr_layout == Layout::ONE_COLUMN) {
            // TODO(Marce): use max width system to specify center stuff
            curr_col = 0;
            r->x = 0;
            r->w = SCREEN_WIDTH;
            y_offset += BLOCK_HEIGHT;
        } else if (curr_layout == Layout::TWO_COLUMNS) {
            if (curr_col == 0) {
                r->x = 0;
            } else if (curr_col == 1) {
                r->x = SCREEN_WIDTH / 2;
            }

            r->w = SCREEN_WIDTH / 2;
            curr_col = (curr_col + 1) % 2;

            if (curr_col == 0) {
                y_offset += BLOCK_HEIGHT;
            }
        }

        r->h = BLOCK_HEIGHT;
    }

    void advance_idx() {
        if (curr_layout == Layout::ONE_COLUMN) {
            draw_idx.x = 0;
            draw_idx.y += 1;
        } else if (curr_layout == Layout::TWO_COLUMNS) {
            draw_idx.x = curr_col;

            if (curr_col == 1) {
                draw_idx.y += 1;
            }
        }

        if (draw_idx.y > max_curr) {
            max_curr = draw_idx.y;
        }
    }

    bool button(char *text, Color bgc, int fgc) {
        bool res = false;

        region r;
        get_layout_space(&r, NULL);

        mp.display.fillRect(r.x + 1, r.y + 1, r.w - 2, r.h - 2, bgc);
        mp.display.setTextColor(fgc);
        mp.display.setTextSize(1);
        mp.display.setTextFont(2);
        mp.display.drawString(text, r.x, r.y + 2);

        if (ctx_stack[curr_ctx].idx.y == draw_idx.y &&
            (curr_layout == Layout::ONE_COLUMN ||
             (curr_layout == Layout::TWO_COLUMNS && ctx_stack[curr_ctx].idx.x == draw_idx.x)
            )
           ) {
            some_fit = true;
            if (blink) {
                mp.display.drawRect(r.x, r.y, r.w, r.h, TFT_RED);
                mp.display.drawRect(r.x+1, r.y+1, r.w-2, r.h-2, TFT_RED);
            }
            res = confirmed;
        }

        advance_idx();
        return res;
    }

    void text(char *text) {
        region r;
        get_layout_space(&r, NULL);

        mp.display.setTextColor(TFT_WHITE);
        mp.display.setTextSize(1);
        mp.display.setTextFont(2);
        mp.display.drawString(text, 30, r.y + 2);
    }
}
