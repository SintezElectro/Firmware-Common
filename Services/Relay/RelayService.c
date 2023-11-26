//==============================================================================
//includes:

#include "RelayService.h"
//==============================================================================
//defines:

#define UID xSERVICE_UID
//==============================================================================
//variables:

static const ObjectDescriptionT privateObjectDescription =
{
	.Key = OBJECT_DESCRIPTION_KEY,
	.ObjectId = UID,
	.Type = nameof(RelayServiceT)
};
//==============================================================================
//functions:

static void privateHandler(RelayServiceT* service)
{
	service->Adapter.Interface->Handler((void*)service);
}
//------------------------------------------------------------------------------
static xResult privateRequestListener(RelayServiceT* service, int selector, void* arg)
{
	if (selector > xServiceBaseRequestOffset && service->Base.Info.Type != xServiceTypeGPIO_Control)
	{
		return xResultInvalidRequest;
	}

	switch ((int)selector)
	{
		case RelayServiceRequestGetPinsState:
		{
			break;
		}

		default : return xResultRequestIsNotFound;
	}

	return xResultAccept;
}
//==============================================================================
//initialization:

static xServiceAdapterInterfaceT privateInterface =
{
	.Handler = (xServiceAdapterHandlerT)privateHandler,
	.RequestListener = (xServiceAdapterRequestListenerT)privateRequestListener
};
//------------------------------------------------------------------------------

xResult RelayServiceInit(RelayServiceT* service, RelayServiceInitT* init)
{
	xServiceInit((xServiceT*)service, (xServiceInitT*)init);

	service->Base.Base.Description = &privateObjectDescription;
	service->Base.Info.Type = xServiceTypeGPIO_Control;

	service->Base.Adapter.Interface = service->Adapter.Interface;
	//service->Base.Adapter.Description = nameof(RelayServiceT);

	return xResultAccept;
}
//==============================================================================