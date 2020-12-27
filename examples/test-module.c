#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

#define MODULE_LOCATION "/proc/stt_mod"
#define BUFF_LEN 100

int main()
{
  int fd;
  char buffer[BUFF_LEN];
  int read_count, write_count;
  int len;
  int pid;
  int seconds;

  printf("Enter the process id:\n");
  scanf("%d", &pid);

  printf("Enter seconds:\n");
  scanf("%d", &seconds);

  fd = open(MODULE_LOCATION, O_WRONLY);

  if (fd < 0)
  {
    printf("Error opening pid file descriptor\n");
    printf("error code : %d\n", fd);
    return -1;
  }

  sprintf(buffer, "%d %d", pid, seconds);
  printf("%s\n", buffer);
  len = strlen(buffer);

  write_count = write(fd, buffer, len);
  if (write_count > 0)
  {
    printf("Wrote to pid -> %s\n", buffer);
  }
  else
  {
    printf("Couldnt print to pid location\n");
    return -1;
  }
  close(fd);

  fd = open(MODULE_LOCATION, O_RDONLY);
  if (fd < 0)
  {
    printf("Error opening module file descriptor\n");
  }

  read_count = read(fd, NULL, 0);
  close(fd);

  return 0;
}