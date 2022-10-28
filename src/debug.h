#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

#define DEBUG_LOG_BUFFER_SIZE 50
#define DEBUG_LOG_SIZE 50

void dbg(char *format, ...);

#ifdef DEBUG
int dbg_screen();
#endif

#endif // DEBUG_H_
