solution options defaults
options set Input/SearchPath ../include
options set Output/OutputVHDL false
solution options set /Input/CppStandard c++11
solution options set /Input/CompilerFlags -DCODE_FOR_COV
solution options set /Output/GenerateCycleNetlist false
solution options set ComponentLibs/SearchPath ./lib -append
flow package require /SCVerify
flow package option set /SCVerify/USE_CCS_BLOCK true
flow package option set /SCVerify/INVOKE_ARGS {../image/people_gray.bmp orig1.bmp ba.bmp}
flow package option set /LowPower/SWITCHING_ACTIVITY_TYPE saif
solution file add ./testbench.cpp -type C++
solution file add ../include/bmp_io.cpp -exclude true
go new
options set Input/CompilerFlags -DCIRCULAR
go compile

solution library add nangate-45nm_beh -- -rtlsyntool OasysRTL -vendor Nangate -technology 045nm
solution library add ram_1k_64_sp
go libraries
directive set -CLOCKS {clk {-CLOCK_PERIOD 3.0 -CLOCK_EDGE rising -CLOCK_UNCERTAINTY 0.0 -CLOCK_HIGH_TIME 1.5 -RESET_SYNC_NAME rst -RESET_ASYNC_NAME arst_n -RESET_KIND sync -RESET_SYNC_ACTIVE high -RESET_ASYNC_ACTIVE low -ENABLE_ACTIVE high}}
go assembly
directive set -MERGEABLE false
go assembly
directive set /EdgeDetect_Hierarchy/verticalDerivative/core/line_buf0.data:rsc -MAP_TO_MODULE ram_1k_64_sp.ram_1k_64_sp
directive set /EdgeDetect_Hierarchy/verticalDerivative/core/line_buf1.data:rsc -MAP_TO_MODULE ram_1k_64_sp.ram_1k_64_sp
directive set /EdgeDetect_Hierarchy/verticalDerivative/core/VCOL -PIPELINE_INIT_INTERVAL 1
directive set /EdgeDetect_Hierarchy/verticalDerivative/core/VCOL4 -UNROLL yes
directive set /EdgeDetect_Hierarchy/horizontalDerivative/core/HCOL -PIPELINE_INIT_INTERVAL 1
directive set /EdgeDetect_Hierarchy/horizontalDerivative/core/SHIFT -UNROLL yes
directive set /EdgeDetect_Hierarchy/horizontalDerivative/core/MCOL4 -UNROLL yes
directive set /EdgeDetect_Hierarchy/horizontalDerivative/core/MAC -UNROLL yes
directive set /EdgeDetect_Hierarchy/imageWidth:rsc -MAP_TO_MODULE {[DirectInput]}
directive set /EdgeDetect_Hierarchy/imageHeight:rsc -MAP_TO_MODULE {[DirectInput]}
directive set /EdgeDetect_Hierarchy/magnitudeAngle/core/MCOL -PIPELINE_INIT_INTERVAL 1
directive set /EdgeDetect_Hierarchy/magnitudeAngle/core/MCOL4 -UNROLL yes
directive set /EdgeDetect_Hierarchy/magnitudeAngle/core/ac_math::ac_sqrt<17,17,AC_TRN,AC_WRAP,16,9,AC_TRN,AC_WRAP>:for -UNROLL yes
directive set /EdgeDetect_Hierarchy/magnitudeAngle/core/ac_math::ac_atan2_cordic<9,9,AC_TRN,AC_WRAP,9,9,AC_TRN,AC_WRAP,8,3,AC_TRN,AC_WRAP>:for -UNROLL yes
directive set /EdgeDetect_Hierarchy/dy:cns -FIFO_DEPTH 2
directive set /EdgeDetect_Hierarchy/dx:cns -FIFO_DEPTH 0
directive set /EdgeDetect_Hierarchy/dat:cns -FIFO_DEPTH 0

 go architect
 go extract

go switching
