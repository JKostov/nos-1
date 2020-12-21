#include<stdio.h>
#include<linux/kernel.h>
#include<sys/syscall.h>
#include<unistd.h>
#include<stdlib.h>

int main()
{
  int pid;
  int seconds;

  printf("Enter the process id:\n");
  scanf("%d", &pid);

  printf("Enter seconds:\n");
  scanf("%d", &seconds);

  long int amma = syscall(437, pid, seconds);
  printf("Sys call returned %ld\n", amma);
  return 0;
}