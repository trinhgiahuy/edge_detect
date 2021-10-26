#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "edge_detect_defines.h"

#define TIMER (*((volatile unsigned long *) 0x600A0000))

//--------------------------------------------------------------------------
// Function: clip
//   Perform boundary processing by "adjusting" the index value to "clip"
//   at either end
static int clip(int i, int bound) {
  if (i < 0) {
    return 0;               // clip to the top/left value
  } else if (i >= bound) {
    return bound;           // clip to the bottom/right value
  } else {
    return i;               // return all others untouched
  }
}
  
void verticalDerivativeSw(unsigned char *dat_in, 
                          float *dy, const int *kernel) 
{
  int x;
  int y;
  float f;
  unsigned int offset;
  unsigned int a_index;
  unsigned int b_index;
  unsigned int c_index;
  int a;
  int b;
  int c;

  for (y = 0; y < IMAGE_HEIGHT; y++) {
    for (x = 0; x < IMAGE_WIDTH; x++) {
      a_index = clip(y - 1, IMAGE_HEIGHT-1) * IMAGE_WIDTH + x;
      b_index = y * IMAGE_WIDTH + x;
      c_index = clip(y + 1, IMAGE_HEIGHT-1) * IMAGE_WIDTH + x;
      
      a = *(dat_in + a_index);
      b = *(dat_in + b_index);
      c = *(dat_in + c_index);

      f = a * kernel[0] + b * kernel[1] + c * kernel[2];

      offset = y * IMAGE_WIDTH + x;
      *(dy + offset) = f;
    }
  }
}

void horizontalDerivativeSw(unsigned char *dat_in, 
                            float *dy, const int *kernel) 
{
  int x;
  int y; 
  float f;
  unsigned int offset;
  unsigned int a_index;
  unsigned int b_index;
  unsigned int c_index;
  int a;
  int b;
  int c;

  for (y = 0; y < IMAGE_HEIGHT; y++) {
    for (x = 0; x < IMAGE_WIDTH; x++) {

      a_index = y * IMAGE_WIDTH + clip(x - 1, IMAGE_WIDTH-1);
      b_index = y * IMAGE_WIDTH + x;
      c_index = y * IMAGE_WIDTH + clip(x + 1, IMAGE_WIDTH-1);

      a = *(dat_in + a_index);
      b = *(dat_in + b_index);
      c = *(dat_in + c_index);

      f = a * kernel[0] + b * kernel[1] + c * kernel[2];

      offset = y * IMAGE_WIDTH + x;
      *(dy + offset) = f;
    }
  }
}

static void magnitudeAngleSw(float *dx,
                    float *dy,
                    float *data_out)
{
  float dx_sq;
  float dy_sq;
  float sum;
  int x;
  int y;
  int offset;

  for (y = 0; y < IMAGE_HEIGHT; y++) {
    for (x = 0; x < IMAGE_WIDTH; x++) {
      dx_sq = *(dx + y * IMAGE_WIDTH + x) * *(dx + y * IMAGE_WIDTH + x);
      dy_sq = *(dy + y * IMAGE_WIDTH + x) * *(dy + y * IMAGE_WIDTH + x);
      sum = dx_sq + dy_sq;
      offset = y * IMAGE_WIDTH + x;
      *(data_out + offset) = sqrt(sum);
      *(data_out + offset + IMAGE_SIZE) = atan2(dy[y * IMAGE_WIDTH + x], dx[y * IMAGE_WIDTH + x]);
    }
  }
}

void edge_detect_sw(unsigned char *data_in,     // image data (streamed in by pixel)
                    float         *data_out)    // magnitude and angle output
{
  // buffers for image data
  static float         dx[IMAGE_SIZE];
  static float         dy[IMAGE_SIZE];

  const int kernel[] = KERNEL;
  unsigned long start, end;

  start = TIMER;
  verticalDerivativeSw(data_in, dy, kernel);
  end = TIMER;
  printf("Vertical derivative clocks: %d \n", end-start);

  start = TIMER;
  horizontalDerivativeSw(data_in, dx, kernel);
  end = TIMER;
  printf("Horizontal derivative clocks: %d \n", end-start);

  start = TIMER;
  magnitudeAngleSw(dx, dy, data_out);
  end = TIMER;
  printf("Magnitude/angle clocks: %d \n", end-start);

}


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

  printf("loading data... \n");

  load_data_array(data_in);

  printf("Running... \n");

  start = TIMER;
  edge_detect_sw(data_in, sw_data_out);
  end = TIMER;

  printf("sw execution time: %d clocks \n", end-start);

  printf("Finished \n");
}

