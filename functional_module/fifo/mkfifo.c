#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>
#include <string.h>
// #include "debug.h"
int mkfifo_fd;
int mkfifo_init(void)
{
    int ret,len;
    ret = access("mkfifo_file.txt",F_OK);
    if(ret == -1)
    {
        ret = mkfifo("mkfifo_file.txt",0644);
        if(ret ==-1)
        {
            perror("mkfifo error"); 
            return -1;
        }
    }

    ret = access("mkfifo_file.txt",R_OK);
    if(ret ==-1)
    {
        perror("access error");
        return -1;
    }

    mkfifo_fd = open("mkfifo_file.txt",O_RDONLY|O_NONBLOCK );
    
    return 0;
}


void read_mkfifo(char *buf ,uint8_t *len)
{
    *len = read(mkfifo_fd,buf,128);
    if(*len ==-1)
    {
        perror("read error");
        return ;
    }
    else if(*len ==  0)
    {
        // BASE_PRINTF("no mkfifo data \r\n");
    }
    else
    {
        // BASE_PRINTF("buf size %d %s ",*len,buf);
    }
    
}


/***********************************************************
int write_mkfifo(char *buf)
{
    int fd;
    int ret;

    ret = access("mkfifo_file.txt",W_OK);
    if(ret ==-1)
    {
        perror("access error");
        return -1;
    }

    fd = open("mkfifo_file.txt",O_WRONLY);

    if(fd ==-1)
    {
        perror("open error"); 
        return -1;   
    }

    printf("write data : %s\n", buf);
    write(fd, buf, strlen(buf)+1);
     
    close(fd);


    return 0;
}
**********************************************************/


void mkfifo_deinit(void)
{
	close(mkfifo_fd);
}