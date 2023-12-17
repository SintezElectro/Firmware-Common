//==============================================================================
//includes:

#include "Common/xMemory.h"

#include "HostTransferLayer-Component.h"
#include "CAN-Ports/CAN_Ports-Component.h"

#include "CAN_Local-Types.h"
#include "Abstractions/xTransferLayer/xTransferLayer.h"
#include "Abstractions/xSystem/xSystem.h"
#include "../Adapters/TransferLayer-Adapter.h"

#include "Components.h"
//==============================================================================
//defines:

#define HOST_TRANSFER_LAYER_BUFFER_SIZE 20
//==============================================================================
//import:


//==============================================================================
//variables:

static CAN_LocalTransferT CAN_HostTransfersBuffer[HOST_TRANSFER_LAYER_BUFFER_SIZE];

xTransferLayerT HostTransferLayer;

static uint32_t privateTimeStamp;
static uint32_t privateTimeStamp1;

static xTransferT privateTransfer;
static xTransferT* privateMasterTransfer;

volatile static struct
{
	uint32_t CAN_Local1NoiseIsEnable : 1;
	uint32_t CAN_Local2NoiseIsEnable : 1;

} privateFlags;
//==============================================================================
//functions:

static void transferComplite(xTransferLayerT* layer, xTransferT* transfer, int selector, void* arg)
{
	transfer->State = xTransferStateIdle;

	xMemoryFree(transfer->Data);
}
//------------------------------------------------------------------------------
void HostTransferLayerComponentHandler()
{
	xTransferLayerHandlerDirect(HostTransferLayer);

	uint32_t time = xSystemGetTime(NULL);
	if (time - privateTimeStamp > 2000)
	{
		privateTimeStamp = time;

		privateMasterTransfer = &privateTransfer;
		if (privateMasterTransfer->State == xTransferStateIdle)
		{
			privateMasterTransfer->Holder = HostDevice.Services.Head->Value;
			privateMasterTransfer->Id = TemperatureService3.Base.Id;
			//privateMasterTransfer.Base.Type = xTransferTypeTransmite;
			//privateMasterTransfer.Base.ValidationIsEnabled = true;
			privateMasterTransfer->MasterModeIsEnabled = true;

			privateMasterTransfer->TimeOut = 1000;
			privateMasterTransfer->EventListener = transferComplite;
			privateMasterTransfer->TransmittingAttempts = 1;

			if (privateMasterTransfer->Type == xTransferTypeTransmite)
			{
				privateMasterTransfer->Data = (uint8_t*)TEMPERATURE_SERVICE_TX_DATA;
				privateMasterTransfer->DataLength = sizeof_str(TEMPERATURE_SERVICE_TX_DATA);
			}
			else if (privateMasterTransfer->Type == xTransferTypeReceive)
			{
				privateMasterTransfer->Data = xMemoryAllocate(1, sizeof_str(TEMPERATURE_SERVICE_TX_DATA));
				privateMasterTransfer->DataLength = sizeof_str(TEMPERATURE_SERVICE_TX_DATA);
			}

			xTransferLayerAdd(&HostTransferLayer, privateMasterTransfer);
		}
	}

	if (time - privateTimeStamp1 > 10)
	{
		privateTimeStamp1 = time;

		CAN_LocalSegmentT packet;
		packet.ExtensionHeader.MessageType = CAN_LocalMessageTypeNotification;
		packet.ExtensionHeader.ServiceType = 11;
		packet.ExtensionHeader.ServiceId = 333;
		packet.ExtensionIsEnabled = true;

		packet.DataLength = 8;

		if (privateFlags.CAN_Local1NoiseIsEnable)
		{
			xPortExtendedTransmition(&CAN_Port1, &packet);
		}

		packet.ExtensionHeader.ServiceId = 334;

		if (privateFlags.CAN_Local2NoiseIsEnable)
		{
			xPortExtendedTransmition(&HOST_TRANSFER_LAYER_PORT, &packet);
		}
	}
}
//==============================================================================
//initializations:

static TransferLayerAdapterT privateHostTransferLayerAdapter;

//==============================================================================
//initialization:

xResult HostTransferLayerComponentInit(void* parent)
{
	TransferLayerAdapterInitT adapterInit;
	adapterInit.Port = &HOST_TRANSFER_LAYER_PORT;
	TransferLayerAdapterInit(&HostTransferLayer, &privateHostTransferLayerAdapter, &adapterInit);

	xTransferLayerInitT init;
	init.Parent = parent;
	init.Transfers = (void*)&CAN_HostTransfersBuffer;
	init.TransfersCount = HOST_TRANSFER_LAYER_BUFFER_SIZE;
	xTransferLayerInit(&HostTransferLayer, &init);

	return xResultAccept;
}
//==============================================================================
