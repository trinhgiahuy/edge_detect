#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "edge_detect_defines.h"

#include "magnitude_angle_if.h"
#include "common_stim_results.h"

#ifdef HOST

static void trim(char *s)
{
   char *sp = s;
   char *sb = s;

   // advance to a ":" character

   while (*sp != ':') sp++;

   // advance to an alpha numeric character

   while (!isxdigit(*sp)) sp++;

   // copy all alpha numeric characters to the start of the line

   while (isxdigit(*sp)) *sb++ = *sp++;

   // null terminate the line

   *sb = 0;
} 


void load_data_array(unsigned char *d)
{
  int ptr = 0;
  unsigned char line[1000]; 
  unsigned char *last_byte;
  FILE *memory_image = fopen(MEMORY_IMAGE_FILE, "r");

  if (!memory_image) {
    fprintf(stderr, "Unable to open file \"%s\" for reading \n");
    perror("testbench");
    exit(-1);
  }

  while (!feof(memory_image)) {
    if (fgets(line, sizeof(line), memory_image)) {
      trim(line);
      while (strlen(line)) {
         last_byte = line + strlen(line) - 2;
         d[ptr++] = strtol(last_byte, NULL, 16);
         *last_byte = 0;
      }
    }
  }
  fclose(memory_image);
}

#else // not HOST (i.e. RISCV | EMBEDDED)

void load_data_array(unsigned char *d)
{
  int i;

  for (i=0; i<IMAGE_SIZE; i++) d[i] = *(SHARED_MEMORY_BASE + i);
}

#endif // not HOST

void memory_test()
{
   unsigned long   ul_value;
   unsigned int    i_value;
   unsigned short  s_value;
   unsigned char   c_value;
   int             i;
   int             reps = 16;
   int             errors = 0;

   printf("64 bits... \n");
   for (i=0; i<reps; i++) {
      *(((volatile unsigned long *) 0x70000000) + i) = i;
   }
   for (i=0; i<reps; i++) {
      ul_value = *(((unsigned long *) 0x70000000) + i);
      if (i != ul_value) {
         errors++;
         printf ("Expected: %d got: %d \n", i, ul_value);
      }
   }

   reps = reps * 2;
   printf("32 bits... \n");
   for (i=0; i<reps; i++) {
      *(((volatile unsigned int *) 0x70000000) + i) = i;
   }
   for (i=0; i<reps; i++) {
      ul_value = *(((unsigned int *) 0x70000000) + i);
      if (i != ul_value) {
         errors++;
         printf ("Expected: %d got: %d \n", i, ul_value);
      }
   }

   reps = reps * 2;
   printf("16 bits... \n");
   for (i=0; i<reps; i++) {
      *(((volatile unsigned short *) 0x70000000) + i) = i;
   }
   for (i=0; i<reps; i++) {
      ul_value = *(((unsigned short *) 0x70000000) + i);
      if (i != ul_value) {
         errors++;
         printf ("Expected: %d got: %d \n", i, ul_value);
      }
   }

   reps = reps * 2;
   printf("8 bits... \n");
   for (i=0; i<reps; i++) {
      *(((volatile unsigned char *) 0x70000000) + i) = i;
   }
   for (i=0; i<reps; i++) {
      ul_value = *(((unsigned char *) 0x70000000) + i);
      if (i != ul_value) {
         errors++;
         printf ("Expected: %d got: %d \n", i, ul_value);
      }
   }

   printf("errors: %d \n", errors);
}

void hello()
{
   printf("Hello, World!\n");
}


#define TIMER (*((volatile unsigned long *) 0x600A0000))


int main(int argument_count, char *argument_list[])
{
  unsigned long         start;
  unsigned long         end;
  static unsigned char  data_in[IMAGE_SIZE];
  static float          sw_data_out[2 * IMAGE_SIZE];
  static float          hw_data_out[2 * IMAGE_SIZE];

//  hello();
//
//  printf("memory_test... \n");
//
//  memory_test();

  printf("loading data... \n");

  load_data_array(data_in);

  printf("Running... \n");

  start = TIMER;
  edge_detect_sw(data_in, sw_data_out);
  end = TIMER;

  printf("sw execution time: %d clocks \n", end-start);

  start = TIMER;
  edge_detect_hw((unsigned char *) 0x70000000, hw_data_out);
  end = TIMER;

  printf("hw execution time: %d clocks \n", end-start);

  check_results(sw_data_out, hw_data_out);

#ifdef HOST

  FILE *expected_results = fopen("expected_results.bin", "r");
  float sum_error = 0.0;
  float ang_error = 0.0;
  int count;
  int r;
  
  if (!expected_results) {
    fprintf(stderr, "Unable to open file \"image_results.bin\" for reading \n");
    perror("testbench");
    exit(-1);
  }

  count = 0;

  while (!feof(expected_results)) {

    float f[2];
    r = fread(f, sizeof(float), 2, expected_results);
    if (r == 2) {
      sum_error += fabs(f[0] - data_out[count * 2]);
      ang_error += fabs(f[1] - data_out[count * 2 + 1]);
      count++;       
    }
  }

  if (count != IMAGE_SIZE) {
     fprintf(stdout, "results file size is incorrect, found %d records, expected %d records \n", count, IMAGE_SIZE);
  }

  printf("data points: %d \n", count);
  printf("Average sum error: %f \n", sum_error/IMAGE_SIZE);
  printf("Average ang error: %f \n", ang_error/IMAGE_SIZE);

#endif

  printf("Finished \n");
}

