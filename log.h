#ifndef __LOG_H
#define __LOG_H

extern int log_error(unsigned char *error_msg);
extern int init_log();
extern int statistics(unsigned char *type, int number);
#endif
