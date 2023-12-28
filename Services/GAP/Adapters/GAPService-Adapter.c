//==============================================================================
//includes:

#include "Services/Common/ServiceCommon.h"
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

static const ServiceReceiverInterfaceT privateReceiverInterface;
//==============================================================================
//functions:

static xResult privateOpenTransferRequestReceiver (xServiceT* service,
	xPortT* port,
	CAN_LocalSegmentT* segment,
	CAN_LocalRequestContentOpenTransferT content)
{
	return xResultNotSupported;
}
//------------------------------------------------------------------------------
static xResult privateRequestReceiver(xServiceT* service,
		xPortT* port,
		CAN_LocalSegmentT* segment,
		CAN_LocalRequestContentT content)
{
	xDeviceT* device = xServiceGetDevice(service);

	CAN_LocalResponseContentDataT payload = { 0 };
	uint8_t payloadSize = 0;

	switch (content.Description.Action)
	{
		case GAPServiceRequestGetNumberOfServices:
		{
			payload.Bytes[0] = device->Services.Count;
			payloadSize = sizeof(payload.Bytes[0]);

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

			payload.Content = responseContent.Value;
			payloadSize = sizeof(CAN_LocalResponseGATGetServiceT);

			break;
		}

		case xServiceRequestSetId:
		{
			CAN_LocalRequestContentServiceSetIdT requestContent = { .Value = content.Data.Content };
			xServiceT* targetService = xDeviceGetServiceById(service->Base.Parent, requestContent.ServiceId);

			if (targetService != NULL)
			{
				xServiceRequestSetIdT parameter;
				parameter.NewId = requestContent.NewServiceId;
				xServiceRequestListener((xServiceT*)targetService, xServiceRequestSetId, 0, &parameter, NULL);
			}
			break;
		}

		default: return xResultNotSupported;
	}

	ServiceSendResponse((void*)service,
			port,
			segment,
			&payload,
			payloadSize);

	return xResultAccept;
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
	GAPServiceAdapterT* adapter = service->Base.Adapter.Content;

	switch (selector)
	{
		case xServiceAdapterEventRecieveData:
		{
			uint8_t isPersonal = ServicePacketReceiver((void*)service,
					adapter->Port,
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
static void privateHandler(GAPServiceT* service)
{

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
	.OpenTransferRequestReceiver = privateOpenTransferRequestReceiver,
	.RequestReceiver = privateRequestReceiver
};
//==============================================================================
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
