//==============================================================================
//includes:

#include <stdlib.h>
#include "Common/xMemory.h"
#include "Common/xCircleBuffer.h"
#include "VirtualTemperatureService-Adapter.h"
#include "Abstractions/xSystem/xSystem.h"
#include "CAN_Local-Types.h"
#include "Services/Common/VirtualServiceCommonRequests.h"
#include "Components.h"
//==============================================================================
//defines:


//==============================================================================
//types:


//==============================================================================
//variables:

static volatile uint32_t TemperatureServiceTotalTime;
//==============================================================================
//functions:

static void privateNotificationHandler(TemperatureServiceT* service,
		VirtualTemperatureServiceAdapterT* adapter,
		CAN_LocalSegmentT* segment)
{
	CAN_LocalBaseEventPacketT content = { .Value = segment->Data.Value };

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
static void privateExtensionNotificationHandler(TemperatureServiceT* service,
		VirtualTemperatureServiceAdapterT* adapter,
		CAN_LocalSegmentT* segment)
{
	if (segment->ExtensionHeader.ServiceType == service->Base.Info.Type
		&& segment->ExtensionHeader.ServiceId == service->Base.Id)
	{
		switch (segment->ExtensionHeader.PacketType)
		{
			case CAN_LocalTemperatureNotificationUpdateTemperature:
			{
				CAN_LocalTemperatureNotificationUpdateTemperatureT content;
				memcpy(content.Data, segment->Data.Bytes, sizeof(content));
				service->Temperature = (float)content.Temperature / 1000;
				break;
			}
		}

		xServiceSubscriberListElementT* element = service->Base.Subscribers.Head;

		while (element)
		{
			xServiceSubscriberT* subscriber = element->Value;

			if (subscriber->EventListener)
			{
				subscriber->EventListener((void*)service, subscriber, 0, &segment->Data.Bytes);
			}

			element = element->Next;
		}
	}
}
//------------------------------------------------------------------------------
static void privateExtensionBroadcastHandler(TemperatureServiceT* service,
		VirtualTemperatureServiceAdapterT* adapter,
		CAN_LocalSegmentT* segment)
{

	if (segment->ExtensionHeader.ServiceId == service->Base.Id)
	{
		switch (segment->ExtensionHeader.PacketType)
		{
			case CAN_LocalBroadcastPacketTypeIdChanged:
			{
				volatile CAN_LocalBroadcastContentIdChangedT content = { .Value = segment->Data.Value };
				service->Base.Id = content.NewId;
				break;
			}
		}
	}
}
//------------------------------------------------------------------------------
static void privateHandler(TemperatureServiceT* service)
{
	VirtualTemperatureServiceAdapterT* adapter = service->Base.Adapter.Content;
	xPortT* port = xServiceGetPort((void*)service);

	xCircleBufferT* circleBuffer = xPortGetRxCircleBuffer(port);
	TemperatureServiceTotalTime = xSystemGetTime(NULL);

	while (adapter->Internal.RxPacketHandlerIndex != circleBuffer->TotalIndex)
	{
		CAN_LocalSegmentT* segment = xCircleBufferGetElement(circleBuffer, adapter->Internal.RxPacketHandlerIndex);

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
				case CAN_LocalMessageTypeNotification:
				{
					privateExtensionNotificationHandler(service, adapter, segment);
					break;
				}
				case CAN_LocalMessageTypeBroadcast:
				{
					privateExtensionBroadcastHandler(service, adapter, segment);
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

		adapter->Internal.RxPacketHandlerIndex++;
		adapter->Internal.RxPacketHandlerIndex &= circleBuffer->SizeMask;
	}
}
//------------------------------------------------------------------------------
static xResult privateRequestListener(TemperatureServiceT* service, int selector, void* arg[])
{
	extern xResult VirtualServiceRequestListener(xServiceT* service, int selector, void* arg, void* adapterRequest);

	CAN_LocalRequestDataT requestData;
	uint8_t requestDataSize = 0;

	switch ((uint32_t)selector)
	{
		case TemperatureServiceRequestGetTemperature:
		{
			break;
		}

		default : return VirtualServiceRequestListener((void*)service, selector, arg[0], arg[1]);
	}

	VirtualServiceRequestGetParameter(NULL, (void*)service, selector, arg[0], &requestData, requestDataSize);

	return xResultAccept;
}
//==============================================================================
//initializations:

static TemperatureServiceAdapterInterfaceT privateInterface =
{
	.Handler = (TemperatureServiceAdapterHandlerT)privateHandler,
	.RequestListener = (TemperatureServiceAdapterRequestListenerT)privateRequestListener
};
//------------------------------------------------------------------------------
xResult VirtualTemperatureServiceAdapterInit(TemperatureServiceT* service,
		VirtualTemperatureServiceAdapterT* adapter,
		VirtualTemperatureServiceAdapterInitT* init)
{
	service->Base.Adapter.Content = adapter;
	service->Adapter.Interface = &privateInterface;
	//service->Adapter.Description = nameof(VirtualTemperatureServiceAdapterT);

	service->Base.IsEnable = true;
	service->Base.IsAvailable = false;
	service->Base.DynamicallyAllocated = true;

	return xResultAccept;
  
  return xResultError;
}
//==============================================================================