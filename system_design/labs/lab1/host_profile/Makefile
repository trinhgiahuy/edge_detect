
CATAPULT_HOME  ?= /wv/hlsb/CATAPULT/10.6a/PRODUCTION/aol/Mgc_home
EDGE_DETECT     = ../../utils
CXX             = g++
CXX_FLAGS       = -O0 -Wno-write-strings
IMAGE           = $(EDGE_DETECT)/image/people_gray.bmp

all: profile

profile: profile_main $(IMAGE)
	./profile_main $(IMAGE)

profile_main: profile_main.cpp EdgeDetect_Algorithm.h edge_defs.h Makefile
	$(CXX) $(CXX_FLAGS) -o profile_main profile_main.cpp $(CATAPULT_HOME)/shared/include/bmpUtil/bmp_io.cpp -I . -I $(CATAPULT_HOME)/shared/include 

clean:
	rm -f profile_main 

