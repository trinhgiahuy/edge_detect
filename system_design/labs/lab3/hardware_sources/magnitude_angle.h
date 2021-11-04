#ifndef _INCLUDED_MAGNITUDE_ANGLE_
#define _INCLUDED_MAGNITUDE_ANGLE_

//--------------------------------------------------------------------------
// Function: magnitudeAngle
//   Compute the magnitute and angle based on the horizontal and vertical
//   derivative results

//This is Catapult's math library implementation, see docs for details
#include <ac_math/ac_sqrt_pwl.h>
#include <ac_math/ac_atan2_cordic.h>

#define N 4

typedef ac_int<10, false>  dimension_type;
typedef ac_int<64, false>  bus_type;
typedef ac_int<8, false>   in_type;
typedef ac_int<8, false>   out_type;

// Define some bit-accurate types to use in this model
typedef uint8                             pixelType;    // input pixel is 0-255
typedef int8                              gradType;     // Derivative is max range -255 to 255
typedef ac_fixed<18,18, false>            sqType;       // Result of 9-bit x 9-bit
typedef ac_fixed<19,19, false>            sumType;      // Result of 18-bit + 18-bit fixed pt integer for squareroot
typedef ac_fixed<8,8,false,AC_RND,AC_SAT> magType; // 9-bit unsigned magnitute result
typedef ac_fixed<8,3,true>                angType;      // 3 integer bit, 5 fractional bits for quantized angle -pi to pi
#pragma hls_design
class magnitude_angle: public sc_module {
 public:

  sc_in<bool>                              clk;
  sc_in<bool>                              rstn;

  sc_in<ac_int<12, false> >                height;
  sc_in<ac_int<12, false> >                width;

  Connections::In <ac_int<64, false> >     CCS_INIT_S1(mem_in_data);
  Connections::Out <ac_int<64, false> >    CCS_INIT_S1(mem_out_data);

  SC_CTOR(magnitude_angle) {
    SC_THREAD(run_mag_ang);
    sensitive << clk.pos();
    async_reset_signal_is(rstn, false);
  }

  void run_mag_ang() {
    mem_out_data.Reset();
    mem_in_data.Reset();
    wait();
    while (1) {
      ac_int<64, false> dxdy_buffer;
      ac_int<64, false> mag_ang_buffer;
      gradType dx, dy;
      ac_fixed<16,9,false> sq_rt; // square-root return type
      sumType sum;
      sqType dx_sq,dy_sq;
      angType at;

      MROW: for (int y = 0; ; y++) {
        MCOL: for (int x = 0; ; x++) {
          dxdy_buffer = mem_in_data.Pop();
          MCOL8: for (int i=0; i<N; i++) {
            dx.set_slc(0,dxdy_buffer.slc<8>(i*8));
            dy.set_slc(0,dxdy_buffer.slc<8>(32 + i*8));
            dx_sq = dx * dx;
            dy_sq = dy * dy;
            sum = dx_sq + dy_sq;
            // Catapult's math library piecewise linear implementation of sqrt and cordic atan2
            ac_math::ac_sqrt_pwl(sum,sq_rt);
            ac_math::ac_atan2_cordic((ac_fixed<9,9>)dy, (ac_fixed<9,9>) dx, at);
            mag_ang_buffer.set_slc(i*8,magType(sq_rt).slc<8>(0));
            mag_ang_buffer.set_slc(32+i*8,at.slc<8>(0));
            wait(4);  // model delay - one calcualtion takes 4 clocks
          }
          mem_out_data.Push(mag_ang_buffer);
          if (x== width.read()/N-1)
          { break; }
        }
        if (y== height.read()-1)
        { break; }
      }
    }
  }
};

#endif
