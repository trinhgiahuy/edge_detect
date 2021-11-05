#include <stdio.h>
#include <stdlib.h>

using namespace std;

#include "EdgeDetect_Algorithm.h"
#include "MagnitudeAngle.h"

#include "bmp_io.hpp"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <mc_scverify.h>

CCS_MAIN(int argc, char *argv[])
{

  unsigned int iW = 1296;
  unsigned int iH = 864;
  static EdgeDetect_Algorithm    inst0,inst1;
  static MagnitudeAngle  ma_inst;
  ac_channel<ac_fixed<8,8,true>> dx_in;
  ac_channel<ac_fixed<8,8,true>> dy_in;
  ac_channel<ac_fixed<8,8,false,AC_RND,AC_SAT>>  magn;
  ac_channel<ac_fixed<8,3,true>>  angle;
 
  unsigned long int width = iW;
  long int height         = iH;
  unsigned char *rarray = new unsigned char[iW*iH];
  unsigned char *garray = new unsigned char[iW*iH];
  unsigned char *barray = new unsigned char[iW*iH];

  // allocate buffers for image data
  double *dy = new double[iH*iW];
  double *dx = new double[iH*iW];

  cout << "Loading Input File" << endl;

  if (argc < 3) {
    cout << "Usage: " << argv[0] << " <inputbmp> <outputbmp_alg> <outputbmp_ba>" << endl;
    CCS_RETURN(-1);
  }

  std::string bmpIn(argv[1]);  // input bitmap file
  std::string bmpAlg(argv[2]); // output bitmap (algorithm)
  std::string bmpBA(argv[3]);  // output bitmap (bit-accurate)

  bmp_read((char*)bmpIn.c_str(), &width, &height, &rarray, &garray, &barray);
  assert(width==iW);
  assert(height==iH);

#ifdef DEBUG
  std::string cmd;
  cmd = "display ";
  cmd.append(bmpIn.c_str());
  std::cout << "Display input image file using command: " << cmd << endl;
  std::system(cmd.c_str());
#endif

  uint8 *dat_in = new uint8[iH*iW];
  unsigned char *dat_in_orig = new unsigned char[iH*iW];;
  double *magn_orig = new double[iH*iW];
  double *angle_orig = new double[iH*iW];

  unsigned  cnt = 0;
  for (int y = 0; y < iH; y++) {
    for (int x = 0; x < iW; x++) {
      dat_in[cnt]      = rarray[cnt]; // just using red component (pseudo monochrome)
      dat_in_orig[cnt] = rarray[cnt];
      cnt++;
    }
  }

  cout << "Running" << endl;
  
//Run a  smaller image if doing C++ to RTL verifction so we don't have to wait too long
#ifdef CCS_SCVERIFY
  iH = iW = 100;
#endif
  //Run the orginal algorithm
  inst0.run(dat_in_orig,magn_orig,angle_orig,iW,iH);
  //Compute derivatives in the testbench
  inst1.verticalDerivative(dat_in_orig,dy,iW,iH);
  inst1.horizontalDerivative(dat_in_orig,dx,iW,iH);
 
  for (int y = 0; y < iH; y++) {
    for (int x = 0; x < iW; x++) {
      dx_in.write(dx[y*iW + x]);
      dy_in.write(dy[y*iW + x]);
    }
  }

  ma_inst.run(dx_in,dy_in,magn,angle,iW,iH);
  cnt = 0;
  float sumErr = 0;
  float sumAngErr = 0;
  for (int y = 0; y < iH; y++) {
    for (int x = 0; x < iW; x++) {
      int hw = magn.read().to_int();
      angType angHw = angle.read();
      int alg = (int)*(magn_orig+cnt);
      int diff = alg-(hw);
      int adiff = abs(diff);
      sumErr += adiff;
      float angO = (double)*(angle_orig+cnt);
      float angAdiff = abs(angO-angHw.to_double());
        sumAngErr += angAdiff;
        cnt++;
        rarray[cnt] = hw;   // repurposing 'red' array to the bit-accurate monochrome edge-detect output
        garray[cnt] = alg;  // repurposing 'green' array to the original algorithmic edge-detect output
    }
  }

  printf("Magnitude: average error per pixel %f\n",sumErr/(iH*iW));
  printf("Angle: average per pixel %f\n",sumAngErr/(iH*iW));

  cout << "Writing algorithmic bitmap output to: " << bmpAlg << endl;
  bmp_24_write((char*)bmpAlg.c_str(), iW,  iH, garray, garray, garray);

#ifdef DEBUG
  cmd = "display ";
  cmd.append(bmpAlg.c_str());
  std::cout << "Display output image file using command: " << cmd << endl;
  std::system(cmd.c_str());
#endif

  cout << "Writing bit-accurate bitmap output to: " << bmpBA << endl;
  bmp_24_write((char*)bmpBA.c_str(), iW,  iH, rarray, rarray, rarray);

#ifdef DEBUG
  cmd = "display ";
  cmd.append(bmpBA.c_str());
  std::cout << "Display output image file using command: " << cmd << endl;
  std::system(cmd.c_str());
#endif

  delete (dat_in_orig);
  delete (magn_orig);
  delete (angle_orig);
  //delete (dat_in);
  delete (rarray);
  delete (garray);
  delete (barray);

  cout << "Finished" << endl;

  CCS_RETURN(0);
}
