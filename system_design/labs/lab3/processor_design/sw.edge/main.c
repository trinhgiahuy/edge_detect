#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TIMER (*((volatile unsigned long *) 0x600A0000))

#include "common_stim_results.h"

void load_data_array(unsigned char *d)
{
  int i;

  for (i=0; i<IMAGE_SIZE; i++) d[i] = *(SHARED_MEMORY_BASE + i);
}

int main(int argument_count, char *argument_list[])
{
  unsigned long         start;
  unsigned long         end;
  static unsigned char  data_in[IMAGE_SIZE];
  static float          sw_data_out[2 * IMAGE_SIZE];
  static float          hw_data_out[2 * IMAGE_SIZE];

  check_register_access();

/*
  printf("loading data... \n");

  load_data_array(data_in);

  printf("Running... \n");

  edge_detect_sw(data_in, sw_data_out);
  edge_detect_hw(data_in, hw_data_out);

  check_results(sw_data_out, hw_data_out);
*/

  printf("Finished \n");
}

