#pragma once
#include "types.h"
#include "axi4_segment.h"
#include "sysbus_axi_struct.h"

#define SHIFT 2
#define WEIGHT_MEMORY_BASE 0x70000000

class bus_master_if : public sc_module, public sysbus_axi {
 public:

  //== Ports 

  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rstn);

  sc_in<ac_int<12, false> >                height;
  sc_in<ac_int<12, false> >                width;
  sc_in<ac_int<18, false>>                 CCS_INIT_S1(input_offset);
  sc_in<ac_int<18, false>>                 CCS_INIT_S1(output_offset);

  Connections::SyncIn                      CCS_INIT_S1(start);
  Connections::SyncOut                     CCS_INIT_S1(done);
//Bus I/Fs
  r_master<> CCS_INIT_S1(r_master0);
  w_master<> CCS_INIT_S1(w_master0);

//User side
//Read
  Connections::Out<ac_int<64, false>>         CCS_INIT_S1(mem_in_data);
//Write
  Connections::In<ac_int<64, false>>          CCS_INIT_S1(mem_out_data);

  Connections::SyncChannel          CCS_INIT_S1(sync);
  //== Constructor

  SC_CTOR(bus_master_if) {

    SC_THREAD(write_master_process);
    sensitive << clk.pos();
    async_reset_signal_is(rstn, false);

    SC_THREAD(read_master_process);
    sensitive << clk.pos();
    async_reset_signal_is(rstn, false);

    AXI4_W_SEGMENT_BIND(w_segment0, clk, rstn, w_master0);
    AXI4_R_SEGMENT_BIND(r_segment0, clk, rstn, r_master0);
  }

 private:

  // write and read segmenters segment long bursts to conform to AXI4 protocol (which allows 256 beats maximum).
  AXI4_W_SEGMENT(w_segment0)
  AXI4_R_SEGMENT(r_segment0)

  void read_master_process() {
    AXI4_R_SEGMENT_RESET(r_segment0, r_master0);
    mem_in_data.Reset();
    start.Reset();
    sync.reset_sync_out();
    wait();

    while (1) {
      ex_ar_payload ar;

      start.sync_in();
      sync.sync_out();
      ar.addr = uint32(input_offset.read().to_int()); // << SHIFT; //convert to byte address
      ar.addr += WEIGHT_MEMORY_BASE;
      ar.ex_len = height.read()*width.read()/N-1;
      r_segment0_ex_ar_chan.Push(ar);

      #pragma hls_pipeline_init_interval 1
      #pragma pipeline_stall_mode flush
      while (1) {
        r_payload r = r_master0.r.Pop();
        //ac_int<17, false> data;
        //data.set_slc(0,r.data.slc<dtype::width>(0));
        mem_in_data.Push(r.data);
        if (ar.ex_len-- == 0) { break; }
      }
    }
  }

  void write_master_process() {
    AXI4_W_SEGMENT_RESET(w_segment0, w_master0);
    mem_out_data.Reset();
    done.Reset();
    sync.reset_sync_in();
    wait();

    while (1) {
      ex_aw_payload aw;
      sync.sync_in();
      aw.addr = uint32(output_offset.read()); // << SHIFT; // convert to byte address
      aw.ex_len = height.read()*width.read()/N-1;
      aw.addr += WEIGHT_MEMORY_BASE;
      w_segment0_ex_aw_chan.Push(aw);

      #pragma hls_pipeline_init_interval 1
      #pragma pipeline_stall_mode flush
      while (1) {
        w_payload w;
        w.data = mem_out_data.Pop(); // .slc<dtype::width>(0);
        w_segment0_w_chan.Push(w);

        if (aw.ex_len-- == 0) { break; }
      }

      b_payload b = w_segment0_b_chan.Pop();
      done.sync_out();
    }
  }
};
