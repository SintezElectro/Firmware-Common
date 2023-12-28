//==============================================================================
//includes:

#include "Device1-Component.h"
#include "CAN-Ports/CAN_Ports-Component.h"
#include "Components/TransferLayer/Local/LocalTransferLayer-Component.h"

#include "Services/Temperature/Adapters/TemperatureService-Adapter.h"
#include "Services/GAP/Adapters/GAPService-Adapter.h"
#include "../Adapters/ClientDevice-Adapter.h"
//==============================================================================
//defines:

#define DEVICE_ID 2001
//==============================================================================
//import:


//==============================================================================
//variables:

GAPServiceT Device1GAP DEVICE1_LOCAL_SERVICES_MEM_SECTION;

TemperatureServiceT TemperatureService3 DEVICE1_LOCAL_SERVICES_MEM_SECTION;
TemperatureServiceT TemperatureService4 DEVICE1_LOCAL_SERVICES_MEM_SECTION;

xDeviceT Device1 DEVICE1_MEM_SECTION;
//==============================================================================
//functions:

static void privateServiceEventListener(xServiceT* service, int selector, void* arg)
{
	switch ((int)selector)
	{
		default: break;
	}
}
//------------------------------------------------------------------------------
static void privateDeviceEventListener(xDeviceT* device, xDeviceEventSelector selector, void* arg)
{
	switch ((int)selector)
	{
		case xDeviceEventIdChanged:
		{
			Device1GAP.Base.Id = device->Id;
			break;
		}

		default: break;
	}
}
//------------------------------------------------------------------------------
void Device1ComponentHandler()
{
	xDeviceHandler(&Device1);
}
//------------------------------------------------------------------------------
void Device1ComponentTimeSynchronization()
{

}
//------------------------------------------------------------------------------
static void privateDeviceGAPEventListener(xServiceT* service, int selector, void* arg)
{
	switch ((int)selector)
	{
		default: break;
	}
}
//==============================================================================
//initializations:

static GAPServiceAdapterT privateGAPServiceAdapter DEVICE1_LOCAL_SERVICES_MEM_SECTION;

static ClientDeviceAdapterT privateDeviceAdapter DEVICE1_MEM_SECTION;

static TemperatureServiceAdapterT privateTemperatureServiceAdapter1 DEVICE1_LOCAL_SERVICES_MEM_SECTION;
static TemperatureServiceAdapterT privateTemperatureServiceAdapter2 DEVICE1_LOCAL_SERVICES_MEM_SECTION;
//==============================================================================
//initialization:

xResult Device1ComponentInit(void* parent)
{
	ClientDeviceAdapterInitT deviceAdapterInit = { 0 };
	deviceAdapterInit.TransferLayer = DEVICE_1_TRANSFER_LAYER;
	deviceAdapterInit.Port = &DEVICE_1_PORT;
	ClientDeviceAdapterInit(&Device1, &privateDeviceAdapter, &deviceAdapterInit);

	xDeviceInitT deviceInit = { 0 };
	deviceInit.Parent = parent;
	deviceInit.Id = DEVICE_ID;
	deviceInit.EventListener = (void*)privateDeviceEventListener;
	xDeviceInit(&Device1, &deviceInit);

	Device1.MAC = UniqueDeviceID->MAC;
	//----------------------------------------------------------------------------
	GAPServiceAdapterInitT gapServiceAdapterInit;
	gapServiceAdapterInit.Port = &DEVICE_1_PORT;
	GAPServiceAdapterInit(&Device1GAP, &privateGAPServiceAdapter, &gapServiceAdapterInit);

	GAPServiceInitT gapServiceInit;
	gapServiceInit.Base.EventListener = (void*)privateDeviceGAPEventListener;
	gapServiceInit.Base.Id = DEVICE_ID;
	GAPServiceInit(&Device1GAP, &gapServiceInit);

	xDeviceAddService(&Device1, (xServiceT*)&Device1GAP);
	//----------------------------------------------------------------------------
	TemperatureServiceAdapterInitT temperatureServiceAdapterInit;
	temperatureServiceAdapterInit.Port = &DEVICE_1_PORT;
	TemperatureServiceAdapterInit(&TemperatureService3, &privateTemperatureServiceAdapter1, &temperatureServiceAdapterInit);
	TemperatureServiceAdapterInit(&TemperatureService4, &privateTemperatureServiceAdapter2, &temperatureServiceAdapterInit);

	TemperatureServiceInitT temperatureServiceInit;
	temperatureServiceInit.Base.EventListener = (void*)privateServiceEventListener;

	temperatureServiceInit.Base.Id = TEMPERATURE_SERVICE3_ID;
	TemperatureServiceInit(&TemperatureService3, &temperatureServiceInit);

	temperatureServiceInit.Base.Id = TEMPERATURE_SERVICE4_ID;
	TemperatureServiceInit(&TemperatureService4, &temperatureServiceInit);

	xDeviceAddService(&Device1, (xServiceT*)&TemperatureService3);
	xDeviceAddService(&Device1, (xServiceT*)&TemperatureService4);

	return xResultAccept;
}
//==============================================================================
