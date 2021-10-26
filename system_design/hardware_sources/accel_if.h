#pragma once

#include "ac_int.h"
#include "ac_channel.h"

#include "axi4_segment.h"
#include "magnitude_angle.h"
#include "sysbus_axi_struct.h"

#include "sys_accelerator.h"

 // register map

#define GO_REG                     0
#define GO_READY_REG               1
#define DONE_REG                   2
#define DONE_VALID_REG             3
#define IMAGE_OFFSET               4
#define OUTPUT_OFFSET              5
#define HEIGHT                     6
#define WIDTH                      7

class accel_if : public sc_module, public sysbus_axi {
public:

  //== Ports

  sc_in<bool>   CCS_INIT_S1(clk);
  sc_in<bool>   CCS_INIT_S1(rst_bar);

  r_slave<>     CCS_INIT_S1(r_reg_if);
  w_slave<>     CCS_INIT_S1(w_reg_if);

  r_master<>    CCS_INIT_S1(r_mem_if);
  w_master<>    CCS_INIT_S1(w_mem_if);

  //== Instances

  sys_accelerator   CCS_INIT_S1(accel_inst);

  //== Local signals

  bool chatty = false;

    // interface signals

  sc_signal<ac_int<18, false>>      input_offset;
  sc_signal<ac_int<18, false>>      output_offset;
  sc_signal<ac_int<12, false>>      height;
  sc_signal<ac_int<12, false>>      width;

  Connections::SyncChannel          CCS_INIT_S1(start);
  Connections::SyncChannel          CCS_INIT_S1(done);

  sc_uint<64>                       register_bank[8];
  const int                         num_regs    = 8;

  sc_signal<bool>                   go;
  sc_signal<bool>                   idle;
  sc_signal<bool>                   busy;

  //== Local methods

  void reg_reader()
  {
    ar_payload ar;
    r_payload  r;
    int        reg_addr;

    done.reset_sync_in();
    r_reg_if.reset();
    idle.write(1);

    wait();
    
    while (1) {
      r_reg_if.start_multi_read(ar);
      reg_addr = ar.addr >> 3;  // registers are 64-bit wide word addressable
      if (chatty) printf("Read from register: %d \n", reg_addr);
      while (1) {
        if (reg_addr > num_regs) {
          std::string address_error = "invalid addr, attempted access at " + std::to_string(ar.addr);
          SC_REPORT_ERROR("accel_if", address_error.c_str());
          r.resp = Enc::XRESP::SLVERR;
        } else {
          r.data = register_bank[reg_addr].to_int64();
          r.resp = Enc::XRESP::OKAY;
	  r.id   = ar.id;
        }
        if (!r_reg_if.next_multi_read(ar, r)) break;
      } 
      if (reg_addr == DONE_REG) {
        done.sync_in();
        idle.write(1);
      } 

      if (busy) idle.write(0);
      wait();
    }
  }

  void reg_writer() 
  {
    int     reg_addr;

    start.reset_sync_out();
    busy.write(0);
    w_reg_if.reset();

    wait();

    while(1) {
      aw_payload  aw;
      w_payload   w;
      b_payload   b;

      if (w_reg_if.aw.PopNB(aw)) {

        w = w_reg_if.w.Pop();

        reg_addr = aw.addr >> 3;  // registers are 64-bit wide and word addressable

        if (chatty) printf("Writing to register: %d = 0x%08x \n", reg_addr, w.data.to_int());  

        if (reg_addr > num_regs) {
          std::string address_error = "invalid addr, attempted access at " + std::to_string(aw.addr);
          SC_REPORT_ERROR("accel_if", address_error.c_str());
          b.resp = Enc::XRESP::SLVERR;
        } else {
          register_bank[reg_addr] = w.data.to_uint64();
          b.resp = Enc::XRESP::OKAY;
          b.id   = aw.id;
	}

        w_reg_if.b.Push(b);
      
        if (reg_addr == GO_REG) {
          busy.write(1);
          printf("Syncing\n");
          start.sync_out();
          printf("Done Syncing\n");
        } 
      } 

      if (idle) busy.write(0);

      // drive outputs 

      register_bank[GO_READY_REG]    = start.rdy; // !busy.read();
      register_bank[DONE_VALID_REG]  = done.vld.read();  // idle.read();
      register_bank[DONE_REG]        = idle.read();

      wait();
    }
  }

  void set_inputs()
  {
    go =             register_bank[GO_REG];
    input_offset =   register_bank[IMAGE_OFFSET].to_int();
    output_offset =  register_bank[OUTPUT_OFFSET].to_int();
    height =         register_bank[HEIGHT].to_int();
    width =          register_bank[WIDTH].to_int();
  }


  SC_CTOR(accel_if)
  {

    SC_THREAD(reg_reader);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(reg_writer);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    SC_METHOD(set_inputs);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    accel_inst.clk             (clk);
    accel_inst.rstn            (rst_bar);
    
    accel_inst.r_master0       (r_mem_if);
    accel_inst.w_master0       (w_mem_if);

    accel_inst.input_offset    (input_offset);
    accel_inst.output_offset   (output_offset);
    accel_inst.height          (height);
    accel_inst.width           (width);

    accel_inst.start           (start);
    accel_inst.done            (done);

  }
};
