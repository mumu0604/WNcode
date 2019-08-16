

/******************************************************************************
 *
 * File Name:
 *
 *      PlxDefinitionsCheck.h
 *
 * Description:
 *
 *      Verifies definitions required by the PLX 
 *
 ******************************************************************************/




/**********************************************
*       Automatic selection for Windows
**********************************************/
#if defined(_WIN32)
    #if !defined(PCI_CODE)
        #define PCI_CODE
    #endif

    #if !defined(PLX_LITTLE_ENDIAN)
        #define PLX_LITTLE_ENDIAN
    #endif
#endif




/**********************************************
*               Error Checks
**********************************************/
#if !defined(IOP_CODE) && !defined(PCI_CODE)
    #error ERROR: Either IOP_CODE or PCI_CODE must be defined.
#endif

#if !defined(PLX_LITTLE_ENDIAN) && !defined(PLX_BIG_ENDIAN)
    #error ERROR: Either PLX_LITTLE_ENDIAN or PLX_BIG_ENDIAN must be defined.
#endif
