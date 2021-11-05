#ifndef _INCLUDED_TYPES_H_
#define _INCLUDED_TYPES_H_

#include <ac_fixed.h>
#include <ac_sync.h>
#include <ac_channel.h>

const int IMG_WIDTH=1920;
const int IMG_HEIGHT=1080;
const int BUS_WORDS = 4;

// Define some bit-accurate types to use in this model
typedef ac_fixed<8,8,true,AC_RND,AC_SAT>  gradType;     // Derivative is max range -255 to 255
typedef ac_fixed<18,18,false>             sqType;       // Result of 9-bit x 9-bit
typedef ac_fixed<19,19,false>             sumType;      // Result of 18-bit + 18-bit fixed pt integer for squareroot
typedef ac_fixed<8,8,false,AC_RND,AC_SAT> magType;      //9-bit unsigned magnitute result
typedef ac_fixed<8,3,true>                angType;      // 3 integer bit, 5 fractional bits for quantized angle -pi to pi
typedef ac_fixed<16,9,false>              sqRtType;

enum{
 widthBits = ac::nbits<IMG_WIDTH>::val,
 heightBits = ac::nbits<IMG_HEIGHT>::val
};
struct gradBus {
  gradType data0[BUS_WORDS];
  gradType data1[BUS_WORDS];
};

struct magAngBus {
  magType data0[BUS_WORDS];
  angType data1[BUS_WORDS];
};

const int kernel[3] = {1, 0, -1};

#endif

