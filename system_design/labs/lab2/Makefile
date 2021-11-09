
# allow for str.c_str() to pass as char*
CFLAGS += -O3 -std=c++11 

run.exe: types.h EdgeDetect_Algorithm.h MagnitudeAngle.h testbench.cpp
	-$(MGC_HOME)/bin/g++ $(CFLAGS) -I $(MGC_HOME)/shared/include -I ../include ../include/bmp_io.cpp testbench.cpp -o $@
.PHONY: run

run: 	run.exe		
	ulimit -S -s 80000 && ./run.exe ../image/people_gray.bmp orig1.bmp ba.bmp

run_4x.exe: types.h EdgeDetect_Algorithm.h MagnitudeAngle_4x.h testbench_4x.cpp
	-$(MGC_HOME)/bin/g++ $(CFLAGS) -I $(MGC_HOME)/shared/include -I ../include ../include/bmp_io.cpp testbench_4x.cpp -o $@
.PHONY: run_4x

run_4x: run_4x.exe		
	ulimit -S -s 80000 && ./run_4x.exe ../image/people_gray.bmp orig1.bmp ba.bmp

clean:
	rm -f run.exe run_4x.exe *.bmp

