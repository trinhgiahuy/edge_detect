#pragma once
#include "types.h"
#include "axi4_segment.h"
#include "sysbus_axi_struct.h"
#include "edge_detect_defines.h"

#ifdef INITIALIZE_RAM

#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>
using namespace std;

#endif

class sys_ram : public sc_module, public sysbus_axi {
public:
  sc_in<bool>            CCS_INIT_S1(clk);
  sc_in<bool>            CCS_INIT_S1(rst_bar);
  r_slave<AUTO_PORT>     CCS_INIT_S1(r_slave0);
  w_slave<AUTO_PORT>     CCS_INIT_S1(w_slave0);
/*
*/
  static const int sz = MEM_SIZE; // size in axi_cfg::dataWidth words
  static const int buswidth_bits = 64;
  static const int buswidth_bytes = buswidth_bits / 8;

  //typedef NVUINTW(axi_cfg::dataWidth) arr_t;
  //typedef NVUINTW(buswidth_bits) arr_t;
  typedef ac_int<64, false> arr_t;
  //arr_t array[MEM_SIZE] ;
  arr_t *array;

  bool chatty = false;
  SC_CTOR(sys_ram)
  {
    SC_THREAD(slave_r_process);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(slave_w_process);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
   
    array = new arr_t[MEM_SIZE];  // do this, you must, hmm? Else the stack, overflow, you will. Yes! Hmm?

#ifdef INITIALIZE_RAM

    for (int i=0; i<sz; i++) {
       array[i] = 0;
    }

    load_image();

#else 
    for (int i=0; i<sz; i++) {
       array[i] = 0;
    }
#endif

  }

  NVUINTW(axi_cfg::dataWidth) debug_read_addr(uint32_t addr)
  {
    std::string address_error = "invalid addr, attempted access at " + std::to_string(addr);

    if (addr >= (sz * bytesPerBeat)) {
      SC_REPORT_ERROR("ram", address_error.c_str());
      return 0;
    }

    return(array[addr / bytesPerBeat]);
  }

  void slave_r_process() {

    r_slave0.reset();

    wait();
 
    while(1) {
      ar_payload ar;
      r_slave0.start_multi_read(ar);

      if (chatty) CCS_LOG("ram read  addr: " << std::hex << ar.addr.to_int() << " len: " << ar.len.to_int());
      if (chatty) printf("RAM read: Address: %08x \n", ar.addr.to_int());

      while (1) {
        r_payload r;

        if (ar.addr >= (sz * bytesPerBeat)) {
          std::string address_error = "invalid addr, attempted access at " + std::to_string(ar.addr);

          SC_REPORT_ERROR("ram", address_error.c_str());
          r.resp = Enc::XRESP::SLVERR;

        } else {
          r.data = array[ar.addr / bytesPerBeat].to_uint64();
          r.id = ar.id;
          r.resp = Enc::XRESP::OKAY;
          if (chatty) printf("READ Addr: %08x  Data: %08llx \n", ar.addr.to_int(), r.data.to_uint64());
	}

        if (!r_slave0.next_multi_read(ar, r)) break;
      } 
    }
  }

