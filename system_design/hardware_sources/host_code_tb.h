
#include <sstream>

#pragma once
#include "axi4_segment.h"
#include "sysbus_axi_struct.h"

#include "magnitude_angle_if.h"

class host_code_tb : public sc_module, public sysbus_axi {
public:

  //== Ports 

  sc_in<bool>   CCS_INIT_S1(clk);
  sc_in<bool>   CCS_INIT_S1(rst_bar);

  r_master<>    CCS_INIT_S1(r_master);
  w_master<>    CCS_INIT_S1(w_master);

  //== Local methods

#include "common_stim_results.h"

  float *hw_data_out;
  float *sw_data_out;
  unsigned char *sw_image;

  void load_data(unsigned char *dst, unsigned char *src)
  {
    int i;

    for (i=0; i<IMAGE_SIZE; i++) {
      dst[i] = TB_READ_8(src + i);
    }
  }

  void sw_thread()
  {
    w_master.reset();
    r_master.reset();

    wait();

    // inputs are preloaded into memory at 0x70000000

    load_data(sw_image, (unsigned char *) 0x70000000);

    edge_detect_hw((unsigned char *) 0x70000000, hw_data_out);
    edge_detect_sw(sw_image, sw_data_out);

    check_results(sw_data_out, hw_data_out);

    sc_stop();
  }

  //== Constructor

  SC_CTOR(host_code_tb)
  {
    hw_data_out = new float[IMAGE_SIZE*2];
    sw_data_out = new float[IMAGE_SIZE*2];
    sw_image = new unsigned char[IMAGE_SIZE];

    SC_THREAD(sw_thread);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }
};
