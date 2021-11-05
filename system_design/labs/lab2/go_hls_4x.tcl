#This script runs all of the solution created from following the Lab book steps
#for the modified architecture for a 64-bit bus
project new
solution options defaults
options set Input/SearchPath ../include
options set Output/OutputVHDL false
solution options set /Input/CppStandard c++11
solution options set /Input/CompilerFlags -DCODE_FOR_COV
solution options set /Output/GenerateCycleNetlist false
flow package require /SCVerify
flow package option set /SCVerify/USE_CCS_BLOCK true
flow package option set /SCVerify/INVOKE_ARGS {../image/people_gray.bmp orig1.bmp ba.bmp}
solution file add ./testbench_4x.cpp -type C++
solution file add ../include/bmp_io.cpp -exclude true
go new
go compile
directive set /MagnitudeAngle -TRANSACTION_DONE_SIGNAL false
solution library add nangate-45nm_beh -- -rtlsyntool OasysRTL -vendor Nangate -technology 045nm
go libraries
directive set -CLOCKS {clk {-CLOCK_PERIOD 3.0 -CLOCK_EDGE rising -CLOCK_UNCERTAINTY 0.0 -CLOCK_HIGH_TIME 1.5 -RESET_SYNC_NAME rst -RESET_ASYNC_NAME arst_n -RESET_KIND sync -RESET_SYNC_ACTIVE high -RESET_ASYNC_ACTIVE low -ENABLE_ACTIVE high}}
go assembly
directive set -MERGEABLE false
directive set /MagnitudeAngle/run/ac_math::ac_sqrt<19,19,AC_TRN,AC_WRAP,16,9,AC_TRN,AC_WRAP>:for -UNROLL yes
directive set /MagnitudeAngle/run/ac_math::ac_atan2_cordic<9,9,AC_TRN,AC_WRAP,9,9,AC_TRN,AC_WRAP,8,3,AC_TRN,AC_WRAP>:for -UNROLL yes
directive set /MagnitudeAngle/run/MCOL -PIPELINE_INIT_INTERVAL 1
go architect
go extract

