#ifndef _MKFIFO_H_
#define _MKFIFO_H_

int mkfifo_init(void);
void read_mkfifo(char *buf ,uint8_t *len);

void mkfifo_deinit(void);





#endif