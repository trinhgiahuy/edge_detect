#include <systemc.h>

#include "sys_ram.h"
#include "accel_if.h"
#include "uart_if.h"
#include "timer.h"

#include "proc_fabric.h"

#include "axi/axi4.h"
#include "axi4_segment.h"
#include "sysbus_axi_struct.h"

class systemc_subsystem : public sc_module, public sysbus_axi {
public:

  //== Ports

  sc_in<bool>     clk;
  sc_in<bool>     reset_bar;

  r_slave<>       CCS_INIT_S1(r_cpu);
  w_slave<>       CCS_INIT_S1(w_cpu);

  //== Local signals

//r_chan<>        CCS_INIT_S1(r_acc_regs);
//w_chan<>        CCS_INIT_S1(w_acc_regs);

//r_chan<>        CCS_INIT_S1(r_acc_master);
//w_chan<>        CCS_INIT_S1(w_acc_master);

  r_chan<>        CCS_INIT_S1(r_memory);
  w_chan<>        CCS_INIT_S1(w_memory);

  r_chan<>        CCS_INIT_S1(r_uart);
  w_chan<>        CCS_INIT_S1(w_uart);

  r_chan<>        CCS_INIT_S1(r_timer);
  w_chan<>        CCS_INIT_S1(w_timer);

  //== Instances

  fabric          CCS_INIT_S1(io_matrix);
//accel_if        CCS_INIT_S1(go_fast);
  sys_ram         CCS_INIT_S1(shared_memory);
  uart_if         CCS_INIT_S1(uart);
  clock_timer     CCS_INIT_S1(timer);

  //== Constructor

  SC_CTOR(systemc_subsystem)
  {
    io_matrix.clk          (clk);
    io_matrix.rst_bar      (reset_bar);
    io_matrix.r_mem        (r_memory);
    io_matrix.w_mem        (w_memory);
    io_matrix.r_cpu        (r_cpu);
    io_matrix.w_cpu        (w_cpu);
    io_matrix.r_uart       (r_uart);
    io_matrix.w_uart       (w_uart);
    io_matrix.r_timer      (r_timer);
    io_matrix.w_timer      (w_timer);
    io_matrix.r_reg        (r_acc_regs);
    io_matrix.w_reg        (w_acc_regs);
    io_matrix.r_acc        (r_acc_master);
    io_matrix.w_acc        (w_acc_master);

    shared_memory.clk      (clk);
    shared_memory.rst_bar  (reset_bar);
    shared_memory.r_slave0 (r_memory);
    shared_memory.w_slave0 (w_memory);

//  go_fast.clk            (clk);
//  go_fast.rst_bar        (reset_bar);
//  go_fast.r_reg_if       (r_acc_regs);
//  go_fast.w_reg_if       (w_acc_regs);
//  go_fast.r_mem_if       (r_acc_master);
//  go_fast.w_mem_if       (w_acc_master);

    uart.clk               (clk);
    uart.rst_bar           (reset_bar);
    uart.r_uart_if         (r_uart);
    uart.w_uart_if         (w_uart);

    timer.clk              (clk);
    timer.rst_bar          (reset_bar);
    timer.r_timer          (r_timer);
    timer.w_timer          (w_timer);
  }
};
