
RISCV_GCC_HOME   = /home/russk/platforms/catapult_cpu_platforms/riscv-tools/riscv-gcc
#RISCV_GCC_HOME   = /opt/riscv

AS               = $(RISCV_GCC_HOME)/bin/riscv64-unknown-elf-as
CC               = $(RISCV_GCC_HOME)/bin/riscv64-unknown-elf-gcc
LD               = $(RISCV_GCC_HOME)/bin/riscv64-unknown-elf-ld
OC               = $(RISCV_GCC_HOME)/bin/riscv64-unknown-elf-objcopy

COMMON_SOURCE    = ../../hardware_sources
OBJS             = start.o main.o console.o

#ASFLAGS          = -a -march=rv64imafdc -mabi=lp64d
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

TARGET           = edge_detect

all: $(TARGET).mem bootrom.mem

$(TARGET).mem : $(TARGET).bin
	@echo "OD     $(TARGET).bin > $(TARGET).mem"
	@od --address-radix=x --output-duplicates --format=x8 --width=8 $(TARGET).bin | \
		cut -c7- > $(TARGET).mem 

$(TARGET).bin: $(TARGET).x
	@echo "OC     $(TARGET).x -O binary $(TARGET).bin"
	@$(OC) -O binary $(TARGET).x $(TARGET).bin >> make.out

$(TARGET).x: $(OBJS)
	@echo "LD     --script link.ld -o $(TARGET).x"
	@$(LD) -o $(TARGET).x $(OBJS) $(LDFLAGS) --script link.ld -lc -Map $(TARGET).map >> make.out

main.o: main.c console.h 
	@echo "CC     main.c"
	@$(CC) $(CCFLAGS) -o main.o -c main.c >> make.out
	
edge_detect_sw.o: edge_detect_sw.c console.h 
	@echo "CC     edge_detect_sw.c"
	@$(CC) $(CCFLAGS) -o edge_detect_sw.o -c edge_detect_sw.c >> make.out
	
edge_detect_hw.o: edge_detect_hw.c console.h 
	@echo "CC     edge_detect_hw.c"
	@$(CC) $(CCFLAGS) -o edge_detect_hw.o -c edge_detect_hw.c >> make.out
	
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
	@rm -f bootrom.mem bootrom.bin bootrom.x bootrom.o bootrom.map bootrom.list $(TARGET).mem $(TARGET).bin $(TARGET).x $(TARGET).map $(OBJS) start.list make.out


