#include "edge_detect_defines.h"

#define MEM_BASE 0x70000000

  void check_results(float *sw_results, float *hw_results)
  {
    float mag_err = 0;
    float ang_err = 0;
    int i;

    printf("comparing magnitudes: \n");
    for (i=0; i<IMAGE_SIZE; i++) {
      if (sw_results[i] != hw_results[i]) {
         printf("difference[%d]: sw: %f hw: %f \n", i, sw_results[i], hw_results[i]);
         mag_err += abs(sw_results[i] - hw_results[i]);
      }
    }
    mag_err = mag_err/IMAGE_SIZE;

    printf("comparing angles: \n");
    for (i=IMAGE_SIZE; i<IMAGE_SIZE * 2; i++) {
      if (sw_results[i] != hw_results[i]) {
         printf("difference[%d]: sw: %f hw: %f \n", i-IMAGE_SIZE, sw_results[i], hw_results[i]);
         ang_err += abs(sw_results[i] - hw_results[i]);
      }
    }
    ang_err = ang_err/IMAGE_SIZE;
    printf("Magnitude avg error = %f\n",mag_err);
    printf("Angle avg error = %f\n",ang_err);
    if (mag_err > 2 || ang_err >0.2) {
      printf("Errors bigger than tolerance\n");

#ifdef SYSTEMC
      SC_REPORT_ERROR("testbench checker", "results did not match expected output");
#endif

    } else {
      printf("Passed \n");
    }
  }


  static inline unsigned int float_to_fixed(float f, int width, int integer_bits)
  { 
    unsigned long mask = (1 << width) - 1;
    unsigned int  fractional_bits = width - integer_bits;
    unsigned long fbits;
    int neg;
    
    if (f < 0) neg = 1; else neg = 0;
    if (neg) f = f * -1.0;
    
    f = f * (1 << fractional_bits);
    fbits = (unsigned long) f;
    if (neg) fbits = (~fbits) + 1;
    fbits = fbits & mask;
    return fbits;
  }
  
  
  static inline float fixed_to_float(int fixed_value, int width, int integer_bits)
  { 
    unsigned int  fractional_bits = width - integer_bits;
    unsigned long mask = (1 << width) - 1;
    int neg = 0;
    float f;
    
    if ((1 << (width - 1)) & fixed_value) neg = 1; else neg = 0;
    if (neg) fixed_value = ((~fixed_value)  + 1) & mask;
    
    f = ((float) fixed_value) / ((float)(1 << fractional_bits)) ;
     
    if (neg) f = f * -1.0;
    
    return f;
  }

   static inline float ufixed_to_float(int fixed_value, int width, int integer_bits)
  { 
    unsigned int  fractional_bits = width - integer_bits;
    unsigned long mask = (1 << width) - 1;
    int neg = 0;
    float f;
    
    
    f = ((float) fixed_value) / ((float)(1 << fractional_bits)) ;
     
    
    return f;
  }
  

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
  
  static unsigned int calc_interleave(int x, int y, int w)
  {
    int index = y * w + x;
  
    return index + (index/4) * 4;
  } 
  
  //--------------------------------------------------------------------------
  // Function: verticalDerivative
  //   Compute the vertical derivative on the input data
  //
  void verticalDerivativeHw(unsigned char *dat_in, 
                            unsigned char *dy, const int *kernel) 
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
    unsigned char r;
  
    for (y = 0; y < IMAGE_HEIGHT; y++) {
      for (x = 0; x < IMAGE_WIDTH; x++) {
        a_index = clip(y - 1, IMAGE_HEIGHT-1) * IMAGE_WIDTH + x;
        b_index = y * IMAGE_WIDTH + x;
        c_index = clip(y + 1, IMAGE_HEIGHT-1) * IMAGE_WIDTH + x;
        
        a = TB_READ_8(dat_in + a_index);
        b = TB_READ_8(dat_in + b_index);
        c = TB_READ_8(dat_in + c_index);

        f = a * kernel[0] + b * kernel[1] + c * kernel[2];

        offset = calc_interleave(x, y, IMAGE_WIDTH) + 4;
        if(f > 0) f += 0.5;
        if(f < 0) f += -0.5;
        if (f > 127.0) f = 127.0;
        if (f < -128.0) f = -128.0;
        r = float_to_fixed(f, 8, 8);
        TB_WRITE_8(dy + offset, r);
printf("vd y: %d x: %d offset: %d value: %f \n", y, x, offset, f);
      }
    }
  }
  
  //--------------------------------------------------------------------------
  // Function: horizontalDerivative
  //   Compute the horizontal derivative on the input data
  void horizontalDerivativeHw(unsigned char *dat_in, 
                          unsigned char *dy, const int *kernel) 
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
    unsigned char r;

    for (y = 0; y < IMAGE_HEIGHT; y++) {
      for (x = 0; x < IMAGE_WIDTH; x++) {
  
        a_index = y * IMAGE_WIDTH + clip(x - 1, IMAGE_WIDTH-1);
        b_index = y * IMAGE_WIDTH + x;
        c_index = y * IMAGE_WIDTH + clip(x + 1, IMAGE_WIDTH-1);

        a = TB_READ_8(dat_in + a_index);
        b = TB_READ_8(dat_in + b_index);
        c = TB_READ_8(dat_in + c_index);

        f = a * kernel[0] + b * kernel[1] + c * kernel[2];
  
        offset = calc_interleave(x, y, IMAGE_WIDTH);
        if(f > 0) f += 0.5;
        if(f < 0) f += -0.5;
        if (f > 127.0) f = 127.0;
        if (f < -128.0) f = -128.0;
        r = float_to_fixed(f, 8, 8);
        TB_WRITE_8(dy + offset, r);
printf("hd y: %d x: %d offset: %d value: %f (%d + %d) \n", y, x, offset, f, a, 0-c);
      }
    }
  }
   
  //--------------------------------------------------------------------------
  // Function: magnitudeAngle
  //   Compute the magnitute and angle based on the horizontal and vertical
  //   derivative results
  void magnitudeAngleHw(unsigned char  *dxy,
                        unsigned char  *temp,
                        float          *data_out) 
  {
    unsigned long long magnitude_array;
    unsigned long long angle_array; 
    unsigned char magnitude;
    unsigned char angle;
    int i;
    int j;
  
    SET_HEIGHT(IMAGE_HEIGHT);
    SET_WIDTH(IMAGE_WIDTH);
    SET_INPUT_OFFSET(((unsigned char *)dxy) - SHARED_MEMORY_BASE);
    SET_OUTPUT_OFFSET(((unsigned char *)temp) - SHARED_MEMORY_BASE);
    
    GO;
    WAIT_FOR_DONE;
  
    // copy from temp to data_out
    // convert from fixed point to floats as it is copied
  
    for (i=0; i<IMAGE_SIZE * 2; i+=2 * N) {
       magnitude_array = TB_READ_32(temp + i );
       angle_array     = TB_READ_32(temp + i + N);
  
printf("magnitudes read at: %08x = %016llx \n", temp + i, magnitude_array);
printf("angles read at:     %08x = %016llx \n", temp + i + N, angle_array);  

       for (j=0; j<N; j++) {
          magnitude = (magnitude_array >> (j * 8)) & 0xFF;
          angle = (angle_array >> (j * 8)) & 0xFF;
          data_out[i/2 + j] = ufixed_to_float(magnitude, 8, 8);
          data_out[IMAGE_SIZE + i/2 + j] = fixed_to_float(angle, 8, 3);
       }
    } 
  }

  void edge_detect_hw(unsigned char *data_in,     // image data (streamed in by pixel)
                      float         *data_out)    // magnitude and angle output
  {
    // buffers for image data
  #ifdef SMALL
    static unsigned char *dxy        = SHARED_MEMORY_BASE + 0x1000;
    static unsigned char *scratchpad = SHARED_MEMORY_BASE + 0x2000;
  #else
    static unsigned char *dxy        = SHARED_MEMORY_BASE + 0x200000;
    static unsigned char *scratchpad = SHARED_MEMORY_BASE + 0x400000;
  #endif
    const int kernel[] = KERNEL;

    verticalDerivativeHw(data_in, dxy, kernel);
    horizontalDerivativeHw(data_in, dxy, kernel);
    magnitudeAngleHw(dxy, scratchpad, data_out);
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

    verticalDerivativeSw(data_in, dy, kernel);
    horizontalDerivativeSw(data_in, dx, kernel);
    magnitudeAngleSw(dx, dy, data_out);

    // compare dx
    
    int x, y, ih, is;
    for (y=0; y<IMAGE_HEIGHT; y++) {
      for (x=0; x<IMAGE_WIDTH; x++) {
        is = y * IMAGE_WIDTH + x;
        ih = calc_interleave(x, y, IMAGE_WIDTH);
        printf("sw dx[%d] = %f hw dx[%d] = %f \n", is, dx[is], ih, (float) (signed char) TB_READ_8(0x70001000 + ih));
      }
    } 
    // compare dy
    
    for (y=0; y<IMAGE_HEIGHT; y++) {
      for (x=0; x<IMAGE_WIDTH; x++) {
        is = y * IMAGE_WIDTH + x;
        ih = calc_interleave(x, y, IMAGE_WIDTH) + 4;
        printf("sw dy[%d] = %f hw dy[%d] = %f \n", is, dy[is], ih, (float) (signed char) TB_READ_8(0x70001000 + ih));
      }
    } 

  }
