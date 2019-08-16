#ifndef __PLX_API_H
#define __PLX_API_H

/******************************************************************************
 *
 * File Name:
 *
 *      PlxApi.h
 *
 * Description:
 *
 *      The main PLX include file
 *
 ******************************************************************************/




/**********************************************
*          Verify Required Definitions
**********************************************/
#include "PlxDefinitionsCheck.h"




/**********************************************
*              Include Files
**********************************************/
#include "Plx.h"
#include "PlxTypes.h"

#if defined(IOP_CODE)
    #include "LocalApi.h"
#endif

#if defined(PCI_CODE)
    #include "PciApi.h"
#endif



#endif
