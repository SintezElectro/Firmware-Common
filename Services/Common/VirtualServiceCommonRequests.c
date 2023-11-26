//==============================================================================
//includes:

#include "CAN_Local-Types.h"
#include "VirtualServiceCommonRequests.h"
#include "Components/RequestControl/Host/HostRequestControl-Component.h"
#include "Components/Devices/Host/HostDevice-Component.h"
#include "Common/xMemory.h"
//==============================================================================
//defines:


//==============================================================================
//types:


//==============================================================================
//variables:


//==============================================================================
//functions:

static void privateRequestEventListener(xRequestControlT* control, int selector, xRequestT* request, ...)
{
	xIAsyncRequestT* asyncRequest = request->Content;

	if (!asyncRequest)
	{
		return;
	}

	xIAsyncResultT asyncResult = { 0 };
	asyncResult.Holder = asyncRequest->Holder;
	asyncResult.Operation = selector;
	asyncResult.OperationResult = request->Result;
	asyncResult.Content = asyncRequest->Content;

	if (request->Result == xRequestResultNoError)
	{
		asyncResult.Result = request->RxData;
		asyncResult.ResultSize = request->RxDataSize;
	}

	if(asyncRequest->Callback)
	{
		asyncRequest->Callback(&asyncResult);
	}

	xMemoryFree(asyncRequest);
}
//------------------------------------------------------------------------------
xResult VirtualServiceRequestSetParameter(xServiceT* sender,
		xServiceT* recipient,
		uint16_t action,
		xIAsyncRequestT* asyncRequest,
		void* data,
		uint8_t dataSize)
{
	CAN_LocalRequestT* request = xRequestNew(&HostRequestControl);
	request->Base.Sender = sender != NULL ? sender : (void*)&HostGAP;
	request->Base.EventListener = privateRequestEventListener;
	request->Action = xServiceRequestSetId;
	request->Recipient = (void*)recipient;
	request->Base.Content = asyncRequest != NULL ? xMemoryClone(asyncRequest, sizeof(xIAsyncRequestT)) : NULL;

	request->Base.TxData = data;
	request->Base.TxDataSize = dataSize;

	xRequestControlAdd(&HostRequestControl, (void*)request);

	return xResultAccept;
}
//------------------------------------------------------------------------------
xResult VirtualServiceRequestGetParameter(xServiceT* sender,
		xServiceT* recipient,
		uint16_t action,
		xIAsyncRequestT* asyncRequest,
		void* data,
		uint8_t dataSize)
{
	CAN_LocalRequestT* request = xRequestNew(&HostRequestControl);
	request->Base.Sender = sender != NULL ? sender : (void*)&HostGAP;
	request->Base.EventListener = privateRequestEventListener;
	request->Action = action;
	request->Recipient = (void*)recipient;
	request->Base.Content = asyncRequest != NULL ? xMemoryClone(asyncRequest, sizeof(xIAsyncRequestT)) : NULL;

	request->Base.TxData = data;
	request->Base.TxDataSize = dataSize;

	xRequestControlAdd(&HostRequestControl, (void*)request);

	return xResultAccept;
}
//------------------------------------------------------------------------------
xResult VirtualServiceRequestListener(xServiceT* service, int selector, void* arg, void* adapterRequest)
{
	CAN_LocalRequestDataT requestData;
	uint8_t requestDataSize = 0;

	switch (selector)
	{
		case xServiceRequestSetId:
		{
			xDeviceT* device = xServiceGetDevice(service);

			xServiceRequestSetIdT* request = adapterRequest;

			CAN_LocalRequestContentServiceSetIdT requestContent;
			requestContent.ServiceId = service->Id;
			requestContent.NewServiceId = request->NewId;

			requestData.Value = requestContent.Value;
			requestDataSize = sizeof(CAN_LocalRequestContentServiceSetIdT);

			//set GAP service
			service = device->Services.Head->Value;

			break;
		}

		default : return xResultNotSupported;
	}

	VirtualServiceRequestGetParameter(NULL, (void*)service, selector, arg, &requestData, requestDataSize);

	return xResultAccept;
}
//==============================================================================
