#pragma once
#include "axi4_segment.h"
#include "sysbus_axi_struct.h"

#include "terminal_class.h"

 // register map

#define CHAR_OUT                   0
#define CHAR_IN                    1
#define CHAR_READY                 2

#define SHIFT 2

class uart_if : public sc_module, public sysbus_axi {
public:

  //== Ports

  sc_in<bool>   CCS_INIT_S1(clk);
  sc_in<bool>   CCS_INIT_S1(rst_bar);

  r_slave<>     CCS_INIT_S1(r_uart_if);
  w_slave<>     CCS_INIT_S1(w_uart_if);

  //== Instances

  terminal      screen;

  //== Local signals

  bool chatty = false;

  // interface signals

  sc_signal<ac_int<8 , false> >     char_in;

  sc_uint<64>                       register_bank[256];
  const int                         num_regs    = 256;

  void reg_reader()
  {
    ar_payload ar;
    r_payload  r;
    int        reg_addr;

    r_uart_if.reset();

    wait();

    while (1) {
      r_uart_if.start_multi_read(ar);
      reg_addr = ar.addr >> SHIFT;
      if (chatty) printf("Read from register: %d \n", reg_addr);
      while (1) {
        if (reg_addr > num_regs) {
          SC_REPORT_ERROR("uart_if", "invalid addr");
          r.resp = Enc::XRESP::SLVERR;
        } else {
          r.data = register_bank[reg_addr].to_int64();
          r.resp = Enc::XRESP::OKAY;
        }
        if (!r_uart_if.next_multi_read(ar, r)) break;
      } 

      if (reg_addr == CHAR_IN) {
          unsigned char key_press = screen.get_key();
          register_bank[CHAR_IN] = key_press;
          r.data = key_press;
      }

      register_bank[CHAR_READY] = screen.key_ready(); 

      wait();
    }
  }

  void reg_writer() 
  {
    int     reg_addr;

    w_uart_if.reset();

    wait();

    while(1) {
      aw_payload  aw;
      w_payload   w;
      b_payload   b;

      if (w_uart_if.aw.PopNB(aw)) {

        w = w_uart_if.w.Pop();

        reg_addr = aw.addr >> SHIFT;

        if (chatty) printf("Writing to register: %d = 0x%08x \n", reg_addr, (unsigned int) w.data.to_int64());  

        if (reg_addr > num_regs) {
          SC_REPORT_ERROR("uart_if", "invalid addr");
          b.resp = Enc::XRESP::SLVERR;
        } else {
          register_bank[reg_addr] = w.data.to_uint64();
	  b.resp = Enc::XRESP::OKAY;
          b.id   = aw.id;
	}

        w_uart_if.b.Push(b);
      
        if (reg_addr == CHAR_OUT) {
           screen.send_char(w.data.to_uint64());
        } 
      } 

      wait();
    }
  }

  SC_CTOR(uart_if)
  {
    SC_THREAD(reg_reader);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(reg_writer);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }
};
