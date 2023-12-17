//==============================================================================
//includes:

#include <stdlib.h>
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


//==============================================================================
//functions:

static void privateNotificationHandler(GAPServiceT* service,
		VirtualGAPServiceAdapterT* adapter,
		CAN_LocalSegmentT* segment)
{
	CAN_LocalBaseEventPacketT content = { .Value = segment->Data.DoubleWord };

	if (segment->Header.ServiceType == service->Base.Info.Type && content.Id == service->Base.Id)
	{
		xServiceSubscriberListElementT* element = service->Base.Subscribers.Head;

		while (element)
		{
			xServiceSubscriberT* subscriber = element->Value;

			if (subscriber->EventListener)
			{
				subscriber->EventListener((void*)service, subscriber, 0, &content.Content);
			}

			element = element->Next;
		}
	}
}
//------------------------------------------------------------------------------
static void privateReceiver(GAPServiceT* service, VirtualGAPServiceAdapterT* adapter, CAN_LocalSegmentT* segment)
{
	if (segment->ExtensionIsEnabled)
	{
		switch((uint8_t)segment->ExtensionHeader.MessageType)
		{
			case CAN_LocalMessageTypeTransfer:
			{
				switch((uint8_t)segment->ExtensionHeader.PacketType)
				{
					/*case CAN_LocalTransferPacketTypeOpenTransfer:
						privateOpenTransferHandler(service, adapter, segment);
						break;*/
				}
				break;
			}
		}
	}
	else
	{
		switch((uint8_t)segment->Header.MessageType)
		{
			case CAN_LocalMessageTypeNotification:
			{
				privateNotificationHandler(service, adapter, segment);
				break;
			}
		}
	}
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
			/*xServiceRequestSetIdT* parameters = GetParameter(arg, 1);

			CAN_LocalRequestContentServiceSetIdT requestContent;
			requestContent.ServiceId = parameters->ServiceId;
			requestContent.NewServiceId = parameters->NewId;

			CAN_LocalRequestT* request = xRequestNew(&HostRequestControl);
			request->Base.Sender = (void*)&HostGAP;
			request->Action = xServiceRequestSetId;
			request->Recipient = (void*)service;

			request->Data.Value = requestContent.Value;
			request->Base.TxDataSize = sizeof(CAN_LocalRequestContentServiceSetIdT);

			xRequestControlAdd(&HostRequestControl, (void*)request);
			VirtualServiceRequestSetId();*/
			break;
		}

		default : return xResultRequestIsNotFound;
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
			privateReceiver(service, service->Base.Adapter.Content, in);
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
