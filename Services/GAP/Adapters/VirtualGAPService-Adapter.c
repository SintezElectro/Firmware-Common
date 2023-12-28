//==============================================================================
//includes:

#include "Services/Common/ServiceCommon.h"
#include "Common/xCircleBuffer.h"
#include "VirtualGAPService-Adapter.h"
#include "Abstractions/xSystem/xSystem.h"
#include "CAN_Local-Types.h"
#include "Components.h"
//==============================================================================
//defines:


//==============================================================================
//types:


//==============================================================================
//variables:

static const ServiceReceiverInterfaceT privateReceiverInterface;
//==============================================================================
//functions:

static xResult privateNotificationReceiver(xServiceT* service,
		xPortT* port,
		CAN_LocalSegmentT* segment,
		CAN_LocalBaseEventPacketT content)
{
	ServiceSendToSubscribers(service, 0, 0, NULL);

	return xResultAccept;
}
//------------------------------------------------------------------------------
static void privateHandler(GAPServiceT* service)
{
	
}
//------------------------------------------------------------------------------
static xResult privateRequestListener(GAPServiceT* service, int selector, uint32_t mode, void* in, void* out)
{
	switch ((uint32_t)selector)
	{
		case xServiceRequestSetId:
		{

			break;
		}

		default : return xResultNotIdentified;
	}

	return xResultAccept;
}
//------------------------------------------------------------------------------
static void privateEventListener(GAPServiceT* service, int selector, uint32_t mode, void* in, void* out)
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
//==============================================================================
//initializations:

static GAPServiceAdapterInterfaceT privateInterface =
{
	.Handler = (xServiceAdapterHandlerT)privateHandler,
	.RequestListener = (xServiceAdapterRequestListenerT)privateRequestListener,
	.EventListener = (xServiceAdapterEventListenerT)privateEventListener,
};
//------------------------------------------------------------------------------
static const ServiceReceiverInterfaceT privateReceiverInterface =
{
	.NotificationReceiver = privateNotificationReceiver,
};
//==============================================================================
xResult VirtualGAPServiceAdapterInit(GAPServiceT* service,
		VirtualGAPServiceAdapterT* adapter,
		VirtualGAPServiceAdapterInitT* init)
{
	if (service && init)
	{
		service->Base.Adapter.Content = adapter;
		service->Base.Adapter.Interface = (void*)&privateInterface;
		//service->Adapter.Description = nameof(VirtualGAPServiceAdapterT);

		return xResultAccept;
	}
  
  return xResultError;
}
//==============================================================================
