
#ifndef EC20_H
#define EC20_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <memory.h>
#include <malloc.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>        
#include <termios.h>        

#ifndef POLLRDHUP
#define POLLRDHUP       0x2000
#endif

#ifndef boolean
#define boolean unsigned int
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define MAXLINE 1024	//一次读取最大的字节数
#define BIGNUMBER 100*1024	//一次读取最大的字节数

int Ql_SendAT(char* atCmd, char* finalRsp, long timeout_ms);
int Ql_SendAT2(char* atCmd, char* finalRsp, long timeout_ms, int should_Add);
int Ql_SendAT3(char* atCmd, char* finalRsp, long timeout_ms, int should_Add);

int Module_Test(void);
// int Upload_Image(char *image_path);
int Upload_Image_TCP3(char *image_path);
int Upload_Image_Info(int class);
int HTTP_Config(void);
int GPS_Config(void);

extern int smd_fd;

#endif