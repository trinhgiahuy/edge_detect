mem load -infile sw.edge/bootrom.mem -format hex /testbench/top/riscv/riscv_complex/bootrom/bootrom_russk
mem load -filldata 0 /testbench/top/riscv/code_mem/mem
mem load -infile sw.edge/edge_detect.mem -format hex /testbench/top/riscv/code_mem/mem
add wave -r /*
run 40000 ns
run 5 ms
#run -all