  void slave_w_process() {

    w_slave0.reset();

    wait();

    while(1) {
      aw_payload aw;
      b_payload b;

      w_slave0.start_multi_write(aw, b);

      if (chatty) CCS_LOG("ram write addr: " << std::hex << aw.addr << " len: " << aw.len);

      while (1) {
        w_payload w = w_slave0.w.Pop();

        if (aw.addr >= (sz * bytesPerBeat)) {
          std::string address_error = "invalid addr, attempted access at " + std::to_string(aw.addr);

          SC_REPORT_ERROR("ram", address_error.c_str());
          b.resp = Enc::XRESP::SLVERR;
          b.id = aw.id;

        } else {
          decltype(w.wstrb) all_on{~0};

          if (w.wstrb == all_on) {
            array[aw.addr / bytesPerBeat] = w.data.to_uint64();
            if (chatty) printf("WRITE(1) Addr: %08x  Data: %08llx \n", aw.addr.to_int(), w.data.to_uint64());
          } else {
	    if (chatty) CCS_LOG("write strobe enabled");
            arr_t orig  = array[aw.addr / bytesPerBeat];
	    arr_t wdata = w.data.to_uint64();

            if (chatty) printf("orig: %016llx \n", orig.to_uint64());
	    #pragma unroll
            for (int i=0; i<WSTRB_WIDTH; i++) {
              if (w.wstrb[i]) {
                orig = nvhls::set_slc(orig, nvhls::get_slc<8>(wdata, (i*8)), (i*8));
                if (chatty) printf("setting byte at address %08x (aw.addr=%08x i=%d) to %02x \n", bytesPerBeat * (aw.addr.to_int()/bytesPerBeat) + i, aw.addr.to_int(), i, nvhls::get_slc<8>(wdata, (i*8)).to_int());
                if (chatty) printf("new orig (after setting byte): %016llx \n", orig.to_uint64());
              }
            }

            if (chatty) printf("WRITE(2) Addr: %08x  Data: %08llx \n", aw.addr.to_int(), orig.to_uint64());
            array[aw.addr / bytesPerBeat] = orig;
          }
	}

        if (!w_slave0.next_multi_write(aw)) break;
      } 

      w_slave0.b.Push(b);
    }
  }

#ifdef INITIALIZE_RAM

  long get_address(char *s)
  {
    return strtoul(s, 0, 16) / buswidth_bytes;
  }

  void trim(char *s)
  {
    char *data_start;
    char *data_end;
    char *sp = s;

    while (*sp != ':') sp++;
    sp++;
    while (isspace(*sp)) sp++;
    data_start = sp;
    while (isxdigit(*sp)) sp++;
    data_end = sp; 
    
    for (sp=data_start; sp<data_end; sp++) *s++ = *sp;
    *s = 0;
  }   

  void load(char* dir, const char* base_filename, int offset)
  {
    int    addr;
    bool   chatty = false;
    FILE  *f;
    char   filename[2000];
    char  *p;
    int    count = 0;
   
    strcpy(filename, dir);
    strcat(filename, "/");
    strcat(filename, base_filename);
    strcat(filename, ".mem");

    cout << "Opening file \"" << filename << "\"" << endl;
    f = fopen(filename, "r");

    if (!f) {
      std::string err_str("Unable to open \"");
      err_str.append(filename);
      err_str.append("\" for reading."); 
      SC_REPORT_ERROR(this->name(), "Error opening file for RAM initialization");
      cout << err_str << endl;
      return;
    } 

    addr = 0;
    cout << "Reading " << base_filename << "... " << endl;

    while ((!feof(f)) && (count < IMAGE_SIZE)) {

      char s[1000];
      char *sp = s;
      char byte_str[3];
      unsigned int value;
      int byte = 0;

      p = fgets(s, sizeof(s), f);
      if (p) {
        addr = get_address(s);
        trim(s);
        if (chatty) printf("%08x: %s \n", addr, s);
        while (strlen(s)) {
           sp = s + strlen(s) - 2;
           byte_str[0] = sp[0];
           byte_str[1] = sp[1];
           byte_str[2] = 0;
           *sp = 0;
           value = strtoul(byte_str, 0, 16); 
           array[offset + addr].set_slc(8 * byte, (uint8) value);
           byte++;
           count++;
        }
      }
    }

    fclose(f);

    cout << count << " values loaded " << endl;

    return;
  }

  void load_image()
  {
    char *dir_ptr = getenv("FILE_IO_DIR");

    if (dir_ptr == NULL) {
       cout << "Please set environment variable \"FILE_IO_DIR\" to where the image file is located" << endl;
       SC_REPORT_ERROR(this->name(), "Unable to load image data. Set environment variable \"FILE_IO_DIR\".");
       return;
    }

    load(dir_ptr, "image", 0);

    cout << "Memory initialization complete. " << endl;
  }
#endif
};
