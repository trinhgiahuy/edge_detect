
# allow for str.c_str() to pass as char*
CFLAGS += -g -std=c++11 
CFLAGS_SP += -g -std=c++11 -DSINGLEPORT 
CFLAGS_CIRC += -g -std=c++11 -DCIRCULAR 

run.exe: types.h EdgeDetect_Algorithm.h EdgeDetect_Hierarchy.h testbench.cpp
	-$(MGC_HOME)/bin/g++ $(CFLAGS) -I $(MGC_HOME)/shared/include -I ../include ../include/bmp_io.cpp testbench.cpp -o $@
.PHONY: run

run: 	run.exe		
	ulimit -S -s 80000 && ./run.exe ../image/people_gray.bmp orig1.bmp ba.bmp

run_sp.exe: types.h EdgeDetect_Algorithm.h EdgeDetect_Hierarchy_Singleport.h testbench.cpp
	-$(MGC_HOME)/bin/g++ $(CFLAGS_SP) -I $(MGC_HOME)/shared/include -I ../include ../include/bmp_io.cpp testbench.cpp -o $@
.PHONY: run_sp

run_sp: run_sp.exe		
	ulimit -S -s 80000 && ./run_sp.exe ../image/people_gray.bmp orig1.bmp ba.bmp

run_circ.exe: types.h EdgeDetect_Algorithm.h EdgeDetect_Hierarchy_Circular.h testbench.cpp
	-$(MGC_HOME)/bin/g++ $(CFLAGS_CIRC) -I $(MGC_HOME)/shared/include -I ../include ../include/bmp_io.cpp testbench.cpp -o $@
.PHONY: run_circ

run_circ: run_circ.exe		
	ulimit -S -s 80000 && ./run_circ.exe ../image/people_gray.bmp orig1.bmp ba.bmp


clean:
	rm -f run.exe run_4x.exe run_sp.exe run_circ.exe *.bmp catapult.log design_checker_constraints.tcl design_checker_pre_build.tcl

