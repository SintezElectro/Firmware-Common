//==============================================================================
//includes:

#include "RequestControl-Component.h"
#include "../Adapters/CAN_LocalRequestControl-Adapter.h"

#include "Components.h"
//==============================================================================
//defines:


//==============================================================================
//import:


//==============================================================================
//variables:

static CAN_LocalRequestT CAN_LocalRequestBuffer[REQUEST_CONTROL_BUFFER_SIZE] REQUEST_CONTROL_BUFFER_MEM_SECTION;

xRequestControlT RequestControl;
//==============================================================================
//functions:

void RequestControlComponentHandler()
{
	xRequestControlHandlerDirect(RequestControl);
}
//==============================================================================
//initializations:

static CAN_LocalRequestControlAdapterT privateRequestControlAdapter;
//==============================================================================
//initialization:

xResult RequestControlComponentInit(void* parent)
{
	CAN_LocalRequestControlAdapterInitT adapterInit;
	adapterInit.RequestBuffer = (void*)CAN_LocalRequestBuffer;
	adapterInit.RequestBufferSize = REQUEST_CONTROL_BUFFER_SIZE;
	adapterInit.Port = &REQUEST_CONTROL_PORT;
	CAN_LocalRequestControlAdapterInit(&RequestControl, &privateRequestControlAdapter, &adapterInit);

	xRequestControlInitT controlInit;
	controlInit.Parent = parent;
	xRequestControlInit(&RequestControl, &controlInit);

	return xResultAccept;
}
//==============================================================================
