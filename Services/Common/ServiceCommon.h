//==============================================================================
//header:

#ifndef _SERVICE_COMMON_
#define _SERVICE_COMMON_
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "Abstractions/xDevice/xDevice.h"
#include "Abstractions/xPort/xPort.h"
#include "Abstractions/xSystem/xSystem.h"
#include "Abstractions/xTransferLayer/xTransferLayer.h"
#include "CAN_Local-Types.h"
//==============================================================================
//types:

typedef struct
{
	xServiceT* Service;

	CAN_LocalMessageTypes MessageType;
	uint8_t PacketType;

	void* Content;
	uint8_t ContentSize;

} CAN_LocalRequestAddEx1T;
//------------------------------------------------------------------------------

typedef xResult (*ServiceNotificationReceiverT)(xServiceT* service,
		xPortT* port,
		CAN_LocalSegmentT* segment,
		CAN_LocalBaseEventPacketT payload);

typedef xResult (*ServiceRequestReceiverT)(xServiceT* service,
		xPortT* port,
		CAN_LocalSegmentT* segment,
		CAN_LocalRequestContentT payload);

typedef xResult (*ServiceOpenTransferRequestReceiverT)(xServiceT* service,
		xPortT* port,
		CAN_LocalSegmentT* segment,
		CAN_LocalRequestContentOpenTransferT payload);

typedef xResult (*ServiceBroadcastRequestReceiverT)(xServiceT* service,
		xPortT* port,
		CAN_LocalSegmentT* segment,
		uint64_t payload);

typedef struct
{
	ServiceNotificationReceiverT NotificationReceiver;
	ServiceRequestReceiverT RequestReceiver;
	ServiceOpenTransferRequestReceiverT OpenTransferRequestReceiver;
	ServiceBroadcastRequestReceiverT BroadcastRequestReceiver;

} ServiceReceiverInterfaceT;
//==============================================================================
//functions:

xResult ServiceRequestListener(xServiceT* service, int selector, uint32_t description, void* in, void* out);
xResult ServiceRequestSetId(xServiceT* service, xIAsyncRequestT* asyncRequest, xServiceRequestSetIdT* request);
void ServiceSendToSubscribers(xServiceT* service, int selector, uint32_t description, void* in);
void ServiceCompleteTransfer(xTransferLayerT* layer, xTransferT* transfer);

xResult ServiceRequestGetParameter(xServiceT* sender,
		xServiceT* recipient,
		uint16_t action,
		xIAsyncRequestT* asyncRequest,
		void* data,
		uint8_t dataSize);

xResult ServiceRequestSetParameter(xServiceT* sender,
		xServiceT* recipient,
		uint16_t action,
		xIAsyncRequestT* asyncRequest,
		void* data,
		uint8_t dataSize);

void ServiceAccomplishAsyncRequest(xIAsyncRequestT* asyncRequest,
		xResult result,
		void* content,
		uint32_t contentSize);

void ServiceSendResponse(xServiceT* service,
		xPortT* port,
		CAN_LocalSegmentT* segment,
		void* content,
		uint8_t contentSize);

xResult ServicePacketReceiver(xServiceT* service,
		xPortT* port,
		const ServiceReceiverInterfaceT* interface,
		CAN_LocalSegmentT* segment);

xResult ServiceAcceptOpenTransfer(xServiceT* service,
		xPortT* port,
		CAN_LocalSegmentT* segment,
		void* data,
		uint16_t dataLength,
		xTransferEventListenerT eventListener);
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_SERVICE_COMMON_
