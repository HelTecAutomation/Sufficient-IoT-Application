# mkfifo instructions for use.
1. First enter the graphical configuration interface and select the following options.
>- mkfifo
2. Modify the macro definition according to the location of the file mkfifo_file.txt:
>- #define MKFIFO_ADDRESS   "/home/lora/bin/mkfifo_file.txt"
3. Compile the following content into an executable file to send data to lora's to-be-sent buffer.

```c
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#define MKFIFO_ADDRESS  "/home/lora/bin/mkfifo_file.txt"
int write_mkfifo(char *buf);

int main(int argc,char *argv[])
{

	if(argc==2)
	{
		 write_mkfifo(argv[1]);
	}
	else
	{
		write_mkfifo("9988776655");
	}
	return 0;
}


int write_mkfifo(char *buf)
{
    int fd;
    int ret;

    ret = access(MKFIFO_ADDRESS,W_OK);
    if(ret ==-1)
   	{
        perror("access error");
        return -1;
    }

    fd = open(MKFIFO_ADDRESS,O_WRONLY);
 	printf("open success\n");
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
```
