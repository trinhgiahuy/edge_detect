#pragma once
#include "axi4_segment.h"
#include "sysbus_axi_struct.h"

 // register map

#define TIME                       0
#define RESET                      1

#define SHIFT 2

class clock_timer : public sc_module, public sysbus_axi {
public:

  //== Ports

  sc_in<bool>   CCS_INIT_S1(clk);
  sc_in<bool>   CCS_INIT_S1(rst_bar);

  r_slave<>     CCS_INIT_S1(r_timer);
  w_slave<>     CCS_INIT_S1(w_timer);

  //== Local signals

  bool chatty = false;

  sc_uint<64>                       register_bank[2];
  const int                         num_regs    = 2;

  void reg_reader()
  {
    ar_payload ar;
    r_payload  r;
    int        reg_addr;

    r_timer.reset();

    wait();

    while (1) {
      r_timer.start_multi_read(ar);
      reg_addr = ar.addr >> SHIFT;
      if (chatty) printf("Read from register: %d \n", reg_addr);
      while (1) {
        if (reg_addr > num_regs) {
          SC_REPORT_ERROR("clock_timer", "invalid addr");
          r.resp = Enc::XRESP::SLVERR;
        } else {
          r.data = register_bank[reg_addr].to_int64();
          r.resp = Enc::XRESP::OKAY;
        }
        if (!r_timer.next_multi_read(ar, r)) break;
      } 

      wait();
    }
  }

  void reg_writer() 
  {
    int     reg_addr;

    w_timer.reset();

    wait();

    while(1) {
      aw_payload  aw;
      w_payload   w;
      b_payload   b;

      if (w_timer.aw.PopNB(aw)) {

        w = w_timer.w.Pop();

        reg_addr = aw.addr >> SHIFT;

        if (chatty) printf("Writing to register: %d = 0x%08x \n", reg_addr, (unsigned int) w.data.to_int64());  

        if (reg_addr > num_regs) {
          SC_REPORT_ERROR("clock_timer", "invalid addr");
          b.resp = Enc::XRESP::SLVERR;
        } else {
          // no writable registers in this module
          // register_bank[reg_addr] = w.data.to_uint64();
	  b.resp = Enc::XRESP::OKAY;
          b.id   = aw.id;
	}

        if (reg_addr == RESET) {
           register_bank[TIME] = 0;
        } 
      } 

      wait();
    }
  }

  void tic_toc()
  {
    for (int i=0; i<num_regs; i++) register_bank[i] = 0;

    wait();

    while(1) {
      register_bank[TIME]++;
      wait();
    }
  }

  SC_CTOR(clock_timer)
  {
    SC_THREAD(reg_reader);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(reg_writer);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(tic_toc);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }
};
