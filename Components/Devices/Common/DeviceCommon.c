//==============================================================================
//includes:

#include "DeviceCommon.h"
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

xResult DeviceReceiveData(xDeviceT* device, CAN_LocalSegmentT* segment)
{
	xListElementT* element = xListStartEnumeration((xListT*)&device->Services);
	uint8_t isPersonal = 0;

	while (element && !isPersonal)
	{
		xServiceT* service = element->Value;

		if (service->Adapter.Interface->EventListener)
		{
			service->Adapter.Interface->EventListener(service, xServiceAdapterEventRecieveData, 0, segment, &isPersonal);
		}

		element = element->Next;
	}

	xListStopEnumeration((xListT*)&device->Services);

	if (isPersonal)
	{
		return xResultAccept;
	}

	element = xListStartEnumeration((xListT*)&device->Devices);

	while (element && !isPersonal)
	{
		xDeviceT* totalDevice = element->Value;
		uint8_t isPersonal = 0;

		if (totalDevice->Adapter.Interface->EventListener)
		{
			totalDevice->Adapter.Interface->EventListener(totalDevice, xDeviceAdapterEventRecieveData, 0, segment, &isPersonal);
		}

		if (isPersonal)
		{
			break;
		}

		element = element->Next;
	}

	xListStopEnumeration((xListT*)&device->Devices);

	return isPersonal ? xResultAccept : xResultNotSupported;
}
//==============================================================================
