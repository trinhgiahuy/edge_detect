solution options defaults
options set Input/SearchPath ../include
options set Output/OutputVHDL false
solution options set /Input/CppStandard c++11
solution options set /Input/CompilerFlags -DCODE_FOR_COV
solution options set /Output/GenerateCycleNetlist false
flow package require /SCVerify
flow package option set /SCVerify/USE_CCS_BLOCK true
flow package option set /SCVerify/INVOKE_ARGS {../image/people_gray.bmp orig1.bmp ba.bmp}
solution file add ./testbench.cpp -type C++
solution file add ../include/bmp_io.cpp -exclude true
go new
go compile
directive set /MagnitudeAngle -TRANSACTION_DONE_SIGNAL false
solution library add nangate-45nm_beh -- -rtlsyntool OasysRTL -vendor Nangate -technology 045nm
go libraries
directive set -CLOCKS {clk {-CLOCK_PERIOD 3.0 -CLOCK_EDGE rising -CLOCK_UNCERTAINTY 0.0 -CLOCK_HIGH_TIME 1.5 -RESET_SYNC_NAME rst -RESET_ASYNC_NAME arst_n -RESET_KIND sync -RESET_SYNC_ACTIVE high -RESET_ASYNC_ACTIVE low -ENABLE_ACTIVE high}}
go assembly
directive set -MERGEABLE false
go architect
go extract

