#include <stdio.h>
#include <stdlib.h>

using namespace std;

#include "EdgeDetect_Algorithm.h"
//#include "EdgeDetect_BitAccurate.h"

#include "bmpUtil/bmp_io.hpp"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <mc_scverify.h>

CCS_MAIN(int argc, char *argv[])
{
  const int iW = 1296;
  const int iH = 864;
  EdgeDetect_Algorithm    inst0;
  //EdgeDetect_BitAccurate  inst1;

  unsigned long int width = iW;
  long int height         = iH;
  unsigned char *rarray = new unsigned char[iW*iH];
  unsigned char *garray = new unsigned char[iW*iH];
  unsigned char *barray = new unsigned char[iW*iH];

  cout << "Loading Input File" << endl;

  if (argc != 2) {
    cout << "Usage: " << argv[0] << " <inputbmp> " << endl;
    CCS_RETURN(-1);
  }

  std::string bmpIn(argv[1]);  // input bitmap file
  //std::string bmpAlg(argv[2]); // output bitmap (algorithm)
  //std::string bmpBA(argv[3]);  // output bitmap (bit-accurate)

  bmp_read((char*)bmpIn.c_str(), &width, &height, &rarray, &garray, &barray);
  // assert(width==iW);
  // assert(height==iH);

#ifdef DEBUG
  std::string cmd;
  cmd = "display ";
  cmd.append(bmpIn.c_str());
  std::cout << "Display input image file using command: " << cmd << endl;
  std::system(cmd.c_str());
#endif

  // uint8 *dat_in = new uint8[iH*iW];
  // uint9 *magn = new uint9[iH*iW];
  // ac_fixed<8,3> *angle = new ac_fixed<8,3>[iH*iW];;
  unsigned char *dat_in_orig = new unsigned char[iH*iW];;
  double *magn_orig = new double[iH*iW];
  double *angle_orig = new double[iH*iW];

  unsigned  cnt = 0;
  for (int y = 0; y < iH; y++) {
    for (int x = 0; x < iW; x++) {
      //dat_in[cnt]      = rarray[cnt]; // just using red component (pseudo monochrome)
      dat_in_orig[cnt] = rarray[cnt];
      cnt++;
    }
  }

  cout << "Running" << endl;

  inst0.run(dat_in_orig,magn_orig,angle_orig,iW,iH);
  //inst1.run(dat_in,magn,angle);

  cnt = 0;
  float sumErr = 0;
  float sumAngErr = 0;
  for (int y = 0; y < iH; y++) {
    for (int x = 0; x < iW; x++) {
      // int hw = *(magn+cnt);
       int alg = (int)*(magn_orig+cnt);
      // int diff = alg-hw;
      // int adiff = abs(diff);
      // sumErr += adiff;
      // float angO = (double)*(angle_orig+cnt);
      // float angHw = (*(angle+cnt)).to_double();
      // float angAdiff = abs(angO-angHw);
      // sumAngErr += angAdiff;
      cnt++;
      //rarray[cnt] = hw;   // repurposing 'red' array to the bit-accurate monochrome edge-detect output
      garray[cnt] = alg;  // repurposing 'green' array to the original algorithmic edge-detect output
    }
  }

  // printf("Magnitude: Manhattan norm per pixel %f\n",sumErr/(iH*iW));
  // printf("Angle: Manhattan norm per pixel %f\n",sumAngErr/(iH*iW));

  //cout << "Writing algorithmic bitmap output to: " << bmpAlg << endl;
  //bmp_24_write((char*)bmpAlg.c_str(), iW,  iH, garray, garray, garray);

#ifdef DEBUG
  //cmd = "display ";
  //cmd.append(bmpAlg.c_str());
  //std::cout << "Display output image file using command: " << cmd << endl;
  //std::system(cmd.c_str());
#endif

  //cout << "Writing bit-accurate bitmap output to: " << bmpBA << endl;
  //bmp_24_write((char*)bmpBA.c_str(), iW,  iH, rarray, rarray, rarray);

#ifdef DEBUG
  //cmd = "display ";
  //cmd.append(bmpBA.c_str());
  //std::cout << "Display output image file using command: " << cmd << endl;
  //std::system(cmd.c_str());
#endif

  //==========================================
  // Save inputs in Questa memory image file 
  //==========================================

  int memory_width = 8;  // bytes per bus cycle

  FILE *memory_image = fopen("image.mem", "w");
  if (!memory_image) {
    fprintf(stderr, "Unable to open file \"image.mem\" for writing \n");
    perror(argv[0]);
    return(-1);
  }

  for (int i=0; i<cnt; i+=memory_width) {
    fprintf(memory_image, "%08x: ", i);
    for (int j=0; j<memory_width; j++) {
      unsigned char value;
      int offset = (memory_width - 1) - j;
      if (i + offset < cnt) value = dat_in_orig[i + offset];
      else                  value = 0;
      fprintf(memory_image, "%02x", value);
    }
    fprintf(memory_image, " \n");
  }

  fclose(memory_image);

  //==========================================
  // Save outputs in binary float file
  //==========================================

  FILE *expected_results = fopen("expected_results.bin", "w");
  if (!expected_results) {
    fprintf(stderr, "Unable to open file \"expected_results.bin\" for writing \n");
    perror(argv[0]);
    return(-1);
  }

  for (int i=0; i<iW*iH; i++) {
    float f[2];
    f[0] = magn_orig[i];
    f[1] = angle_orig[i];
    fwrite(f, sizeof(f[0]), 2, expected_results);
  }

  fclose(expected_results);

  //===========================================

  delete (dat_in_orig);
  delete (magn_orig);
  delete (angle_orig);
  // delete (dat_in);
  // delete (magn);
  // delete (angle);
  delete (rarray);
  delete (garray);
  delete (barray);

  cout << "Finished" << endl;

  CCS_RETURN(0);
}
