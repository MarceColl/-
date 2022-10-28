#include "debug.h"
#include "kaimen.h"

#ifdef DEBUG
uint8_t buffer_ptr = 0;
char debug_log[DEBUG_LOG_BUFFER_SIZE][DEBUG_LOG_SIZE];

uint8_t offset = 0;

int dbg_screen() {
    K::set_spacing(8);
    for (int i = 0; i < 17; i++) {
        K::text(debug_log[buffer_ptr - offset - i - 1]);
    }

    K::set_spacing(BLOCK_HEIGHT);

    return 0;
}
#endif

void dbg(char *format, ...) {
#ifdef DEBUG
    va_list args;
    va_start(args, format);
    vsprintf(debug_log[buffer_ptr], format, args);
    va_end(args);

    buffer_ptr = (buffer_ptr + 1) % DEBUG_LOG_BUFFER_SIZE;
#endif
}
