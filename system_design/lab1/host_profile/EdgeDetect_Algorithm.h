#ifndef _INCLUDED_EDGEDETECT_ALGORITHM_H_
#define _INCLUDED_EDGEDETECT_ALGORITHM_H_

// Revision History
//    Rev 1 - Coding of edge detection algorithm in C++

#include <math.h>
#include <stdlib.h>
// Include constant kernel definition
#include "edge_defs.h"

#include <sys/times.h>

class EdgeDetect_Algorithm
{
 
public:
  // Constructor
  EdgeDetect_Algorithm() {}

  // Additions to time operations
  struct tms start, end;

  void start_timer()
  {
     times(&start);
  }

  void end_timer(char *s)
  {
     clock_t user_time, system_time;

     times(&end);

     user_time = end.tms_utime - start.tms_utime;
     system_time = end.tms_stime - start.tms_stime;

     printf("%-12s ", s);
     printf("User time: %5.2f ", user_time/100.0);
     printf("System time: %5.2f ", system_time/100.0);
     printf("Total time: %5.2f \n", (user_time + system_time)/100.0);
  }

  //--------------------------------------------------------------------------
  // Function: run
  //   Top interface for data inout of class. Combines vertical and 
  //   horizontal derivative and magnitude/angle computation.
  void run(unsigned char *dat_in,  // image data (streamed in by pixel)
           double        *magn,    // magnitude output
           double        *angle,   // angle output
           unsigned int  imageWidth,
           unsigned int  imageHeight)
  {
    // allocate buffers for image data
    double *dy = (double *)malloc(imageHeight*imageWidth*sizeof(double));
    double *dx = (double *)malloc(imageHeight*imageWidth*sizeof(double));

    start_timer();
    for (int i=0; i<200; i++) verticalDerivative(dat_in, dy, imageWidth, imageHeight);
    for (int i=0; i<200; i++) horizontalDerivative(dat_in, dx, imageWidth, imageHeight);
    for (int i=0; i<200; i++) magnitudeAngle(dx, dy, magn, angle, imageWidth, imageHeight);
    end_timer("Run"); 

    free(dy);
    free(dx);
  }

  //--------------------------------------------------------------------------
  // Function: verticalDerivative
  //   Compute the vertical derivative on the input data
  void verticalDerivative(unsigned char *dat_in,
                          double *dy,
                          unsigned int  imageWidth,
                          unsigned int  imageHeight) 
  {
    for (int y = 0; y < imageHeight; y++) {
      for (int x = 0; x < imageWidth; x++) {
        *(dy + y * imageWidth + x) =
          dat_in[clip(y - 1, imageHeight-1) * imageWidth + x] * kernel[0] +
          dat_in[y * imageWidth + x]                          * kernel[1] +
          dat_in[clip(y + 1, imageHeight-1) * imageWidth + x] * kernel[2];
      }
    }
  }

  //--------------------------------------------------------------------------
  // Function: horizontalDerivative
  //   Compute the horizontal derivative on the input data
  void horizontalDerivative(unsigned char *dat_in, 
                            double *dx,
                            unsigned int  imageWidth,
                            unsigned int  imageHeight) 
  {
    for (int y = 0; y < imageHeight; y++) {
      for (int x = 0; x < imageWidth; x++) {
        *(dx + y * imageWidth + x) =
          dat_in[y * imageWidth + clip(x - 1, imageWidth-1)] * kernel[0] +
          dat_in[y * imageWidth + x]                         * kernel[1] +
          dat_in[y * imageWidth + clip(x + 1, imageWidth-1)] * kernel[2];
      }
    }
  }

  //--------------------------------------------------------------------------
  // Function: magnitudeAngle
  //   Compute the magnitute and angle based on the horizontal and vertical
  //   derivative results
  void magnitudeAngle(double *dx, 
                      double *dy, 
                      double *magn, 
                      double *angle,
                      unsigned int  imageWidth,
                      unsigned int  imageHeight) 
  {
    double dx_sq;
    double dy_sq;
    double sum;
    for (int y = 0; y < imageHeight; y++) {
      for (int x = 0; x < imageWidth; x++) {
        dx_sq = *(dx + y * imageWidth + x) * *(dx + y * imageWidth + x);
        dy_sq = *(dy + y * imageWidth + x) * *(dy + y * imageWidth + x);
        sum = dx_sq + dy_sq;
        *(magn + y * imageWidth + x) = sqrt(sum);
        *(angle + y * imageWidth + x) = atan2(dy[y * imageWidth + x], dx[y * imageWidth + x]);
      }
    }
  }

private: // Helper functions

  //--------------------------------------------------------------------------
  // Function: clip
  //   Perform boundary processing by "adjusting" the index value to "clip"
  //   at either end
  int clip(int i, int bound) {
    if (i < 0) {
      return 0;               // clip to the top/left value
    } else if (i >= bound) {
      return bound;           // clip to the bottom/right value
    } else {
      return i;               // return all others untouched
    }
  }

};

#endif

