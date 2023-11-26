//==============================================================================
//includes:

#include "LocalTransferLayer-Component.h"
#include "CAN-Ports/CAN_Ports-Component.h"

#include "CAN_Local-Types.h"
#include "Abstractions/xTransferLayer/xTransferLayer.h"
#include "Abstractions/xSystem/xSystem.h"
#include "../Adapters/TransferLayer-Adapter.h"

#include "Components.h"
//==============================================================================
//defines:


//==============================================================================
//import:


//==============================================================================
//variables:

static CAN_LocalTransferT CAN_LocalTransfersBuffer[LOCAL_TRANSFER_LAYER_BUFFER_SIZE];

xTransferLayerT LocalTransferLayer;
//==============================================================================
//functions:

/*
void LocalTransferLayerComponentHandler()
{
	xTransferLayerHandlerDirect(LocalTransferLayer);
}*/
//==============================================================================
//initializations:

static TransferLayerAdapterT privateLocalTransferLayerAdapter;
//==============================================================================
//initialization:

xResult LocalTransferLayerComponentInit(void* parent)
{
	TransferLayerAdapterInitT adapterInit;
	adapterInit.Port = &LOCAL_TRANSFER_LAYER_PORT;
	TransferLayerAdapterInit(&LocalTransferLayer, &privateLocalTransferLayerAdapter, &adapterInit);

	xTransferLayerInitT init;
	init.Parent = parent;
	init.Transfers = (void*)&CAN_LocalTransfersBuffer;
	init.TransfersCount = LOCAL_TRANSFER_LAYER_BUFFER_SIZE;
	xTransferLayerInit(&LocalTransferLayer, &init);

	return xResultAccept;
}
//==============================================================================
