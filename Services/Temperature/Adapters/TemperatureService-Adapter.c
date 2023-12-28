//==============================================================================
//includes:

#include "Abstractions/xSystem/xSystem-Types.h"
#include "Common/xMemory.h"
#include "Abstractions/xSystem/xSystem.h"
#include "TemperatureService-Adapter.h"
#include "CAN_Local-Types.h"
#include "Services/Common/ServiceCommon.h"
#include "Components/TransferLayer/Local/LocalTransferLayer-Component.h"
//==============================================================================
//defines:


//==============================================================================
//types:


//==============================================================================
//variables:

static const ServiceReceiverInterfaceT privateReceiverInterface;
//==============================================================================
//functions:

static void privateTransferEventListener(xTransferLayerT* layer,
		xTransferT* transfer,
		int selector,
		uint32_t description,
		void* arg)
{
	ServiceCompleteTransfer(layer, transfer);
}
//------------------------------------------------------------------------------
static xResult privateOpenTransferRequestReceiver (xServiceT* service,
	xPortT* port,
	CAN_LocalSegmentT* segment,
	CAN_LocalRequestContentOpenTransferT content)
{
	void* data = NULL;

	if (content.Type == xTransferTypeReceive)
	{
		data = TEMPERATURE_SERVICE_TX_DATA;
	}

	return ServiceAcceptOpenTransfer(service,
			port,
			segment,
			data,
			content.ContantSize,
			privateTransferEventListener);
}
//------------------------------------------------------------------------------
static xResult privateNotificationReceiver(xServiceT* service,
		xPortT* port,
		CAN_LocalSegmentT* segment,
		CAN_LocalBaseEventPacketT content)
{
	ServiceSendToSubscribers(service, 0, 0, NULL);

	return xResultAccept;
}
//------------------------------------------------------------------------------
static xResult privateRequestReceiver(xServiceT* service,
		xPortT* port,
		CAN_LocalSegmentT* segment,
		CAN_LocalRequestContentT content)
{
	float value;

	switch(content.Description.Action)
	{
		case TemperatureServiceRequestGetTemperature:
		{
			value = 10.0f + (float)(xSystemGetRandom() & 0x3fff) / 1000;
			break;
		}

		default: return xResultNotSupported;
	}

	ServiceSendResponse((xServiceT*)service, port, segment, &value, sizeof(value));

	return xResultAccept;
}
//------------------------------------------------------------------------------
static xResult privateRequestListener(TemperatureServiceT* service, int selector, uint32_t mode, void* in, void* out)
{
	void* result;
	uint32_t resultSize;

	switch ((uint32_t)selector)
	{
		case TemperatureServiceRequestGetTemperature:
		{
			service->Temperature = 50.0f + (float)(xSystemGetRandom() & 0x3fff) / 1000;
			result = &service->Temperature;
			resultSize = sizeof(service->Temperature);
			break;
		}

		default : return ServiceRequestListener((void*)service, selector, mode, in, out);
	}

	ServiceAccomplishAsyncRequest(out, xResultAccept, result, resultSize);

	return xResultAccept;
}
//------------------------------------------------------------------------------
static void privateEventListener(TemperatureServiceT* service, int selector, uint32_t mode, void* in, void* out)
{
	TemperatureServiceAdapterT* adapter = service->Base.Adapter.Content;

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
	.OpenTransferRequestReceiver = privateOpenTransferRequestReceiver,
	.NotificationReceiver = privateNotificationReceiver,
	.RequestReceiver = privateRequestReceiver
};
//==============================================================================

xResult TemperatureServiceAdapterInit(TemperatureServiceT* service,
		TemperatureServiceAdapterT* adapter,
		TemperatureServiceAdapterInitT* init)
{
	if (service && init)
	{
		service->Base.Adapter.Content = adapter;
		service->Base.Adapter.Interface = &privateInterface;
		//service->Adapter.Description = nameof(TemperatureServiceAdapterT);

		adapter->Port = init->Port;

		service->Base.IsEnable = true;
		service->Base.IsAvailable = true;

		return xResultAccept;
	}
  
  return xResultError;
}
//==============================================================================
