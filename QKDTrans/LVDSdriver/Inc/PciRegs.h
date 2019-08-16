#ifndef __PCIREGS_H
#define __PCIREGS_H


/******************************************************************************
 *
 * File Name:
 *
 *      PciRegs.h
 *
 * Description:
 *
 *      This file defines the generic PCI Configuration Registers
 *
 ******************************************************************************/


#define CFG_VENDOR_ID           0x000
#define CFG_COMMAND             0x004
#define CFG_REV_ID              0x008
#define CFG_CACHE_SIZE          0x00C
#define CFG_BAR0                0x010
#define CFG_BAR1                0x014
#define CFG_BAR2                0x018
#define CFG_BAR3                0x01C
#define CFG_BAR4                0x020
#define CFG_BAR5                0x024
#define CFG_CIS_PTR             0x028
#define CFG_SUB_VENDOR_ID       0x02C
#define CFG_EXP_ROM_BASE        0x030
#define CFG_CAP_PTR             0x034
#define CFG_RESERVED1           0x038
#define CFG_INT_LINE            0x03C


#endif
