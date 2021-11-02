#ifndef _INCLUDED_DEFINES_H_
#define _INCLUDED_DEFINES_H_

// must be defines for risc-v compiler

#define NUM_PAR_OUTPUTS        2
#define KSIZE                  3
#define KWIDTH                 3
#define MAX_HEIGHT           416
#define MAX_WIDTH            416
#define IN_FMAP             1024
#define OUT_FMAP            1024

enum {
  IMG_SIZE                   = MAX_HEIGHT * MAX_WIDTH,  //Max feature map size
  KSIZESQ                    = KSIZE * KSIZE,           //number of kernel elements
  IF_KSIZE                   = KSIZESQ * IN_FMAP,       //number of kernel elements per output channel
  MEM_SIZE                   = 20000000,                //Storage size for input/output fmaps and kernels
  WEIGHT_OFFSET              = IMG_SIZE * 16 + IMG_SIZE * 16 / 4,
  WEIGHT_SIZE                =    3 *   16 * 9 + 
                                 16 *   32 * 9 + 
                                 32 *   64 * 9 + 
                                 64 *  128 * 9 + 
                                128 *  256 * 9 + 
                                256 *  512 * 9 + 
                                512 * 1024 * 9 + 
                               1024 * 1024 * 9 + 
                               1024 *  125 * 1,
  MEM_OFFSET                 = IMG_SIZE*16/4,
  BIAS_SIZE                  = 16 + 32 + 64 + 128 + 256 + 512 + 1024 + 1024 + 125,
  BIAS_OFFSET                = WEIGHT_OFFSET + WEIGHT_SIZE,
  DATA_OFFSET                = 0,
  EXPECTED_RESULTS_OFFSET    = MEM_SIZE - (13 * 13 * 125) - 1
};

#endif

