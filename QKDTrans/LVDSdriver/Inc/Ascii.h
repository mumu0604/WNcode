#ifndef __ASCII_H
#define __ASCII_H


/******************************************************************************
 *
 * File Name:
 *
 *      Ascii.h
 *
 * Description:
 *
 *      Constants and macros for ASCII token convertions.
 *
 *****************************************************************************/




/*****************************************************************
 *  Constants, used in serial downloads
 ****************************************************************/
#define NUL         0x00     /* NULL */
#define SOH         0x01     /* Start of Header */
#define STX         0x02
#define EXT         0x03
#define EOT         0x04     /* End of Transmission */
#define ENQ         0x05
#define ACK         0x06     /* Acnowledge */
#define BEL         0x07     /* Bell */
#define BS          0x08
#define TAB         0x09     /* Tab */
#define LF          0x0A
#define VT          0x0B
#define FF          0x0C
#define CR          0x0D
#define SO          0x0E
#define SI          0x0F
#define DLE         0x10
#define DC1         0x11
#define DC2         0x12
#define DC3         0x13
#define DC4         0x14
#define NAK         0x15     /* No Acnowledge */
#define SYN         0x16
#define ETB         0x17
#define CAN         0x18     /* Cancel */
#define EM          0x19
#define SUB         0x1A
#define ESC         0x1B     /* Escape */
#define FS          0x1C
#define GS          0x1D
#define RS          0x1E
#define US          0x1F
#define SP          0x20
#define DEL         0x7F     /* Delete */



#endif
