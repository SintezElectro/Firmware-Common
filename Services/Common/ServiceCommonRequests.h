//==============================================================================
//header:

#ifndef _SERVICE_COMMON_REQUESTS_
#define _SERVICE_COMMON_REQUESTS_
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

xResult ServiceRequestSetId(xServiceT* service, xIAsyncRequestT* asyncRequest, xServiceRequestSetIdT* request);

xResult ServiceRequestGetParameter(xServiceT* sender,
		xServiceT* recipient,
		uint16_t action,
		xIAsyncRequestT* asyncRequest,
		void* data,
		uint8_t dataSize);

xResult ServiceRequestSetParameter(xServiceT* sender,
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
#endif //_SERVICE_COMMON_REQUESTS_
