/**************************************************************************
 *                                                                        *
 *  Edge Detect Design Walkthrough for HLS                                *
 *                                                                        *
 *  Software Version: 1.0                                                 *
 *                                                                        *
 *  Release Date    : Tue Jan 14 15:40:43 PST 2020                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 1.0.0                                               *
 *                                                                        *
 *  Copyright 2020, Mentor Graphics Corporation,                          *
 *                                                                        *
 *  All Rights Reserved.                                                  *
 *  
 **************************************************************************
 *  Licensed under the Apache License, Version 2.0 (the "License");       *
 *  you may not use this file except in compliance with the License.      * 
 *  You may obtain a copy of the License at                               *
 *                                                                        *
 *      http://www.apache.org/licenses/LICENSE-2.0                        *
 *                                                                        *
 *  Unless required by applicable law or agreed to in writing, software   * 
 *  distributed under the License is distributed on an "AS IS" BASIS,     * 
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or       *
 *  implied.                                                              * 
 *  See the License for the specific language governing permissions and   * 
 *  limitations under the License.                                        *
 **************************************************************************
 *                                                                        *
 *  The most recent version of this package is available at github.       *
 *                                                                        *
 *************************************************************************/
#ifndef _INCLUDED_EDGEDETECT_HIERARCHY_H_
#define _INCLUDED_EDGEDETECT_HIERARCHY_H_

#include <ac_fixed.h>
// This is Catapult's math library implementation, see docs for details
#include <ac_math/ac_sqrt.h>
#include <ac_math/ac_atan2_cordic.h>

// Class for fifo-style hierarchical interconnect objects
#include <ac_channel.h>

// Include constant kernel definition
#include "types.h"
#include <mc_scverify.h>

class EdgeDetect_Hierarchy
{
  // Static interconnect channels (FIFOs) between blocks
  ac_channel<gradBus>       dy;
  ac_channel<gradBus>       dx;
  ac_channel<pixelBus>      dat; // channel for passing input pixels to horizontalDerivative

public:
  EdgeDetect_Hierarchy() {}

  //--------------------------------------------------------------------------
  // Function: run
  //   Top interface for data in/out of class. Combines vertical and
  //   horizontal derivative and magnitude/angle computation.
  #pragma hls_design interface
  void CCS_BLOCK(run)(ac_channel<pixelBus>     &dat_in,
                      ac_channel<magAngBus>    &magn_angle,
                      ac_int<widthBits,false>  imageWidth,
                      ac_int<heightBits,false> imageHeight) 
  {
    verticalDerivative(dat_in, dat, dy, imageWidth, imageHeight);
    horizontalDerivative(dat, dx, imageWidth, imageHeight);
    magnitudeAngle(dx, dy, magn_angle, imageWidth, imageHeight);
  }

private:
  //--------------------------------------------------------------------------
  // Function: verticalDerivative
  //   Compute the vertical derivative on the input data
  #pragma hls_design
  void verticalDerivative(ac_channel<pixelBus> &dat_in,
                          ac_channel<pixelBus> &dat_out,
                          ac_channel<gradBus>  &dy,
                          ac_int<widthBits,false>  imageWidth,
                          ac_int<heightBits,false> imageHeight) 
  {
    // Line buffers store pixel line history - Mapped to RAM
    pixelBus line_buf0[IMG_WIDTH/BUS_WORDS];
    pixelBus line_buf1[IMG_WIDTH/BUS_WORDS];
    pixelBus pix0, pix1, pix2;
    gradBus  pix;

    VROW: for (int y = 0; y < IMG_HEIGHT+1; y++) { // One extra iteration to ramp-up window
      VCOL: for (int x = 0; x < IMG_WIDTH/BUS_WORDS; x++) {
        // vertical window of pixels
        pix2 = line_buf1[x];
        pix1 = line_buf0[x];
        if (y <= imageHeight-1) {
          pix0 = dat_in.read(); // Read streaming interface
        }
        line_buf1[x] = pix1; // copy previous line
        line_buf0[x] = pix0; // store current line
        // Boundary condition processing
        if (y == 1) {
          pix2 = pix1; // top boundary (replicate pix1 up to pix2)
        }
        if (y == imageHeight) {
          pix0 = pix1; // bottom boundary (replicate pix1 down to pix0)
        }
        VCOL4: for (int i=0; i<BUS_WORDS; i++) {
        // Calculate derivative
          pix.data[i] = pix2.data[i]*kernel[0] + pix1.data[i]*kernel[1] + pix0.data[i]*kernel[2];
        }
        if (y != 0) { // Write streaming interfaces
          dat_out.write(pix1); // Pass thru original data
          dy.write(pix); // derivative output
        }
        if(x == imageWidth/BUS_WORDS-1)
          break;
      }
      if(y == imageHeight)
        break;
    }
  }

