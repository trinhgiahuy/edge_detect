#ifndef _MAGN_ANGLE_
#define _MAGN_ANGLE_

// Include types and constant kernel definition
#include "types.h"
//This is Catapult's math library implementation, see docs for details
#include <ac_math.h>
#include <ac_math/ac_sqrt.h>
#include <ac_math/ac_atan2_cordic.h>

// Class for fifo-style hierarchical interconnect objects
#include <ac_channel.h>
#include <ac_fixed.h>

#include <mc_scverify.h>

#pragma hls_design top
class MagnitudeAngle {
 public:
  MagnitudeAngle() {}
  
  //--------------------------------------------------------------------------
  // Function: run
  //   Compute the magnitute and angle based on the horizontal and vertical
  //   derivative results
  #pragma hls_design interface
  void CCS_BLOCK(run)(ac_channel<gradBus>      &dxdy_in,
                      ac_channel<magAngBus>    &magn_angle,
                      ac_int<widthBits,false>  imageWidth,
                      ac_int<heightBits,false> imageHeight) {
    gradBus dxdy;
    gradType dx, dy;
    sqType dx_sq;
    sqType dy_sq;
    sumType sum; // fixed point integer for sqrt
    angType at;
    magAngBus ma;
    sqRtType sq_rt; // square-root return type

    MROW: for (int y = 0; y < IMG_HEIGHT; y++) {
      MCOL: for (int x = 0; x < IMG_WIDTH/BUS_WORDS; x++) {
        dxdy = dxdy_in.read();
        MCOL4: for (int i=0; i<BUS_WORDS; i++) {
          dx = dxdy.data0[i];
          dy = dxdy.data1[i];
          dx_sq = dx * dx;
          dy_sq = dy * dy;
          sum = dx_sq + dy_sq;
          // Catapult's math library of sqrt and atan2
          ac_math::ac_sqrt(sum,sq_rt);
          ac_math::ac_atan2_cordic((ac_fixed<9,9>)dy, (ac_fixed<9,9>) dx, at);
          ma.data0[i] = sq_rt;
          ma.data1[i] = at;
        }
        magn_angle.write(ma);
        if(x == imageWidth/BUS_WORDS-1)
          break;
      }
      if(y == imageHeight-1)
        break;
    }
  }
};

#endif

