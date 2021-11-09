
CATAPULT_HOME  ?= /wv/hlsb/CATAPULT/10.6a/PRODUCTION/aol/Mgc_home

DATA_DIR        = ../data
HLS_DIR         = .
IMAGE           = $(HLS_DIR)/image/people_gray.bmp

all: $(DATA_DIR)/image.mem $(DATA_DIR)/expected_results.bin

$(DATA_DIR)/image.mem: make_stim_response $(IMAGE)
	./make_stim_response     $(IMAGE)
	mv image.mem             $(DATA_DIR)
	mv expected_results.bin  $(DATA_DIR)

$(DATA_DIR)/expected_results.bin: make_stim_response $(IMAGE) $(HLS_DIR)/EdgeDetect_Algorithm.h
	./make_stim_response     $(IMAGE)
	mv image.mem             $(DATA_DIR)
	mv expected_results.bin  $(DATA_DIR)

make_stim_response: make_stim_response.cpp 
	g++ -o make_stim_response make_stim_response.cpp $(CATAPULT_HOME)/shared/include/bmpUtil/bmp_io.cpp -I $(HLS_DIR) -I $(CATAPULT_HOME)/shared/include 

clean:
	rm -f make_stim_response $(DATA_DIR)/image.mem $(DATA_DIR)/expected_results.bin
