#ifndef _MAGNITUDE_ANGLE_H_
#define _MAGNITUDE_ANGLE_H_

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
class MagnitudeAngle
{
 
public:
  // Constructor
  MagnitudeAngle() {}

  #pragma hls_design interface
  void CCS_BLOCK(run)(ac_channel<ac_fixed<8,8,true>>     &dx_in,
           ac_channel<ac_fixed<8,8,true>>                &dy_in,
           ac_channel<ac_fixed<8,8,false,AC_RND,AC_SAT>> &magn,
           ac_channel<ac_fixed<8,3,true>>                &angle,
           ac_int<11,false>                              imageWidth,
           ac_int<11,false>                              imageHeight) 
  {
    ac_fixed<8,8,true>    dx, dy;
    ac_fixed<18,18,false> dx_sq;
    ac_fixed<18,18,false> dy_sq;
    ac_fixed<19,19,false> sum; // fixed point integer for sqrt
    ac_fixed<8,3,true>    at;
    ac_fixed<16,9,false>  sq_rt; // square-root return type

    MROW: for (int y = 0; y < 1080; y++) {
      MCOL: for (int x = 0; x < 1920; x++) {
        dx = dx_in.read();
        dy = dy_in.read();
        dx_sq = dx * dx;
        dy_sq = dy * dy;
        sum = dx_sq + dy_sq;
        // Catapult's math implementation of sqrt and atan2
        ac_math::ac_sqrt(sum,sq_rt);
        magn.write(sq_rt.to_uint());
        ac_math::ac_atan2_cordic((ac_fixed<9,9>)dy, (ac_fixed<9,9>) dx, at);
        angle.write(at);
        if(x == imageWidth-1)
          break;
      }
      if(y == imageHeight-1)
        break;
    }
  }
private: 

};

#endif

