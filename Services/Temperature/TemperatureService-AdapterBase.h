//==============================================================================
#ifndef _TEMPERATURE_SERVICE_ADAPTER_BASE_H_
#define _TEMPERATURE_SERVICE_ADAPTER_BASE_H_
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "Common/xTypes.h"
//==============================================================================
/// @defgroup TemperatureServiceBaseAdapter TemperatureService adapter base types
/// @brief типы предостовляемые TemperatureService-AdapterBase.h
/// @{

struct TemperatureServiceT;
//------------------------------------------------------------------------------

typedef enum
{
	TemperatureServiceAdapterRequestIdle = 10000,

	TemperatureServiceAdapterRequestsGet,
	TemperatureServiceAdapterRequestGetTemperature

} TemperatureServiceAdapterRequestSelector;
//------------------------------------------------------------------------------

typedef xServiceAdapterInterfaceT TemperatureServiceAdapterInterfaceT;
//------------------------------------------------------------------------------

typedef struct
{
	//char* Description;
	//void* Content;

	TemperatureServiceAdapterInterfaceT* Interface;

} TemperatureServiceAdapterBaseT;
//------------------------------------------------------------------------------

typedef struct
{
	void* Content;

} TemperatureServiceAdapterBaseInitT;

/// @}
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_TEMPERATURE_SERVICE_ADAPTER_BASE_H_
