//==============================================================================
//includes:

#include "ServiceCommon.h"
#include "Components/RequestControl/Host/HostRequestControl-Component.h"
#include "Components/RequestControl/Local/RequestControl-Component.h"
#include "Components/Devices/Host/HostDevice-Component.h"
#include "Common/xMemory.h"
//==============================================================================
//defines:


//==============================================================================
//types:


//==============================================================================
//variables:

static xTransferT* privateSlaveTransfer;
//==============================================================================
//functions:

//------------------------------------------------------------------------------
void ServiceCompleteTransfer(xTransferLayerT* layer, xTransferT* transfer)
{
	transfer->State = xTransferStateIdle;

	xMemoryFree(transfer->Data);
}
//------------------------------------------------------------------------------
static void privateTransferEventListener(xTransferLayerT* layer,
		xTransferT* transfer,
		int selector,
		uint32_t description,
		void* arg)
{
	transfer->State = xTransferStateIdle;

	xMemoryFree(transfer->Data);
}
//------------------------------------------------------------------------------
xResult ServiceAcceptOpenTransfer(xServiceT* service,
		xPortT* port,
		CAN_LocalSegmentT* segment,
		void* data,
		uint16_t dataLength,
		xTransferEventListenerT eventListener)
{
	CAN_LocalRequestContentOpenTransferT content = { .Value = segment->Data.Value };

	xTransferLayerT* transferLayer = NULL;
	xServiceRequestListener((xServiceT*)service, xServiceRequestGetTransferLayer, 0, NULL, &transferLayer);

	if (transferLayer)
	{
		CAN_LocalResponseContentOpenTransferT response;
		response.ServiceId = segment->TransferHeader.ServiceId;
		response.Action = content.Action;
		response.Result = xResultError;
		response.Token = -1;

		privateSlaveTransfer = xTransferLayerNewTransfer(transferLayer);

		if (privateSlaveTransfer)
		{
			response.Result = xResultAccept;
			response.Token = xSystemGetRandom();
			response.ContantSize = dataLength;

			privateSlaveTransfer->Holder = service;
			privateSlaveTransfer->Id = segment->TransferHeader.ServiceId;
			privateSlaveTransfer->Type = content.Type == xTransferTypeReceive ? xTransferTypeTransmite : xTransferTypeReceive;
			privateSlaveTransfer->ValidationIsEnabled = content.ValidationIsEnabled;
			privateSlaveTransfer->MasterModeIsEnabled = false;
			privateSlaveTransfer->Data = data;

			if (privateSlaveTransfer->Type == xTransferTypeReceive && data == NULL)
			{
				privateSlaveTransfer->Data = xMemoryAllocate(1, dataLength);

				if (privateSlaveTransfer->Data == NULL)
				{
					return xResultMemoryAllocationError;
				}
			}

			privateSlaveTransfer->DataLength = dataLength;
			privateSlaveTransfer->Token = response.Token;
			privateSlaveTransfer->TimeOut = 1000;
			privateSlaveTransfer->TransmittingAttempts = 1;

			privateSlaveTransfer->EventListener = eventListener == NULL ? privateTransferEventListener : eventListener;

			xTransferLayerAdd(transferLayer,
					privateSlaveTransfer,
					0,
					(uint32_t)response.Action);
		}

		CAN_LocalSegmentT packet;
		packet.TransferHeader.MessageType = CAN_LocalMessageTypeTransfer;
		packet.TransferHeader.PacketType = CAN_LocalTransferPacketTypeApproveTransfer;
		packet.TransferHeader.Characteristic = service->Info.Type;
		packet.TransferHeader.ServiceId = service->Id;
		packet.TransferHeader.IsEnabled = true;

		packet.Data.Value = response.Value;
		packet.DataLength = sizeof(response);

		xPortExtendedTransmition(port, &packet);
	}

	return xResultError;
}
//------------------------------------------------------------------------------
void ServiceSendToSubscribers(xServiceT* service, int selector, uint32_t description, void* in)
{
	xServiceSubscriberListElementT* element = service->Subscribers.Head;

	while (element)
	{
		xServiceSubscriberT* subscriber = element->Value;

		if (subscriber->EventListener)
		{
			subscriber->EventListener((void*)service, selector, description, (void*[]){ subscriber, in });
		}

		element = element->Next;
	}
}
//------------------------------------------------------------------------------
xResult ServicePacketReceiver(xServiceT* service,
		xPortT* port,
		const ServiceReceiverInterfaceT* interface,
		CAN_LocalSegmentT* segment)
{
	if (segment->ExtensionIsEnabled)
	{
		switch((uint8_t)segment->MessageType)
		{
			case CAN_LocalMessageTypeTransfer:
			{
				switch((uint8_t)segment->TransferHeader.PacketType)
				{
					case CAN_LocalTransferPacketTypeOpenTransfer:
					{
						CAN_LocalRequestContentOpenTransferT content = { .Value = segment->Data.Value };

						if (content.ServiceId == service->Id
						&& interface->OpenTransferRequestReceiver)
						{
							return interface->OpenTransferRequestReceiver(service, port, segment, content);
						}

						break;
					}
				}
				break;
			}

			case CAN_LocalMessageTypeRequest:
			{
				CAN_LocalRequestContentT content = { .Value = segment->Data.Value };

				if (service->Id == content.Description.Recipient
				&& interface->RequestReceiver)
				{
					return interface->RequestReceiver(service, port, segment, content);
				}
			}

			case CAN_LocalMessageTypeBroadcast:
			{
				if (segment->ExtensionHeader.ServiceId == service->Id
				&& interface->BroadcastRequestReceiver)
				{
					return interface->BroadcastRequestReceiver(service, port, segment, segment->Data.Value);
				}
			}
		}

		return xResultNotIdentified;
	}

	switch((uint8_t)segment->Header.MessageType)
	{
		case CAN_LocalMessageTypeNotification:
		{
			CAN_LocalBaseEventPacketT content = { .Value = segment->Data.Value };

			if (segment->Header.ServiceType == service->Info.Type
			&& content.Id == service->Id
			&& interface->NotificationReceiver)
			{
				return interface->NotificationReceiver(service, port, segment, content);
			}

			break;
		}
	}

	return xResultNotIdentified;
}
//------------------------------------------------------------------------------
void ServiceSendResponse(xServiceT* service,
		xPortT* port,
		CAN_LocalSegmentT* segment,
		void* content,
		uint8_t contentSize)
{
	CAN_LocalRequestContentT request = { .Value = segment->Data.Value };

	CAN_LocalResponseContentT responseContent;
	responseContent.Description.Sender = segment->ExtensionHeader.ServiceId;
	responseContent.Description.Action = request.Description.Action;
	responseContent.Description.Sequence = request.Description.Sequence;

	CAN_LocalSegmentT responseSegment;
	responseSegment.ExtensionHeader.IsEnabled = true;
	responseSegment.ExtensionHeader.MessageType = CAN_LocalMessageTypeResponse;
	responseSegment.ExtensionHeader.PacketType = CAN_LocalRequestPacketTypeCommon;
	responseSegment.ExtensionHeader.ServiceId = service->Id;
	responseSegment.ExtensionHeader.ServiceType = service->Info.Type;

	memcpy(responseContent.Data.Bytes, content, contentSize);

	responseSegment.Data.Value = responseContent.Value;
	responseSegment.DataLength = sizeof(responseContent.Description) + contentSize;

	xPortExtendedTransmition(port, &responseSegment);
}
//------------------------------------------------------------------------------
void ServiceAccomplishAsyncRequest(xIAsyncRequestT* asyncRequest,
		xResult result,
		void* content,
		uint32_t contentSize)
{
	xIAsyncResultT asyncResult = { 0 };
	asyncResult.Holder = (void*)asyncRequest->Holder;
	asyncResult.OperationResult = xResultAccept;
	asyncResult.Result = content;
	asyncResult.ResultSize = contentSize;
	asyncResult.Content = asyncRequest->Content;

	if(asyncRequest->Callback)
	{
		asyncRequest->Callback(&asyncResult);
	}
}
//------------------------------------------------------------------------------
xResult ServiceRequestListener(xServiceT* service, int selector, uint32_t description, void* in, void* out)
{
	switch (selector)
	{
		case xServiceRequestSetId:
		{
			xServiceRequestSetIdT* request = in;

			CAN_LocalBroadcastContentIdChangedT broadcastContent;
			broadcastContent.NewId = request->NewId;
			broadcastContent.TimeStamp = xSystemGetTime(NULL);

			CAN_LocalRequestAddEx1T webRequest =
			{
				.Service = service,
				.MessageType = CAN_LocalMessageTypeBroadcast,
				.PacketType = CAN_LocalBroadcastPacketTypeIdChanged,
				.Content = &broadcastContent,
				.ContentSize = sizeof(CAN_LocalBroadcastContentIdChangedT)
			};

			RequestControl.Adapter.Interface->RequestListener(&RequestControl,
					xRequestControlAdapterRequestAdd, 1, &webRequest);

			service->Id = request->NewId;

			break;
		}
		case xServiceRequestGetTransferLayer:
		{
			xDeviceT* holder = xServiceGetDevice(service);

			xDeviceRequestListener(holder, xDeviceRequestGetTransferLayer, 0, NULL, out);
			break;
		}

		default : return xResultNotSupported;
	}

	return xResultAccept;
}
//==============================================================================
