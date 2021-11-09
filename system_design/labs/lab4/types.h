#ifndef _INCLUDED_TYPES_H_
#define _INCLUDED_TYPES_H_

#include <ac_fixed.h>
#include <ac_sync.h>
#include <ac_channel.h>

const int IMG_WIDTH=1920;
const int IMG_HEIGHT=1080;
const int BUS_WORDS = 4;
const int BITS = 8;
// Define some bit-accurate types to use in this model
typedef ac_int<BITS,false>                     pixelType;    // input pixel is 0-255
typedef  ac_int<BITS*2,false>                   pixelType2x;
typedef ac_fixed<BITS,BITS,true,AC_RND,AC_SAT> gradType;     // Derivative is max range -255 to 255
typedef ac_fixed<BITS*2,BITS*2,false>          sqType;       // Result of 9-bit x 9-bit
typedef ac_fixed<BITS*2+1,BITS*2+1,false>      sumType;      // Result of 18-bit + 18-bit fixed pt integer for squareroot
typedef ac_fixed<BITS,BITS,false,AC_RND,AC_SAT> magType;      //9-bit unsigned magnitute result
typedef ac_fixed<BITS,3,true>                   angType;      // 3 integer bit, 5 fractional bits for quantized angle -pi to pi
typedef ac_fixed<BITS*2,9,false>                    sqRtType;

enum{
 widthBits = ac::nbits<IMG_WIDTH>::val,
 heightBits = ac::nbits<IMG_HEIGHT>::val
};

struct pixelBus {
  pixelType data[BUS_WORDS];
};

struct pixelBus2x {
  ac_int<BITS*BUS_WORDS*2,false> data;
  void set_slc(const int idx, pixelBus din){
#pragma unroll yes
    for(int i=0; i<BUS_WORDS;i++){
      data.set_slc(idx+i*BITS, din.data[i]);
    }
  }

  pixelBus slc(int idx){
    pixelBus d;
    #pragma unroll yes
    for(int i=0; i<BUS_WORDS;i++){
      d.data[i] = data.slc<BITS>(idx+i*BITS);
    }
    return d;
  }
};

struct gradBus {
  gradType data[BUS_WORDS];
};

struct magAngBus {
  magType data0[BUS_WORDS];
  angType data1[BUS_WORDS];
};

const int kernel[3] = {1, 0, -1};

#endif

