//==============================================================================
#ifndef _TEMPERATURE_SERVICE_H_
#define _TEMPERATURE_SERVICE_H_
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "Abstractions/xDevice/xService.h"
#include "TemperatureService-AdapterBase.h"
//==============================================================================
//defines:

#define TEMPERATURE_SERVICE_UID 0x5C78700

#define TEMPERATURE_SERVICE_TX_DATA "11223344556677889900112233445566778111111111111111111111111111111111111"\
	"642724574864978764363474626326358888888888888843566313453452363568679567983563253agdgasdbjfDFSDFASGHA89900"\
	"11223344556677889900112233445566778111111111111111111111111111111111111642724574864978764363474626326358888888888888843566313453452363568679567983563253agdgasdbjfDFSDFASGHA89900"\
	"11223344556677889900112233445566778111111111111111111111111111111111111642724574864978764363474626326358888888888888843566313453452363568679567983563253agdgasdbjfDFSDFASGHA89900"
//==============================================================================
/// @defgroup xServices temperature service types
/// @brief функции предостовляемые TemperatureService.c
/// @{

typedef enum
{
	TemperatureServiceRequestIdle = xServiceBaseRequestOffset,

	TemperatureServiceRequestGetTemperature,
	TemperatureServiceRequestSetNotification

} TemperatureServiceRequests;
//------------------------------------------------------------------------------

typedef struct TemperatureServiceT
{
	xServiceT Base;

	float Temperature;

} TemperatureServiceT;
//------------------------------------------------------------------------------

typedef struct
{
	xIAsyncRequestT* Async;

} xServiceRequestGetTemperatureT;
//------------------------------------------------------------------------------

typedef xServiceRequestSetIdT TemperatureServiceRequestSetIdT;
//------------------------------------------------------------------------------

typedef struct
{
	xServiceInitT Base;

} TemperatureServiceInitT;
/// @}
//==============================================================================
/// @defgroup xServices temperature service functions
/// @brief функции предостовляемые слоем TemperatureService.c
/// @{

xResult TemperatureServiceInit(TemperatureServiceT* service, TemperatureServiceInitT* init);

/// @}
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_TEMPERATURE_SERVICE_H_
