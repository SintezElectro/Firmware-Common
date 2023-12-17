//==============================================================================
//includes:

#include <stdlib.h>
#include "Abstractions/xSystem/xSystem.h"
#include "RelayService-Adapter.h"
//==============================================================================
//defines:


//==============================================================================
//types:


//==============================================================================
//variables:


//==============================================================================
//functions:

static void privateHandler(RelayServiceT* service)
{
	
}
//------------------------------------------------------------------------------
static xResult privateRequestListener(RelayServiceT* service, int selector, void* arg)
{
	switch ((uint32_t)selector)
	{
		default : return xResultRequestIsNotFound;
	}

	return xResultAccept;
}
//==============================================================================
//initializations:

static RelayServiceAdapterInterfaceT privateInterface =
{
	.Handler = (xServiceAdapterHandlerT)privateHandler,
	.RequestListener = (xServiceAdapterRequestListenerT)privateRequestListener
};
//------------------------------------------------------------------------------
xResult RelayServiceAdapterInit(RelayServiceT* service,
		RelayServiceAdapterT* adapter,
		RelayServiceAdapterInitT* init)
{
	if (service && init)
	{
		service->Base.Adapter.Content = adapter;
		service->Base.Adapter.Interface = &privateInterface;
		//service->Adapter.Description = nameof(RelayServiceAdapterT);

		return xResultAccept;
	}
  
  return xResultError;
}
//==============================================================================
