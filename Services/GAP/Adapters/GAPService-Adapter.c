//==============================================================================
//includes:

#include <stdlib.h>
#include "Common/xCircleBuffer.h"
#include "GAPService-Adapter.h"
#include "Abstractions/xSystem/xSystem.h"
#include "CAN_Local-Types.h"
#include "Components/TransferLayer/Host/HostTransferLayer-Component.h"
//==============================================================================
//defines:


//==============================================================================
//types:


//==============================================================================
//variables:


//==============================================================================
//functions:

static void privateTransferEventListener(xTransferLayerT* layer, xTransferT* transfer, int selector, void* arg)
{
	transfer->State = xTransferStateIdle;


}
//------------------------------------------------------------------------------
static void privateOpenTransferHandler(GAPServiceT* service,
		GAPServiceAdapterT* adapter,
		volatile CAN_LocalSegmentT* segment)
{
	CAN_LocalRequestContentOpenTransferT request = { .Value = segment->Data.DoubleWord };

	xTransferLayerT* transferLayer = NULL;
	xServiceRequestListener((xServiceT*)service, xServiceRequestGetTransferLayer, 0, NULL, &transferLayer);

	if (request.ServiceId == service->Base.Id && transferLayer)
	{
		CAN_LocalResponseContentOpenTransferT response;
		response.ServiceId = segment->ExtensionHeader.ServiceId;
		response.Action = request.Action;
		response.Token = -1;
		response.Result = xResultError;

		xTransferT* transfer = xTransferLayerNewTransfer(transferLayer);
		CAN_LocalTransferT* extansion = (void*)transfer;

		if (transfer == NULL)
		{
			goto error;
		}

		transfer->Holder = service;
		transfer->Id = segment->TransferHeader.ServiceId;
		transfer->EventListener = privateTransferEventListener;
		transfer->Type = request.Type == xTransferTypeReceive ? xTransferTypeTransmite : xTransferTypeReceive;
		transfer->ValidationIsEnabled = request.ValidationIsEnabled;
		transfer->MasterModeIsEnabled = false;
		extansion->Action = request.Action;

		response.Token = random();
		response.Result = xResultAccept;

		error:;
		transfer->State = xTransferStateIdle;
		transfer = NULL;

		CAN_LocalSegmentT packet;
		packet.ExtensionHeader.MessageType = CAN_LocalMessageTypeTransfer;
		packet.ExtensionHeader.PacketType = CAN_LocalTransferPacketTypeApproveTransfer;
		packet.ExtensionHeader.ServiceType = service->Base.Info.Type;
		packet.ExtensionHeader.ServiceId = service->Base.Id;
		packet.ExtensionIsEnabled = true;

		packet.Data.Value = response.Value;
		packet.DataLength = sizeof(response);

		if (transfer)
		{
			xTransferLayerAdd(transferLayer, transfer);
		}

		xPortExtendedTransmition(adapter->Port, &packet);
	}
}
//------------------------------------------------------------------------------
static void privateRequestHandler(GAPServiceT* service,
		GAPServiceAdapterT* adapter,
		CAN_LocalSegmentT* segment)
{
	CAN_LocalRequestContentT content = { .Value = segment->Data.Value };
	xDeviceT* device = xServiceGetDevice(service);

	if (content.Description.Recipient == service->Base.Id)
	{
		CAN_LocalResponseContentT response;
		response.Description.Sequence = content.Description.Sequence;
		response.Description.Sender = segment->ExtensionHeader.ServiceId;
		response.Description.Action = content.Description.Action;

		CAN_LocalSegmentT segment = { 0 };
		segment.ExtensionHeader.IsEnabled = true;
		segment.ExtensionHeader.MessageType = CAN_LocalMessageTypeResponse;
		segment.ExtensionHeader.PacketType = CAN_LocalRequestPacketTypeCommon;
		segment.ExtensionHeader.ServiceId = service->Base.Id;
		segment.ExtensionHeader.ServiceType = service->Base.Info.Type;

		uint8_t size = 0;

		switch (content.Description.Action)
		{
			case GAPServiceRequestGetNumberOfServices:
			{
				response.Data.Bytes[0] = device->Services.Count;
				//segment.DataLength = sizeof(CAN_LocalResponseContentT) - sizeof(response.Data) + 1;
				size = sizeof(response.Data.Bytes[0]);

				break;
			}

			case GAPServiceRequestGetService:
			{
				CAN_LocalRequestGATGetServiceT requestContent = { .Value = content.Data.Content };
				xServiceT* service = xListGetObjectByIndex((void*)&device->Services, requestContent.Id);

				CAN_LocalResponseGATGetServiceT responseContent;
				responseContent.Id = service->Id;
				responseContent.Type = service->Info.Type;
				responseContent.Extension = service->Info.Extension;

				response.Data.Content = responseContent.Value;
				//segment.DataLength = sizeof(CAN_LocalResponseContentT) - sizeof(response.Data) + sizeof(CAN_LocalResponseGATGetServiceT);
				size = sizeof(CAN_LocalResponseGATGetServiceT);

				break;
			}

			case xServiceRequestSetId:
			{
				CAN_LocalRequestContentServiceSetIdT requestContent = { .Value = content.Data.Content };
				xServiceT* targetService = xDeviceGetServiceById(service->Base.Base.Parent, requestContent.ServiceId);

				if (targetService != NULL)
				{
					xServiceRequestSetIdT parameter;
					parameter.NewId = requestContent.NewServiceId;
					xServiceRequestListener((xServiceT*)targetService, xServiceRequestSetId, 0, &parameter, NULL);
				}
				break;
			}

			default: return;
		}

		segment.Data.Content = response.Value;
		segment.DataLength = sizeof(response.Description) + size;

		xPortExtendedTransmition(adapter->Port, &segment);
	}
}
//------------------------------------------------------------------------------
static void privateNotificationHandler(GAPServiceT* service,
		GAPServiceAdapterT* adapter,
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
static void privateReceiver(GAPServiceT* service, GAPServiceAdapterT* adapter, CAN_LocalSegmentT* segment)
{
	if (segment->ExtensionIsEnabled)
	{
		if (segment->ExtensionHeader.MessageType == CAN_LocalMessageTypeTransfer)
		{
			switch((uint8_t)segment->ExtensionHeader.PacketType)
			{
				case CAN_LocalTransferPacketTypeOpenTransfer:
					privateOpenTransferHandler(service, adapter, segment);
					break;
			}
		}
		else if (segment->ExtensionHeader.MessageType == CAN_LocalMessageTypeRequest)
		{
			privateRequestHandler(service, adapter, segment);
		}

		/*switch((uint8_t)segment->ExtensionHeader.MessageType)
		{
			case CAN_LocalMessageTypeTransfer:
			{
				switch((uint8_t)segment->ExtensionHeader.PacketType)
				{
					case CAN_LocalTransferPacketTypeOpenTransfer:
						privateOpenTransferHandler(service, adapter, segment);
						break;
				}
				break;
			}

			case 3:
			{
				privateRequestHandler(service, adapter, segment);
				break;
			}
		}*/
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
xResult GAPServiceAdapterInit(GAPServiceT* service,
		GAPServiceAdapterT* adapter,
		GAPServiceAdapterInitT* init)
{
	if (service && init)
	{
		service->Base.Adapter.Content = adapter;
		service->Base.Adapter.Interface = (void*)&privateInterface;
		//service->Adapter.Description = nameof(GAPServiceAdapterT);

		adapter->Port = init->Port;

		return xResultAccept;
	}
  
  return xResultError;
}
//==============================================================================
