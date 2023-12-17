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


//==============================================================================
//initialization:


//------------------------------------------------------------------------------
xResult RelayServiceInit(RelayServiceT* service, RelayServiceInitT* init)
{
	xServiceInit((xServiceT*)service, (xServiceInitT*)init);

	service->Base.Base.Description = &privateObjectDescription;
	service->Base.Info.Type = xServiceTypeGPIO_Control;

	return xResultAccept;
}
//==============================================================================
