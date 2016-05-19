#ifndef DEBUG_H
#define DEBUG_H
#include <stdarg.h>

void show_backtrace(void);
void backtrace(void);
void handler(int sig);
void real_dbgprint(int count, const char * format, ...);
#define DEBUG_PRINT true
struct dbg_notice_cnt {};
struct dbg_warning_cnt {};
#define dbgprint(...) \
	if (DEBUG_PRINT) \
		real_dbgprint(__COUNTER__, "GN", __VA_ARGS__); \
		//COUNTER_INC(dbg_notice_cnt);

void real_dbgprint(int counter, const char* msg_type, const char* format, ...);
#endif