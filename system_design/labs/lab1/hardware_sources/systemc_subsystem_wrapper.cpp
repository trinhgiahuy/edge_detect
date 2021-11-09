#include <systemc.h>
//#include "types.h"

#include "axi/axi4.h"
#include "axi4_segment.h"
#include "sysbus_axi_struct.h"

#include "systemc_subsystem.h"

class systemc_subsystem_wrapper : public sc_module, public sysbus_axi {
public:

  //== Ports

  sc_in <bool>            clk;
  sc_in <bool>            reset_bar;

  sc_in <sc_lv<44>>       aw_msg_port;
  sc_in <bool>            aw_valid_port;
  sc_out<bool>            aw_ready_port;

  sc_in <sc_lv<73>>       w_msg_port;
  sc_in <bool>            w_valid_port;
  sc_out<bool>            w_ready_port;

  sc_out<sc_lv<6>>        b_msg_port;
  sc_out<bool>            b_valid_port;
  sc_in <bool>            b_ready_port;

  sc_in <sc_lv<44>>       ar_msg_port;
  sc_in <bool>            ar_valid_port;
  sc_out<bool>            ar_ready_port;

  sc_out<sc_lv<71>>       r_msg_port;
  sc_out<bool>            r_valid_port;
  sc_in <bool>            r_ready_port;

  //== Local signals
  
  sc_clock        connections_clk;
  sc_event        check_event;

  //== Local methods

  virtual void start_of_simulation() {
    Connections::get_sim_clk().add_clock_alias(
      connections_clk.posedge_event(), clk.posedge_event());
  }

  void check_clock() { check_event.notify(2, SC_PS);} // Let SC and Vlog delta cycles settle.

  void check_event_method() {
    if (connections_clk.read() == clk.read()) return;
    CCS_LOG("clocks misaligned!:"  << connections_clk.read() << " " << clk.read());
  }

  //== Instances

  systemc_subsystem CCS_INIT_S1(scs);

  //== Constructor

  SC_CTOR(systemc_subsystem_wrapper) 
   : connections_clk("connections_clk", CLOCK_PERIOD, SC_NS, 0.5, 0, SC_NS, true)
  {
    SC_METHOD(check_clock);
    sensitive << connections_clk;
    sensitive << clk;

    SC_METHOD(check_event_method);
    sensitive << check_event;

    scs.clk(connections_clk);
    scs.reset_bar(reset_bar);

    scs.w_cpu.aw.dat(aw_msg_port);
    scs.w_cpu.aw.vld(aw_valid_port);
    scs.w_cpu.aw.rdy(aw_ready_port);

    scs.w_cpu.w.dat(w_msg_port);
    scs.w_cpu.w.vld(w_valid_port);
    scs.w_cpu.w.rdy(w_ready_port);

    scs.w_cpu.b.dat(b_msg_port);
    scs.w_cpu.b.vld(b_valid_port);
    scs.w_cpu.b.rdy(b_ready_port);

    scs.r_cpu.ar.dat(ar_msg_port);
    scs.r_cpu.ar.vld(ar_valid_port);
    scs.r_cpu.ar.rdy(ar_ready_port);

    scs.r_cpu.r.dat(r_msg_port);
    scs.r_cpu.r.vld(r_valid_port);
    scs.r_cpu.r.rdy(r_ready_port);
  }
};

#ifdef QUESTA
SC_MODULE_EXPORT(systemc_subsystem_wrapper);
#endif
