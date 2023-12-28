//==============================================================================
//includes:

#include "CAN_LocalRequestControl-Adapter.h"
#include "Abstractions/xSystem/xSystem.h"
#include "Abstractions/xDevice/xService-Types.h"
#include "Common/xCircleBuffer.h"
#include "RequestControl-ComponentConfig.h"
#include "Services/Common/ServiceCommon.h"
//==============================================================================
//defines:

#define REQUEST_DEFAULT_ATTEMPTS_COUNT 1
#define REQUEST_DEFAULT_TIMEOUT 300
//==============================================================================
//types:


//==============================================================================
//variables:


//==============================================================================
//prototypes:

static xResult privateRequestListener(xRequestControlT* control,
		xRequestControlAdapterRequestSelector selector,
		uint32_t description,
		void* arg);

//static xResult privateRequestPrepare(xRequestControlT* control, CAN_LocalRequestControlAdapterT* adapter, void* arg[]);
//==============================================================================
//functions:

static void privatePacketHandler(xRequestControlT* control, CAN_LocalRequestControlAdapterT* adapter)
{
	xCircleBufferT* circleBuffer = xPortGetRxCircleBuffer(adapter->Port);

	while (adapter->Content.RxPacketHandlerIndex != circleBuffer->TotalIndex)
	{
		if (!control->ProcessedRequests.Count)
		{
			adapter->Content.RxPacketHandlerIndex = circleBuffer->TotalIndex;
			break;
		}

		CAN_LocalSegmentT* segment = xCircleBufferGetElement(circleBuffer, adapter->Content.RxPacketHandlerIndex);

		if (segment->ExtensionHeader.IsEnabled
		&& segment->ExtensionHeader.MessageType == CAN_LocalMessageTypeResponse)
		{
			xRequestListElementT* element = control->ProcessedRequests.Head;

			while (element)
			{
				CAN_LocalRequestT* request = (void*)element->Value;
				xServiceT* sender = request->Base.Sender;

				if (request->Recipient->Id == segment->ExtensionHeader.ServiceId)
				{
					CAN_LocalResponseContentT content = { .Value = segment->Data.Value };

					if (content.Description.Sender == sender->Id)
					{
						request->Base.RxData = content.Data.Bytes;
						request->Base.RxDataSize = segment->DataLength - sizeof(CAN_LocalResponseDescriptionT);

						request->Base.Complited = true;
						request->Base.State = xRequestStateIdle;

						if (request->Base.EventListener)
						{
							request->TransmitionTime = xSystemGetTime(NULL) - request->StartTime;
							request->Base.EventListener(control, xRequestEventComlite, 0, (void*[]){ request, content.Data.Bytes });
							request->Base.EventListener = NULL;
						}

						request->Base.IsRunning = false;

						xListRemove((void*)&control->ProcessedRequests, request);
						break;
					}
				}

				element = element->Next;
			}
		}

		adapter->Content.RxPacketHandlerIndex++;
		adapter->Content.RxPacketHandlerIndex &= circleBuffer->SizeMask;
	}
}
//------------------------------------------------------------------------------
static void privateHandler(xRequestControlT* control)
{
	CAN_LocalRequestControlAdapterT* adapter = (CAN_LocalRequestControlAdapterT*)control->Adapter.Content;

	privatePacketHandler(control, adapter);

	xRequestListElementT* element = control->ProcessedRequests.Head;
	uint32_t time = xSystemGetTime(NULL);

	while (element)
	{
		void* deletedElement = NULL;
		CAN_LocalRequestT* request = (void*)element->Value;

		switch ((uint8_t)request->Base.State)
		{
			case xRequestStateInProgress:
			{
				if (time - request->Base.TimeStamp < request->Base.TimeOut)
				{
					break;
				}

				if (request->Base.AttemptNumber < request->Base.AttemptsCount)
				{
					request->Base.AttemptNumber++;
					request->Base.TimeStamp = xSystemGetTime(NULL);
					break;
				}

				request->Base.Result = xRequestResultTimeOut;
				request->Base.State = xRequestStateIdle;

				if (request->Base.EventListener)
				{
					request->TransmitionTime = xSystemGetTime(NULL) - request->StartTime;
					request->Base.EventListener(control, xRequestEventError, 0, (void*[]){ request, NULL });
					request->Base.EventListener = NULL;
				}

				request->Base.IsRunning = false;
				deletedElement = request;

				break;
			}

			case xRequestStatePreparing:
				break;

			default:
				deletedElement = request;
				break;
		}

		element = element->Next;

		if (deletedElement)
		{
			xListRemove((void*)&control->ProcessedRequests, deletedElement);
		}
	}

	//privateTranferHandler(control, adapter);
}
//------------------------------------------------------------------------------
static xResult privateRequestAdd(xRequestControlT* control, CAN_LocalRequestControlAdapterT* adapter, xRequestT* request)
{
	CAN_LocalRequestT* extansion = (void*)request;
	xServiceT* sender = request->Sender;

	CAN_LocalRequestContentT content = { 0 };
	content.Description.Recipient = extansion->Recipient->Id;
	content.Description.Action = extansion->Action;
	memcpy(&content.Data.Content, request->TxData, request->TxDataSize);

	extansion->Segment.ExtensionHeader.IsEnabled = true;
	extansion->Segment.ExtensionHeader.MessageType = CAN_LocalMessageTypeRequest;
	extansion->Segment.ExtensionHeader.PacketType = CAN_LocalRequestPacketTypeCommon;
	extansion->Segment.ExtensionHeader.ServiceId = sender->Id;
	extansion->Segment.ExtensionHeader.ServiceType = sender->Info.Type;

	extansion->Segment.Data.Content = content.Value;
	extansion->Segment.DataLength = sizeof(content) - sizeof(content.Data) + request->TxDataSize;

	request->RxData = 0;
	request->RxDataSize = 0;

	extansion->StartTime = xSystemGetTime(NULL);

	xListAdd((void*)&control->ProcessedRequests, request);
	xPortExtendedTransmition(adapter->Port, &extansion->Segment);

	return xResultAccept;
}
//------------------------------------------------------------------------------
static xResult privateRequestAddEx1(xRequestControlT* control,
		CAN_LocalRequestControlAdapterT* adapter,
		CAN_LocalRequestAddEx1T* request)
{
	xServiceT* sender = request->Service;

	CAN_LocalRequestT* newRequest = NULL;
	privateRequestListener(control, xRequestControlAdapterRequestNew, 0, &newRequest);

	if (newRequest)
	{
		newRequest->Base.Sender = sender;
		newRequest->Segment.ExtensionHeader.IsEnabled = true;
		newRequest->Segment.ExtensionHeader.MessageType = request->MessageType;
		newRequest->Segment.ExtensionHeader.PacketType = request->PacketType;
		newRequest->Segment.ExtensionHeader.ServiceId = sender->Id;
		newRequest->Segment.ExtensionHeader.ServiceType = sender->Info.Type;
		newRequest->Segment.DataLength = request->ContentSize;

		memcpy(newRequest->Segment.Data.Bytes, request->Content, request->ContentSize);

		xListAdd((void*)&control->ProcessedRequests, newRequest);
		xPortExtendedTransmition(adapter->Port, &newRequest->Segment);
	}

	return xResultAccept;
}
//------------------------------------------------------------------------------
static xResult privateRequestListener(xRequestControlT* control,
		xRequestControlAdapterRequestSelector selector,
		uint32_t description,
		void* arg)
{
	CAN_LocalRequestControlAdapterT* adapter = (CAN_LocalRequestControlAdapterT*)control->Adapter.Content;

	switch ((uint32_t)selector)
	{
		case xRequestControlAdapterRequestLock:
#ifdef INC_FREERTOS_H
			xSemaphoreTake(adapter->Content.CoreMutex, portMAX_DELAY);
#endif
			break;

		case xRequestControlAdapterRequestUnLock:
#ifdef INC_FREERTOS_H
			xSemaphoreGive(adapter->Content.CoreMutex);
#endif
			break;

		case xRequestControlAdapterRequestAdd:
		{
			switch (description)
			{
				case 0: return privateRequestAdd(control, adapter, arg);
				case 1: return privateRequestAddEx1(control, adapter, arg);
			}
			return xResultInvalidRequest;
		}

		case xRequestControlAdapterRequestNew:
		{
			CAN_LocalRequestT* result = NULL;
			CAN_LocalRequestT* buffer = (void*)adapter->RequestBuffer;

			for(uint16_t i = 0; i < adapter->RequestBufferSize; i++)
			{
				if (!buffer[i].Base.IsRunning)
				{
					result = &buffer[i];
					memset(result, 0, sizeof(CAN_LocalRequestT));

					result->Base.AttemptsCount = REQUEST_DEFAULT_ATTEMPTS_COUNT;
					result->Base.TimeOut = REQUEST_DEFAULT_TIMEOUT;
					result->Base.IsRunning = true;
					break;
				}
			}

			*(void**)arg = result;
			break;
		}


		default : return xResultRequestIsNotFound;
	}

	return xResultAccept;
}
//==============================================================================
//initializations:

static xRequestControlAdapterInterfaceT privateAdapterInterface =
{
	.Handler = (xRequestControlAdapterHandlerT)privateHandler,
	.RequestListener = (xRequestControlAdapterRequestListenerT)privateRequestListener,
};
//------------------------------------------------------------------------------
xResult CAN_LocalRequestControlAdapterInit(xRequestControlT* control,
		CAN_LocalRequestControlAdapterT* adapter,
		CAN_LocalRequestControlAdapterInitT* init)
{
	control->Adapter.Description = nameof(CAN_LocalRequestControlAdapterT);
	control->Adapter.Content = adapter;
	control->Adapter.Interface = &privateAdapterInterface;

	adapter->Port = init->Port;
	adapter->RequestBuffer = init->RequestBuffer;
	adapter->RequestBufferSize = init->RequestBufferSize;

#ifdef INC_FREERTOS_H
	adapter->Content.CoreMutex = xSemaphoreCreateMutex();
	control->ProcessedRequests.Content = xSemaphoreCreateMutex();
#endif

	return xResultError;
}
//==============================================================================
