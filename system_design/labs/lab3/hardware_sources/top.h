#pragma once

#include "systemc_subsystem.h"
#include "host_code_tb.h"

class top : public sc_module, public sysbus_axi {
public:

  //== Ports

  sc_in<bool>        CCS_INIT_S1(clk);
  sc_in<bool>        CCS_INIT_S1(reset_bar);

  //== Instances

  systemc_subsystem  CCS_INIT_S1(sc_design);
  host_code_tb       CCS_INIT_S1(sw);

  //== Local signals

  r_chan<>           r_cpu;
  w_chan<>           w_cpu;

  //== Constructor

  SC_CTOR(top) 
  :  r_cpu("r_cpu"),
     w_cpu("w_cpu")
  {
     sc_design.clk(clk);
     sc_design.reset_bar(reset_bar);

     sc_design.r_cpu(r_cpu);
     sc_design.w_cpu(w_cpu);

     sw.clk(clk);
     sw.rst_bar(reset_bar);

     sw.r_master(r_cpu);
     sw.w_master(w_cpu);
  }
};

