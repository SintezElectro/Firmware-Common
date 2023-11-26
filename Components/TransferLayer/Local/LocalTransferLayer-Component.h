//==============================================================================
//header:

#ifndef _LOCAL_TRANSFER_LAYER_COMPONENT_H_
#define _LOCAL_TRANSFER_LAYER_COMPONENT_H_
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "Components-Types.h"
#include "LocalTransferLayer-ComponentConfig.h"
#include "CAN_Local-Types.h"
#include "Abstractions/xTransferLayer/xTransferLayer.h"
//==============================================================================
//defines:


//==============================================================================
//types:


//==============================================================================
//variables:


//==============================================================================
//functions:

xResult LocalTransferLayerComponentInit(void* parent);

#define LocalTransferLayerComponentHandler() xTransferLayerHandlerDirect(LocalTransferLayer)

#define LocalTransferLayerComponentTimeSynchronization()
//==============================================================================
//import:


//==============================================================================
//override:


//==============================================================================
//export:

extern xTransferLayerT LocalTransferLayer;
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_LOCAL_TRANSFER_LAYER_COMPONENT_H_
