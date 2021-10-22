#ifndef _INCLUDED_MAGNITUDE_ANGLE_
#define _INCLUDED_MAGNITUDE_ANGLE_

//--------------------------------------------------------------------------
// Function: magnitudeAngle
//   Compute the magnitute and angle based on the horizontal and vertical
//   derivative results
#pragma hls_design
//void magnitudeAngle(ac_channel<gradType> &dx_in,
//                    ac_channel<gradType> &dy_in,
//                    ac_channel<magType>  &magn,
//                    ac_channel<angType>  &angle)

//This is Catapult's math library implementation, see docs for details
#include <ac_math/ac_sqrt_pwl.h>
#include <ac_math/ac_atan2_cordic.h>

#define N 4

typedef ac_int<10, false>  dimension_type;
typedef ac_int<64, false>  bus_type;
typedef ac_int<8, false>   in_type;
typedef ac_int<8, false>   out_type;

  // Define some bit-accurate types to use in this model
  typedef uint8                  pixelType;    // input pixel is 0-255
  typedef int9                   gradType;     // Derivative is max range -255 to 255
  typedef ac_fixed<24,18, true>  sqType;       // Result of 9-bit x 9-bit
  typedef ac_fixed<24,24, true>  sumFixedPointType;      // Result of 18-bit + 18-bit fixed pt 
  typedef ac_fixed<19,19, false> sumType;      // Result of 18-bit + 18-bit fixed pt integer for squareroot
  typedef uint9                  magType;      // 9-bit unsigned magnitute result
  typedef ac_fixed<8,3,true>     angType;      // 3 integer bit, 5 fractional bits for quantized angle -pi to pi

class magnitude_angle: public sc_module 
{
public:

  sc_in<bool>                              clk;
  sc_in<bool>                              rstn;

  sc_in<ac_int<18, false> >                input_offset;
  sc_in<ac_int<18, false> >                output_offset;

  sc_in<ac_int<12, false> >                height;
  sc_in<ac_int<12, false> >                width;

  Connections::Out <ac_int<32, false> >    CCS_INIT_S1(mem_in_addr);
  Connections::Out <ac_int<8, false> >     CCS_INIT_S1(mem_in_burst);
  Connections::In <ac_int<64, false> >     CCS_INIT_S1(mem_in_data);
  
  Connections::Out <ac_int<32, false> >    CCS_INIT_S1(mem_out_addr);
  Connections::Out <ac_int<8, false> >     CCS_INIT_S1(mem_out_burst);
  Connections::Out <ac_int<64, false> >    CCS_INIT_S1(mem_out_data);

  Connections::SyncIn                      CCS_INIT_S1(start);
  Connections::SyncOut                     CCS_INIT_S1(done);

  SC_CTOR(magnitude_angle) {
    SC_THREAD(run_mag_ang);
    sensitive << clk.pos();
    async_reset_signal_is(rstn, false);
  }

  void run_mag_ang()
  {
    mem_out_addr.Reset();
    mem_out_burst.Reset();
    mem_out_data.Reset();
  
    mem_in_addr.Reset();
    mem_in_burst.Reset();
    mem_in_data.Reset();

    start.Reset();
    done.Reset();
  
    wait();

    while (1) {

      ac_int<64, false> dx_buffer[2];
      ac_int<64, false> dy_buffer[2];
      ac_int<64, false> magnitude_buffer;
      ac_int<64, false> angle_buffer;
  
      ac_fixed<16,9,false> sq_rt; // square-root return type

      start.sync_in();

      for (int i=0; i<height.read()*width.read(); i+=8) {
  
        mem_in_addr.Push((ac_int<32, false>) ((input_offset.read().to_int() + i * 4)));
        mem_in_burst.Push(3);
    
        dx_buffer[0] = mem_in_data.Pop();
        dy_buffer[0] = mem_in_data.Pop();
        dx_buffer[1] = mem_in_data.Pop();
        dy_buffer[1] = mem_in_data.Pop();

        for (int j=0; j<2; j++) {

          for (int n=0; n<N; n++) {
    
            ac_fixed<16, 16, true> dx;
            ac_fixed<16, 16, true> dy;
      
            sqType dx_sq;
            sqType dy_sq;
      
            sumType sum; // fixed point integer for sqrt
            sumFixedPointType sum_fixed_point;
            angType at;
      
            dx.set_slc(0, dx_buffer[j].slc<16>(n*16));
            dy.set_slc(0, dy_buffer[j].slc<16>(n*16));
  
            dx_sq = dx * dx;
            dy_sq = dy * dy;
  
            sum_fixed_point = dx_sq + dy_sq;
            sum = sum_fixed_point.to_int();
  
            // Catapult's math library piecewise linear implementation of sqrt and atan2
      
            ac_math::ac_sqrt_pwl(sum,sq_rt);
            // use staurating/rounding types
            // if (sq_rt.to_double() > 127.0) sq_rt = 127.0;
            // if (sq_rt.to_double() < -128.0) sq_rt = -128.0;
            magnitude_buffer.set_slc(j*32 + n*8, (ac_int<8, false>) sq_rt.to_uint());
            ac_math::ac_atan2_cordic((ac_fixed<9,9>)dy, (ac_fixed<9,9>)dx, at);
            // use staurating/rounding types
            //if (at.to_double() > 31.9375) at = 31.9375;
            //if (at.to_double() < -32.0) at = -32.0;
            angle_buffer.set_slc(j*32 + n*8, (ac_int<8, false>) at.slc<8>(0));
          }
        }
    
        mem_out_addr.Push((ac_int<32, false>) (output_offset.read() + i * 2));
        mem_out_burst.Push(1);
  
        mem_out_data.Push(magnitude_buffer);
        mem_out_data.Push(angle_buffer);
      }
      done.sync_out();
    }
  }
};

#endif
