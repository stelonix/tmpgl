#include <stdarg.h>

void show_backtrace(void);
void backtrace(void);
void handler(int sig);

#define DEBUG_PRINT false

#define dbgprint(...) if (DEBUG_PRINT) printf(__VA_ARGS__)