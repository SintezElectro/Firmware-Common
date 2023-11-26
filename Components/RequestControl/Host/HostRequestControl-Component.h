//==============================================================================
//header:

#if !defined(_HOST_REQUEST_CONTROL_COMPONENT_H_) && defined(HOST_REQUEST_CONTROL_COMPONENT_ENABLE)
#define _HOST_REQUEST_CONTROL_COMPONENT_H_
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "Components-Types.h"
#include "HostRequestControl-ComponentConfig.h"
#include "CAN_Local-Types.h"
#include "Abstractions/xRequestControl/xRequestControl.h"
//==============================================================================
//defines:


//==============================================================================
//types:


//==============================================================================
//variables:


//==============================================================================
//functions:

xResult HostRequestControlComponentInit(void* parent);

void HostRequestControlComponentHandler();

#define HostRequestControlComponentTimeSynchronization()
//==============================================================================
//import:


//==============================================================================
//override:


//==============================================================================
//export:

extern xRequestControlT HostRequestControl;
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_HOST_REQUEST_CONTROL_COMPONENT_H_
