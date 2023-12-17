//==============================================================================
#ifndef _RELAY_SERVICE_ADAPTER_BASE_H_
#define _RELAY_SERVICE_ADAPTER_BASE_H_
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "Common/xTypes.h"
//==============================================================================
/// @defgroup RelayServiceBaseAdapter RelayService adapter base types
/// @brief типы предостовляемые RelayService-AdapterBase.h
/// @{

struct RelayServiceT;
//------------------------------------------------------------------------------

typedef enum
{
	RelayServiceAdapterEventIdle,
	
} RelayServiceAdapterEventSelector;
//------------------------------------------------------------------------------

typedef enum
{
	RelayServiceAdapterRequestIdle,

	RelayServiceAdapterRequestsGet = 50,
	RelayServiceAdapterRequestGetRelay

} RelayServiceAdapterRequestSelector;
//------------------------------------------------------------------------------

typedef xServiceAdapterInterfaceT RelayServiceAdapterInterfaceT;
//------------------------------------------------------------------------------

typedef struct
{
	//char* Description;
	//void* Content;

	RelayServiceAdapterInterfaceT* Interface;

} RelayServiceAdapterBaseT;
//------------------------------------------------------------------------------

typedef struct
{
	void* Content;

} RelayServiceAdapterBaseInitT;

/// @}
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_RELAY_SERVICE_ADAPTER_BASE_H_
