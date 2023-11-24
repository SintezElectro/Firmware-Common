//==============================================================================
//header:

#ifndef _CLIENT_DEVICE_ADAPTER_H_
#define _CLIENT_DEVICE_ADAPTER_H_
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "Abstractions/xDevice/xDevice.h"
#include "Abstractions/xPort/xPort.h"
#include "Abstractions/xTransferLayer/xTransferLayer-Types.h"
#include "Common/xCircleBuffer.h"
//==============================================================================
//types:

typedef struct
{
	uint32_t OperationTimeStamp;
	uint16_t OperationTimeOut;
	uint16_t Operation;

	uint16_t RxPacketHandlerIndex;
	xCircleBufferT* PortRxCircleBuffer;

} ClientDeviceAdapterContentT;
//------------------------------------------------------------------------------
typedef struct
{
	ClientDeviceAdapterContentT Content;

	xPortT* Port;
	xTransferLayerT* TransferLayer;

} ClientDeviceAdapterT;
//------------------------------------------------------------------------------
typedef struct
{
	xPortT* Port;
	xTransferLayerT* TransferLayer;

} ClientDeviceAdapterInitT;
//==============================================================================
//functions:

xResult ClientDeviceAdapterInit(xDeviceT* device, ClientDeviceAdapterT* adapter, ClientDeviceAdapterInitT* init);
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_CLIENT_DEVICE_ADAPTER_H_
