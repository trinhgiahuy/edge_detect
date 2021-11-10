#include <stdio.h>
#define TIMER (*((volatile unsigned long *) 0x600A0000))

int main()
{
  unsigned long         start;
  unsigned long         end;

  start = TIMER;

  some_big_function():

  end = TIMER;

  printf("function elapsed clocks: %d \n", end - start);
}

