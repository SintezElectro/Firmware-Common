//==============================================================================
//header:

#ifndef _DEVICE_COMMON_
#define _DEVICE_COMMON_
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "Abstractions/xDevice/xDevice.h"
#include "Abstractions/xPort/xPort.h"
#include "CAN_Local-Types.h"
//==============================================================================
//types:


//==============================================================================
//functions:

xResult DeviceReceiveData(xDeviceT* device, CAN_LocalSegmentT* segment);
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_DEVICE_COMMON_
