//==============================================================================
#ifndef _GAP_SERVICE_ADAPTER_BASE_H_
#define _GAP_SERVICE_ADAPTER_BASE_H_
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "Common/xTypes.h"
//==============================================================================
/// @defgroup GAPServiceBaseAdapter GAPService adapter base types
/// @brief типы предостовляемые GAPService-AdapterBase.h
/// @{

struct GAPServiceT;
//------------------------------------------------------------------------------

typedef enum
{
	GAPServiceAdapterEventIdle,
	
} GAPServiceAdapterEventSelector;
//------------------------------------------------------------------------------

typedef enum
{
	GAPServiceAdapterRequestIdle,

	GAPServiceAdapterRequestDispose,

} GAPServiceAdapterRequestSelector;
//------------------------------------------------------------------------------

typedef xServiceAdapterInterfaceT GAPServiceAdapterInterfaceT;
//------------------------------------------------------------------------------

typedef struct
{
	void* Content;

} GAPServiceAdapterBaseT;

/// @}
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_GAP_SERVICE_ADAPTER_BASE_H_
