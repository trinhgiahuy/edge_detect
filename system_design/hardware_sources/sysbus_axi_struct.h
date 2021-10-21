#ifndef __INCLUDED_SYS_AXI_STRUCT_H__
#define __INCLUDED_SYS_AXI_STRUCT_H__

struct sysbus_axi4_config {
  enum {
    dataWidth = 64,
    useVariableBeatSize = 0,
    useMisalignedAddresses = 0,
    useLast = 1,
    useWriteStrobes = 1,
    useBurst = 1, useFixedBurst = 0, useWrapBurst = 0, maxBurstSize = 256,
    useQoS = 0, useLock = 0, useProt = 0, useCache = 0, useRegion = 0,
    aUserWidth = 0, wUserWidth = 0, bUserWidth = 0, rUserWidth = 0,
    addrWidth = 32,
    idWidth = 4,
    useWriteResponses = 1,
  };
};

typedef axi::axi4_segment<sysbus_axi4_config> sysbus_axi;
typedef axi::axi4_segment<axi::cfg::standard> local_axi64;
typedef typename axi::axi4<axi::cfg::lite_nowstrb> local_axi4_lite;
typedef typename axi::axi4_segment<axi::cfg::lite_nowstrb> local_axi4_lite_segment;

#endif