  //Shift by BUS_WORDS
  //Shift register is BUS_WORDS*2 + 1
  void shift_reg(pixelBus din, pixelType pix_buf[BUS_WORDS*2+1]){
    pix_buf[BUS_WORDS*2] = pix_buf[BUS_WORDS*2-BUS_WORDS]; 
    SHIFT:for(int i=0; i<BUS_WORDS; i++){
      pix_buf[BUS_WORDS*2-1-i] = pix_buf[BUS_WORDS-1-i];
      pix_buf[BUS_WORDS-1-i] = din.data[i];
    }
  }
  int clip(int din, int x, ac_int<widthBits,false> imageWidth){
    if(din > BUS_WORDS*2-1 && x==1)
      return BUS_WORDS*2-1;//left boundary
    else if(din == BUS_WORDS-1 && x==imageWidth/BUS_WORDS)
      return BUS_WORDS;//right boundary
    else
      return din;
  }
  //--------------------------------------------------------------------------
  // Function: horizontalDerivative
  //   Compute the horizontal derivative on the input data
#pragma hls_design
  void horizontalDerivative(ac_channel<pixelBus> &dat_in,
                            ac_channel<gradBus>  &dx,
                            ac_int<widthBits,false>  imageWidth,
                            ac_int<heightBits,false> imageHeight) 
  {
    // pixel buffers store pixel history
    pixelType regs[BUS_WORDS*2+1];
    pixelBus pix_buf;
    pixelType pix[3];

    gradBus dx_out;


    HROW: for (int y = 0; y < IMG_HEIGHT; y++) {
      HCOL: for (int x = 0; x < IMG_WIDTH/BUS_WORDS+1; x++) { // One extra iteration to ramp-up window
        if (x <= imageWidth/BUS_WORDS-1) {
          pix_buf = dat_in.read(); // Read streaming interface
        }

        shift_reg(pix_buf,regs);
        
        MCOL4: for (int i=0; i<BUS_WORDS; i++) {
          dx_out.data[i] = 0;
          ac_fixed<9,9,true> p=0;
          MAC: for(int j=0; j<3; j++){
            pix[2-j] = regs[clip(BUS_WORDS*2-j-i,x,imageWidth)];
            p += pix[2-j]*kernel[j];
          }
          dx_out.data[i] = p;
        }
        if (x != 0) { // Write streaming interface
          dx.write(dx_out); // derivative out
        }
        if(x == imageWidth/BUS_WORDS)
          break;
      }
      if(y == imageHeight-1)
        break;
    }
  }

  //--------------------------------------------------------------------------
  // Function: magnitudeAngle
  //   Compute the magnitute and angle based on the horizontal and vertical
  //   derivative results
  #pragma hls_design
  void magnitudeAngle(ac_channel<gradBus>      &dx_in,
                      ac_channel<gradBus>      &dy_in,
                      ac_channel<magAngBus>    &magn_angle,
                      ac_int<widthBits,false>  imageWidth,
                      ac_int<heightBits,false> imageHeight) {
    gradBus dx, dy;
    sqType dx_sq;
    sqType dy_sq;
    sumType sum; // fixed point integer for sqrt
    angType at;
    magAngBus ma;
    sqRtType sq_rt; // square-root return type

    MROW: for (int y = 0; y < IMG_HEIGHT; y++) {
      MCOL: for (int x = 0; x < IMG_WIDTH/BUS_WORDS; x++) {
        dx = dx_in.read();
        dy = dy_in.read();
        MCOL4: for (int i=0; i<BUS_WORDS; i++) {
          dx_sq = dx.data[i] * dx.data[i];
          dy_sq = dy.data[i] * dy.data[i];
          sum = dx_sq + dy_sq;
          // Catapult's math library of sqrt and atan2
          ac_math::ac_sqrt(sum,sq_rt);
          ac_math::ac_atan2_cordic((ac_fixed<9,9>)dy.data[i], (ac_fixed<9,9>) dx.data[i], at);
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

