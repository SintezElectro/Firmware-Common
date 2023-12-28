//==============================================================================
//includes:

#include "HostRequestControl-Component.h"
#include "../Adapters/CAN_LocalRequestControl-Adapter.h"

#include "Components.h"
//==============================================================================
//defines:


//==============================================================================
//import:


//==============================================================================
//variables:

static CAN_LocalRequestT CAN_LocalRequestBuffer[HOST_REQUEST_CONTROL_BUFFER_SIZE] HOST_REQUEST_CONTROL_BUFFER_MEM_SECTION;

xRequestControlT HostRequestControl;
//==============================================================================
//functions:

void HostRequestControlComponentHandler()
{
	xRequestControlHandlerDirect(HostRequestControl);
}
//==============================================================================
//initializations:

CAN_LocalRequestControlAdapterT privateLocalRequestControlAdapter;
//==============================================================================
//initialization:

xResult HostRequestControlComponentInit(void* parent)
{
	CAN_LocalRequestControlAdapterInitT adapterInit;
	adapterInit.RequestBuffer = (void*)CAN_LocalRequestBuffer;
	adapterInit.RequestBufferSize = HOST_REQUEST_CONTROL_BUFFER_SIZE;
	adapterInit.Port = &HOST_REQUEST_CONTROL_PORT;
	CAN_LocalRequestControlAdapterInit(&HostRequestControl, &privateLocalRequestControlAdapter, &adapterInit);

	xRequestControlInitT controlInit;
	controlInit.Parent = parent;
	xRequestControlInit(&HostRequestControl, &controlInit);

	return xResultAccept;
}
//==============================================================================
