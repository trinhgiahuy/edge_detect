
RISCV_GCC_HOME   = /home/russk/platforms/catapult_cpu_platforms/riscv-tools/riscv-gcc

AS               = $(RISCV_GCC_HOME)/bin/riscv64-unknown-elf-as
CC               = $(RISCV_GCC_HOME)/bin/riscv64-unknown-elf-gcc
LD               = $(RISCV_GCC_HOME)/bin/riscv64-unknown-elf-ld
OC               = $(RISCV_GCC_HOME)/bin/riscv64-unknown-elf-objcopy

COMMON_SOURCE    = ../../hardware_sources
OBJS             = start.o main.o console.o

ASFLAGS          = -a -march=rv64imafd -mabi=lp64d

CCFLAGS          = -march=rv64imafdc
CCFLAGS         += -mfdiv
CCFLAGS         += -mabi=lp64d
CCFLAGS         += -fPIC 
CCFLAGS         += -I . 
CCFLAGS         += -I $(COMMON_SOURCE)
CCFLAGS         += -D SMALL
CCFLAGS         += -D EMBEDDED 

ifdef DEBUG 
CCFLAGS         += -O0
CCFLAGS         += -g
else
CCFLAGS         += -O3 
endif

LDFLAGS          = -m elf64lriscv 
LDFLAGS         += -L $(RISCV_GCC_HOME)/riscv64-unknown-elf/lib/rv64imafdc/lp64d -lm 

all: edge_detect.mem bootrom.mem

edge_detect.mem : edge_detect.bin
	@echo "OD     edge_detect.bin > edge_detect.mem"
	@od --address-radix=x --output-duplicates --format=x8 --width=8 edge_detect.bin | \
		cut -c7- > edge_detect.mem 

edge_detect.bin: edge_detect.x
	@echo "OC     edge_detect.x -O binary edge_detect.bin"
	@$(OC) -O binary edge_detect.x edge_detect.bin >> make.out

edge_detect.x: $(OBJS)
	@echo "LD     --script link.ld -o edge_detect.x"
	@$(LD) -o edge_detect.x $(OBJS) $(LDFLAGS) --script link.ld -lc -Map edge_detect.map >> make.out

main.o: main.c console.h $(COMMON_SOURCE)/common_stim_results.h $(COMMON_SOURCE)/magnitude_angle_if.h
	@echo "CC     main.c"
	@$(CC) $(CCFLAGS) -o main.o -c main.c >> make.out
	
console.o: console.c
	@echo "CC     console.c"
	@$(CC) $(CCFLAGS) -o console.o -c console.c >> make.out
	
start.o: start.s
	@echo "AS     start.s"
	@$(AS) $(ASFLAGS) -o start.o -c start.s > start.list

bootrom.mem : bootrom.bin
	@echo "OD     bootrom.bin > bootrom.mem"
	@od --address-radix=x --output-duplicates --format=x8 --width=8 bootrom.bin | \
		cut -c7- > temp.mem 
	@cat fat.mem temp.mem > bootrom.mem
	@rm temp.mem

bootrom.bin: bootrom.x
	@echo "OC     bootrom.x -O binary bootrom.bin"
	@$(OC) -O binary bootrom.x bootrom.bin >> make.out

bootrom.x: bootrom.o
	@echo "LD     --script bootrom.ld -o bootrom.x"
	@$(LD) $(LDFLAGS) -o bootrom.x bootrom.o --script bootrom.ld >> make.out

bootrom.o: bootrom.s
	@echo "AS     bootrom.s"
	@$(AS) $(ASFLAGS) -o bootrom.o -c bootrom.s > bootrom.list 


clean:
	@echo "removing created files"
	@rm -f bootrom.mem bootrom.bin bootrom.x bootrom.o bootrom.map bootrom.list edge_detect.mem edge_detect.bin edge_detect.x edge_detect.map $(OBJS) start.list make.out


