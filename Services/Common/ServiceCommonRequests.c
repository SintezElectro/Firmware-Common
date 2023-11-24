//==============================================================================
//includes:

#include "ServiceCommonRequests.h"
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
xResult ServiceCommonRequestListener(xServiceT* service, int selector, void* arg, void* adapterRequest)
{
	switch (selector)
	{
		case xServiceRequestSetId:
		{
			xServiceRequestSetIdT* request = adapterRequest;

			CAN_LocalBroadcastContentIdChangedT broadcastContent;
			broadcastContent.NewId = request->NewId;
			broadcastContent.TimeStamp = xSystemGetTime(NULL);

			xRequestT* webRequest = xRequestPrepare(&RequestControl, service,
										CAN_LocalMessageTypeBroadcast,
										CAN_LocalBroadcastPacketTypeIdChanged,
										&broadcastContent,
										sizeof(CAN_LocalBroadcastContentIdChangedT));

			service->Id = request->NewId;

			//xRequestControlAdd(&HostRequestControl, webRequest);

			break;
		}

		default : return xResultNotSupported;
	}

	return xResultAccept;
}
//==============================================================================