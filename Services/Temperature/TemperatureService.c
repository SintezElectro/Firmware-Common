//==============================================================================
//includes:

#include "TemperatureService.h"
//==============================================================================
//defines:

#define UID xSERVICE_UID
//==============================================================================
//variables:

static const ObjectDescriptionT privateObjectDescription =
{
	.Key = OBJECT_DESCRIPTION_KEY,
	.ObjectId = UID,
	.Type = nameof(TemperatureServiceT)
};
//==============================================================================
//functions:


//==============================================================================
//initialization:

xResult TemperatureServiceInit(TemperatureServiceT* service, TemperatureServiceInitT* init)
{
	xServiceInit((xServiceT*)service, (xServiceInitT*)init);

	service->Base.Base.Description = &privateObjectDescription;
	service->Base.Info.Type = xServiceTypeTemperatureControl;

	//service->Base.Adapter.Interface = service->Adapter.Interface;
	//service->Base.Adapter.Description = nameof(TemperatureServiceT);

	return xResultAccept;
}
//==============================================================================
