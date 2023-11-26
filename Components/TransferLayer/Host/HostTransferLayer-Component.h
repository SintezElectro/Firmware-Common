//==============================================================================
//header:

#if !defined(_HOST_TRANSFER_LAYER_COMPONENT_H_) && defined(HOST_TRANSFER_LAYER_COMPONENT_ENABLE)
#define _HOST_TRANSFER_LAYER_COMPONENT_H_
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "Components-Types.h"
#include "CAN_Local-Types.h"
#include "HostTransferLayer-ComponentConfig.h"
#include "Abstractions/xTransferLayer/xTransferLayer.h"
//==============================================================================
//defines:


//==============================================================================
//types:


//==============================================================================
//functions:

xResult HostTransferLayerComponentInit(void* parent);

void HostTransferLayerComponentHandler();

#define HostTransferLayerComponentTimeSynchronization()
//==============================================================================
//import:


//==============================================================================
//override:


//==============================================================================
//export:

extern xTransferLayerT HostTransferLayer;
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_HOST_TRANSFER_LAYER_COMPONENT_H_
