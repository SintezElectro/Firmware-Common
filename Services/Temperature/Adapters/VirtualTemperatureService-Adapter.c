//==============================================================================
//includes:

#include "Services/Common/ServiceCommon.h"
#include "Common/xMemory.h"
#include "Common/xCircleBuffer.h"
#include "VirtualTemperatureService-Adapter.h"
#include "Abstractions/xSystem/xSystem.h"
#include "CAN_Local-Types.h"
#include "Services/Common/VirtualServiceCommon.h"
#include "Components.h"
//==============================================================================
//defines:


//==============================================================================
//types:


//==============================================================================
//variables:

static const ServiceReceiverInterfaceT privateReceiverInterface;

static volatile uint32_t TemperatureServiceTotalTime;
//==============================================================================
//functions:

static xResult privateNotificationReceiver(xServiceT* service,
		xPortT* port,
		CAN_LocalSegmentT* segment,
		CAN_LocalBaseEventPacketT payload)
{
	ServiceSendToSubscribers(service, 0, 0, NULL);

	return xResultAccept;
}
//------------------------------------------------------------------------------
static xResult privateBroadcastRequestReceiver(xServiceT* service,
		xPortT* port,
		CAN_LocalSegmentT* segment,
		uint64_t payload)
{
	switch (segment->ExtensionHeader.PacketType)
	{
		case CAN_LocalBroadcastPacketTypeIdChanged:
		{
			CAN_LocalBroadcastContentIdChangedT content = { .Value = payload };
			service->Id = content.NewId;
			break;
		}
	}

	return xResultAccept;
}
//------------------------------------------------------------------------------
static xResult privateRequestListener(TemperatureServiceT* service, int selector, uint32_t mode, void* in, void* out)
{
	extern xResult VirtualServiceRequestListener(xServiceT* service, int selector, uint32_t mode, void* in, void* out);

	CAN_LocalRequestDataT requestData;
	uint8_t requestDataSize = 0;

	switch ((uint32_t)selector)
	{
		case TemperatureServiceRequestGetTemperature:
		{
			break;
		}

		default : return VirtualServiceRequestListener((void*)service, selector, mode, in, out);
	}

	VirtualServiceRequestGetParameter(NULL, (void*)service, selector, out, &requestData, requestDataSize);

	return xResultAccept;
}
//------------------------------------------------------------------------------
static void privateEventListener(TemperatureServiceT* service, int selector, uint32_t mode, void* in, void* out)
{
	switch (selector)
	{
		case xServiceAdapterEventRecieveData:
		{
			uint8_t isPersonal = ServicePacketReceiver((void*)service,
					NULL,
					&privateReceiverInterface,
					in) == xResultAccept;

			*(uint8_t*)out = isPersonal;
			break;
		}

		default:
			break;
	}
}
//------------------------------------------------------------------------------
static void privateHandler(TemperatureServiceT* service)
{

}
//==============================================================================
//initializations:

static TemperatureServiceAdapterInterfaceT privateInterface =
{
	.Handler = (xServiceAdapterHandlerT)privateHandler,
	.RequestListener = (xServiceAdapterRequestListenerT)privateRequestListener,
	.EventListener = (xServiceAdapterEventListenerT)privateEventListener,
};

//------------------------------------------------------------------------------
static const ServiceReceiverInterfaceT privateReceiverInterface =
{
	.NotificationReceiver = privateNotificationReceiver,
	.BroadcastRequestReceiver = privateBroadcastRequestReceiver
};

//==============================================================================
xResult VirtualTemperatureServiceAdapterInit(TemperatureServiceT* service,
		VirtualTemperatureServiceAdapterT* adapter,
		VirtualTemperatureServiceAdapterInitT* init)
{
	service->Base.Adapter.Content = adapter;
	service->Base.Adapter.Interface = &privateInterface;
	//service->Adapter.Description = nameof(VirtualTemperatureServiceAdapterT);

	service->Base.IsEnable = true;
	service->Base.IsAvailable = false;
	service->Base.DynamicallyAllocated = true;

	return xResultAccept;
  
  return xResultError;
}
//==============================================================================
