//==============================================================================
//header:

#ifndef _VIRTUAL_SERVICE_COMMON_
#define _VIRTUAL_SERVICE_COMMON_
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "Abstractions/xDevice/xDevice.h"
#include "Abstractions/xPort/xPort.h"
//==============================================================================
//types:


//==============================================================================
//functions:

xResult VirtualServiceRequestSetId(xServiceT* service, xIAsyncRequestT* asyncRequest, xServiceRequestSetIdT* request);

xResult VirtualServiceRequestGetParameter(xServiceT* sender,
		xServiceT* recipient,
		uint16_t action,
		xIAsyncRequestT* asyncRequest,
		void* data,
		uint8_t dataSize);

xResult VirtualServiceRequestSetParameter(xServiceT* sender,
		xServiceT* recipient,
		uint16_t action,
		xIAsyncRequestT* asyncRequest,
		void* data,
		uint8_t dataSize);
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_VIRTUAL_SERVICE_COMMON_
