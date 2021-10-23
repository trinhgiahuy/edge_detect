#include <stdio.h>
#include <stdlib.h>

using namespace std;

#include "EdgeDetect_Algorithm.h"

#include "bmpUtil/bmp_io.hpp"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <mc_scverify.h>

main(int argc, char *argv[])
{
  const int iW = 1296;
  const int iH = 864;
  EdgeDetect_Algorithm    edge_detect;

  unsigned long  width      = iW;
  long int       height     = iH;
  unsigned char *rarray     = new unsigned char[iW*iH];
  unsigned char *garray     = new unsigned char[iW*iH];
  unsigned char *barray     = new unsigned char[iW*iH];
  double        *magnitude  = new double[iH*iW];
  double        *angle      = new double[iH*iW];

  cout << "Loading Input File" << endl;

  if (argc != 2) {
    cout << "Usage: " << argv[0] << " <inputbmp> " << endl;
    return(-1);
  }

  //std::string bmpIn(argv[1]);  // input bitmap file

  //bmp_read((char*)bmpIn.c_str(), &width, &height, &rarray, &garray, &barray);
  bmp_read(argv[1], &width, &height, &rarray, &garray, &barray);

  cout << "Running" << endl;

  edge_detect.run(rarray,magnitude,angle,iW,iH);

  delete (magnitude);
  delete (angle);
  delete (rarray);
  delete (garray);
  delete (barray);

  cout << "Finished" << endl;

  return(0);
}
