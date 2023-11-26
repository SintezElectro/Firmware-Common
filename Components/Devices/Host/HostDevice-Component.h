//==============================================================================
//header:

#if !defined(_HOST_DEVICE_COMPONENT_H_) && defined(HOST_DEVICE_COMPONENT_ENABLE)
#define _HOST_DEVICE_COMPONENT_H_
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "Components-Types.h"
#include "HostDevice-ComponentConfig.h"
#include "Abstractions/xDevice/xDevice.h"
#include "Services/GAP/GAPService.h"
//==============================================================================
//defines:


//==============================================================================
//functions:

xResult HostDeviceComponentInit(void* parent);
void HostDeviceComponentHandler();
void HostDeviceComponentTimeSynchronization();
//==============================================================================
//import:


//==============================================================================
//override:


//==============================================================================
//export:

extern xDeviceT HostDevice;
extern GAPServiceT HostGAP;
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_HOST_DEVICE_COMPONENT_H_
