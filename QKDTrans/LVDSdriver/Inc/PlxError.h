#ifndef __PLX_ERROR_H
#define __PLX_ERROR_H


/******************************************************************************
 *
 * File Name:
 *
 *      PlxError.h
 *
 * Description:
 *
 *      This file defines all the error codes for PLX PCI.
 *
 ******************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif




/******************************************
*             Definitions
******************************************/
#define API_RETURN_CODE_STARTS              0x200   /* Starting return code */


/* API Return Code Values */
typedef enum _RETURN_CODE
{
    ApiSuccess = API_RETURN_CODE_STARTS,
    ApiFailed,
    ApiAccessDenied,
    ApiDmaChannelUnavailable,
    ApiDmaChannelInvalid,
    ApiDmaChannelTypeError,
    ApiDmaInProgress,
    ApiDmaDone,
    ApiDmaPaused,
    ApiDmaNotPaused,
    ApiDmaCommandInvalid,
    ApiDmaManReady,
    ApiDmaManNotReady,
    ApiDmaInvalidChannelPriority,
    ApiDmaManCorrupted,
    ApiDmaInvalidElementIndex,
    ApiDmaNoMoreElements,
    ApiDmaSglInvalid,
    ApiDmaSglQueueFull,
    ApiNullParam,
    ApiInvalidBusIndex,
    ApiUnsupportedFunction,
    ApiInvalidPciSpace,
    ApiInvalidIopSpace,
    ApiInvalidSize,
    ApiInvalidAddress,
    ApiInvalidAccessType,
    ApiInvalidIndex,
    ApiMuNotReady,
    ApiMuFifoEmpty,
    ApiMuFifoFull,
    ApiInvalidRegister,
    ApiDoorbellClearFailed,
    ApiInvalidUserPin,
    ApiInvalidUserState,
    ApiEepromNotPresent,
    ApiEepromTypeNotSupported,
    ApiEepromBlank,
    ApiConfigAccessFailed,
    ApiInvalidDeviceInfo,
    ApiNoActiveDriver,
    ApiInsufficientResources,
    ApiObjectAlreadyAllocated,
    ApiAlreadyInitialized,
    ApiNotInitialized,
    ApiBadConfigRegEndianMode,
    ApiInvalidPowerState,
    ApiPowerDown,
    ApiFlybyNotSupported,
    ApiNotSupportThisChannel,
    ApiNoAction,
    ApiHSNotSupported,
    ApiVPDNotSupported,
    ApiVpdNotEnabled,
    ApiNoMoreCap,
    ApiInvalidOffset,
    ApiBadPinDirection,
    ApiPciTimeout,
    ApiDmaChannelClosed,
    ApiDmaChannelError,
    ApiInvalidHandle,
    ApiBufferNotReady,
    ApiInvalidData,
    ApiDoNothing,
    ApiDmaSglBuildFailed,
    ApiPMNotSupported,
    ApiInvalidDriverVersion,
    ApiWaitTimeout,
    ApiWaitCanceled,
    ApiLastError               // Do not add API errors below this line
} RETURN_CODE;



#ifdef __cplusplus
}
#endif

#endif
