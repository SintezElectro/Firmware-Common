//==============================================================================
//header:

#ifndef _REQUEST_CONTROL_COMPONENT_H_
#define _REQUEST_CONTROL_COMPONENT_H_
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "RequestControl-ComponentConfig.h"
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

xResult RequestControlComponentInit(void* parent);

void RequestControlComponentHandler();

#define RequestControlComponentTimeSynchronization()
//==============================================================================
//import:


//==============================================================================
//override:


//==============================================================================
//export:

extern xRequestControlT RequestControl;
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_REQUEST_CONTROL_COMPONENT_H_
