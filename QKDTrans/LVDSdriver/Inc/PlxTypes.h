#ifndef __PLXTYPES_H
#define __PLXTYPES_H


/******************************************************************************
 * 
 * File Name:
 *
 *      PlxTypes.h
 *
 * Description:
 *
 *      This file defines the basic types available to the PCI.
 *
 ******************************************************************************/


#include "PlxDefinitionsCheck.h"

#if defined(IOP_CODE)
    #include "LocalTypes.h"
#elif defined(PCI_CODE)
    #include "PciTypes.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif




// Assertion constants
#define AssertBLAST          0
#define EOTAsserted          1

// Hot swap status definitions
#define HS_LED_ON            0x08
#define HS_BOARD_REMOVED     0x40
#define HS_BOARD_INSERTED    0x80


// Access Size Type
typedef enum _ACCESS_TYPE
{
    BitSize8,
    BitSize16,
    BitSize32,
    BitSize64
} ACCESS_TYPE;


// Various DMA states
typedef enum _DMA_STATE
{
    DmaStateClosed,
    DmaStateBlock,
    DmaStateSgl,
    DmaStateShuttle
} DMA_STATE;


// DMA Channel Definitions
typedef enum _DMA_CHANNEL
{
    IopChannel0,
    IopChannel1,
    IopChannel2,
    PrimaryPciChannel0,
    PrimaryPciChannel1,
    PrimaryPciChannel2,
    PrimaryPciChannel3
} DMA_CHANNEL;


// DMA Transfer Direction Type
typedef enum _DMA_DIRECTION
{
    IopToIop,
    IopToPrimaryPci,
    PrimaryPciToIop,
    IopToSecondaryPci,
    SecondaryPciToIop,
    PrimaryPciToSecondaryPci,
    SecondaryPciToPrimaryPci
} DMA_DIRECTION;


// DMA Command Definitions
typedef enum _DMA_COMMAND
{
    DmaStart,
    DmaPause,
    DmaResume,
    DmaAbort,
    DmaStatus
} DMA_COMMAND;


// DMA Channel Priority Definitions
typedef enum _DMA_CHANNEL_PRIORITY
{
    Channel0Highest,
    Channel1Highest,
    Channel2Highest,
    Channel3Highest,
    Rotational
} DMA_CHANNEL_PRIORITY;


// Power State Definitions
typedef enum _PLX_POWER_LEVEL
{
    D0Uninitialized,
    D0,
    D1,
    D2,
    D3Hot,
    D3Cold
} PLX_POWER_LEVEL;


// EEPROM Type Definitions
typedef enum _EEPROM_TYPE
{
    Eeprom93CS46,
    Eeprom93CS56,
    Eeprom93CS66,
    EepromX24012,
    EepromX24022,
    EepromX24042,
    EepromX24162,
    EEPROM_UNSUPPORTED
} EEPROM_TYPE;


// FLASH Device Definitions
typedef enum _FLASH_TYPE
{
    AM29F040,
    AM29LV040B,
    AT49LV040,
    MBM29LV160,
    FLASH_UNSUPPORTED
} FLASH_TYPE;


// PCI Space Definitions
typedef enum _PCI_SPACE
{
    PciMemSpace,
    PciIoSpace
} PCI_SPACE;


// Base Address Register Definitions
typedef enum _BAR_SPACE
{
    Bar0,
    Bar1,
    Bar2,
    Bar3,
    Bar4,
    Bar5,
    IopExpansionRom
} BAR_SPACE;


// Local Space Types
typedef enum _IOP_SPACE
{
    IopSpace0,
    IopSpace1,
    IopSpace2,
    IopSpace3,
    MsLcs0,
    MsLcs1,
    MsLcs2,
    MsLcs3,
    MsDram,
    MsDefault,
    ExpansionRom
} IOP_SPACE;


// Bus Index Types
typedef enum _BUS_INDEX
{
    PrimaryPciBus,
    SecondaryPciBus
} BUS_INDEX;


// Mailbox ID Definitions
typedef enum _MAILBOX_ID
{
    MailBox0,
    MailBox1,
    MailBox2,
    MailBox3,
    MailBox4,
    MailBox5,
    MailBox6,
    MailBox7
} MAILBOX_ID;


// Power Management Data Definitions
typedef enum _POWER_DATA_SELECT
{
    D0PowerConsumed,
    D1PowerConsumed,
    D2PowerConsumed,
    D3HotPowerConsumed,
    D0PowerDissipated,
    D1PowerDissipated,
    D2PowerDissipated,
    D3PowerDissipated
} POWER_DATA_SELECT;


// Power Scale definitions
typedef enum _POWER_DATA_SCALE
{
    PowerScaleUnknown,
    PowerScaleOneTenth,
    PowerScaleOneHundredth,
    PowerScaleOneThousandth
} POWER_DATA_SCALE;


// DMA Channel Descriptor Structure
typedef struct _DMA_CHANNEL_DESC 
{
    unsigned int EnableReadyInput         :1;
    unsigned int EnableBTERMInput         :1;
    unsigned int EnableIopBurst           :1;
    unsigned int EnableWriteInvalidMode   :1;
    unsigned int EnableDmaEOTPin          :1;
    unsigned int DmaStopTransferMode      :1;
    unsigned int HoldIopAddrConst         :1;
    unsigned int HoldIopSourceAddrConst   :1;
    unsigned int HoldIopDestAddrConst     :1;
    unsigned int DemandMode               :1;
    unsigned int SrcDemandMode            :1;
    unsigned int DestDemandMode           :1;
    unsigned int EnableTransferCountClear :1;
    unsigned int WaitStates               :4;
    unsigned int IopBusWidth              :2;
    unsigned int EOTEndLink               :1;
    unsigned int ValidStopControl         :1;
    unsigned int ValidModeEnable          :1;
    unsigned int EnableDualAddressCycles  :1;
    unsigned int Reserved1                :9;
    unsigned int TholdForIopWrites        :4;
    unsigned int TholdForIopReads         :4;
    unsigned int TholdForPciWrites        :4;
    unsigned int TholdForPciReads         :4;
    unsigned int EnableFlybyMode          :1;
    unsigned int FlybyDirection           :1;
    unsigned int EnableDoneInt            :1;
    unsigned int Reserved2                :13;
    DMA_CHANNEL_PRIORITY DmaChannelPriority;
} DMA_CHANNEL_DESC;


// DMA Transfer Element
#if defined(PCI_CODE)
typedef struct _DMA_TRANSFER_ELEMENT
{
    union
    {
        U32      UserVa;                // User space virtual address
        U32      PciAddrLow;            // Lower 32-bits of PCI address
        U32      SourceAddr;            // For 480 Local->Local
    } u;
    U32          PciAddrHigh;           // Upper 32-bits of PCI address
    U32          LocalAddr;             // Local bus address
    U32          DestAddr;              // For 480 Local->Local
    U32          TransferCount;         // Number of bytes to transfer
    U32          NextSglPtr;            // Address of next SGL descriptor
    unsigned int PciSglLoc         :1;  // DMA flags
    unsigned int LastSglElement    :1;
    unsigned int TerminalCountIntr :1;
    unsigned int LocalToPciDma     :1;
} DMA_TRANSFER_ELEMENT;
#endif   // PCI_CODE


#if defined(IOP_CODE)
typedef union _DMA_TRANSFER_ELEMENT
{
    struct
    {
        U32 LowPciAddr;
        U32 IopAddr;
        U32 TransferCount;
    #if defined(PLX_LITTLE_ENDIAN)
        U32 PciSglLoc         :1;
        U32 LastSglElement    :1;
        U32 TerminalCountIntr :1;
        U32 IopToPciDma       :1;
        U32 NextSglPtr        :28;
    #elif defined(PLX_BIG_ENDIAN)
        U32 NextSglPtr        :28;
        U32 IopToPciDma       :1;
        U32 TerminalCountIntr :1;
        U32 LastSglElement    :1;
        U32 PciSglLoc         :1;
    #endif
    } Pci9080Dma;

    struct
    {
        U32 LowPciAddr;
        U32 IopAddr;
        U32 TransferCount;
    #if defined(PLX_LITTLE_ENDIAN)
        U32 PciSglLoc         :1;
        U32 LastSglElement    :1;
        U32 TerminalCountIntr :1;
        U32 IopToPciDma       :1;
        U32 NextSglPtr        :28;
    #elif defined(PLX_BIG_ENDIAN)
        U32 NextSglPtr        :28;
        U32 IopToPciDma       :1;
        U32 TerminalCountIntr :1;
        U32 LastSglElement    :1;
        U32 PciSglLoc         :1;
    #endif
        U32 HighPciAddr;
    } Pci9054Dma, Pci9056Dma, Pci9656Dma;

    struct
    {
        U32 TransferCount;
        union loc1
        {
            U32 LowPciAddr; // DMA channel 0, 1
            U32 SourceAddr; // DMA channel 2
        } Loc1;

        union loc2
        {
            U32 IopAddr;    // DMA channel 0, 1
            U32 DestAddr;   // DMA channel 2
        } Loc2;

    #if defined(PLX_LITTLE_ENDIAN)
        U32 PciSglLoc         :1;
        U32 LastSglElement    :1;
        U32 TerminalCountIntr :1;
        U32 IopToPciDma       :1;
        U32 NextSglPtr        :28;
    #elif defined(PLX_BIG_ENDIAN)
        U32 NextSglPtr        :28;
        U32 IopToPciDma       :1;
        U32 TerminalCountIntr :1;
        U32 LastSglElement    :1;
        U32 PciSglLoc         :1;
    #endif
        U32 HighPciAddr;
    } Iop480Dma;

    /*
     * The DMA Transfer Element must always start on a 16 byte
     * boundary so the following reserve field ensures this. Total size = 0x30.
     */
    U32 Reserved[12];

} DMA_TRANSFER_ELEMENT;
#endif  // IOP_CODE


// DMA SGL Address Data Type
typedef DMA_TRANSFER_ELEMENT          *SGL_ADDR, *PSGL_ADDR;


// DMA Manager initialization Parameters Structure
typedef struct _DMA_PARMS
{
    DMA_CHANNEL           DmaChannel;
    DMA_TRANSFER_ELEMENT *FirstSglElement; 
    U32                  *WaitQueueBase; 
    U32                   NumberOfElements;
} DMA_PARMS;


// PLX Interrupt Structure 
typedef struct _PLX_INTR
{
    unsigned int InboundPost      :1;
    unsigned int OutboundPost     :1;
    unsigned int OutboundOverflow :1;
    unsigned int OutboundOption   :1;
    unsigned int IopDmaChannel0   :1;
    unsigned int PciDmaChannel0   :1;
    unsigned int IopDmaChannel1   :1;
    unsigned int PciDmaChannel1   :1;
    unsigned int IopDmaChannel2   :1;
    unsigned int PciDmaChannel2   :1;
    unsigned int Mailbox0         :1;
    unsigned int Mailbox1         :1;
    unsigned int Mailbox2         :1;
    unsigned int Mailbox3         :1;
    unsigned int Mailbox4         :1;
    unsigned int Mailbox5         :1;
    unsigned int Mailbox6         :1;
    unsigned int Mailbox7         :1;
    unsigned int IopDoorbell      :1;
    unsigned int PciDoorbell      :1;
    unsigned int SerialPort1      :1;
    unsigned int SerialPort2      :1;
    unsigned int BIST             :1;
    unsigned int PowerManagement  :1;
    unsigned int PciMainInt       :1;
    unsigned int IopToPciInt      :1;
    unsigned int IopMainInt       :1;
    unsigned int PciAbort         :1;
    unsigned int PciReset         :1;
    unsigned int PciPME           :1;
    unsigned int Enum             :1;
    unsigned int PciENUM          :1;
    unsigned int IopBusTimeout    :1;
    unsigned int AbortLSERR       :1;
    unsigned int ParityLSERR      :1;
    unsigned int RetryAbort       :1;
    unsigned int LocalParityLSERR :1;
    unsigned int PciSERR          :1;
    unsigned int IopRefresh       :1;
    unsigned int PciINTApin       :1;
    unsigned int IopINTIpin       :1;
    unsigned int TargetAbort      :1;
    unsigned int Ch1Abort         :1;
    unsigned int Ch0Abort         :1;
    unsigned int DMAbort          :1;
    unsigned int IopToPciInt_2    :1;
    unsigned int SwInterrupt      :1;
    unsigned int DmaChannel3      :1;
    unsigned int Reserved         :16;
} PLX_INTR;


// PCI bus properties structure
typedef struct _PCI_BUS_PROP
{
    unsigned int PciRequestMode           :1;
    unsigned int DmPciReadMode            :1;
    unsigned int EnablePciArbiter         :1;
    unsigned int EnableWriteInvalidMode   :1;
    unsigned int DmPrefetchLimit          :1;
    unsigned int PciReadNoWriteMode       :1;
    unsigned int PciReadWriteFlushMode    :1;
    unsigned int PciReadNoFlushMode       :1;
    unsigned int EnableRetryAbort         :1;
    unsigned int WFifoAlmostFullFlagCount :5;
    unsigned int DmWriteDelay             :2;
    unsigned int ReadPrefetchMode         :2;
    unsigned int IoRemapSelect            :1;
    unsigned int EnablePciBusMastering    :1;
    unsigned int EnableMemorySpaceAccess  :1;
    unsigned int EnableIoSpaceAccess      :1;
    unsigned int Reserved1                :10;
    unsigned int ReservedForFutureUse;
} PCI_BUS_PROP;


// PCI Abritration Descriptor Structure
typedef struct _PCI_ARBIT_DESC
{
    U32 PciHighPriority;
} PCI_ARBIT_DESC;


// IOP Bus Properties Structure
typedef struct _IOP_BUS_PROP
{
    unsigned int EnableReadyRecover       :1;
    unsigned int EnableReadyInput         :1;
    unsigned int EnableBTERMInput         :1;
    unsigned int DisableReadPrefetch      :1;
    unsigned int EnableReadPrefetchCount  :1;
    unsigned int ReadPrefetchCounter      :4;
    unsigned int EnableBursting           :1;
    unsigned int EnableIopBusTimeoutTimer :1;
    unsigned int BREQoTimerResolution     :1;
    unsigned int EnableIopBREQo           :1;
    unsigned int BREQoDelayClockCount     :4;
    unsigned int MapInMemorySpace         :1;
    unsigned int OddParitySelect          :1;
    unsigned int EnableParityCheck        :1;
    unsigned int MemoryWriteProtect       :1;
    unsigned int InternalWaitStates       :4;
    unsigned int PciRev2_1Mode            :1;
    unsigned int IopBusWidth              :2;
    unsigned int Reserved1                :4;
    unsigned int Iop480WADWaitStates      :4;
    unsigned int Iop480WDDWaitStates      :4;
    unsigned int Iop480WDLYDelayStates    :3;
    unsigned int Iop480WHLDHoldStates     :3;
    unsigned int Iop480WRCVRecoverStates  :3;
    unsigned int Reserved2                :15;
    unsigned int Iop480RADWaitStates      :4;
    unsigned int Iop480RDDWaitStates      :4;
    unsigned int Iop480RDLYADelayStates   :3;
    unsigned int Iop480RDLYDDelayStates   :3;
    unsigned int Iop480RRCVRecoverStates  :3;
    unsigned int Reserved3                :15;
    unsigned int DramRefreshEnable        :1;
    unsigned int DramRefreshInterval      :11;
    unsigned int Iop480TWRdelay           :2;
    unsigned int Iop480W2Wdelay           :2;
    unsigned int Iop480A2Cdelay           :2;
    unsigned int Iop480RRCVdelay          :2;
    unsigned int Iop480PRCGdelay          :2;
    unsigned int Iop480WCWdelay           :2;
    unsigned int Iop480RCWdelay           :2;
    unsigned int Iop480C2Cdelay           :2;
    unsigned int Iop480R2Cdelay           :2;
    unsigned int Iop480R2Rdelay           :2;
} IOP_BUS_PROP;


// IOP Arbitration Descriptor Structure
typedef struct _IOP_ARBIT_DESC
{
    unsigned int IopBusDSGiveUpBusMode    :1;
    unsigned int EnableDSLockedSequence   :1;
    unsigned int GateIopLatencyTimerBREQo :1;
    unsigned int EnableWAITInput          :1;
    unsigned int EnableBOFF               :1;
    unsigned int BOFFTimerResolution      :1;
    unsigned int EnableIopBusLatencyTimer :1;
    unsigned int EnableIopBusPauseTimer   :1;
    unsigned int EnableIopArbiter         :1;
    unsigned int IopArbitrationPriority   :3;
    unsigned int BOFFDelayClocks          :4;
    unsigned int IopBusLatencyTimer       :8;
    unsigned int IopBusPauseTimer         :8;
    unsigned int EnableIopBusTimeout      :1;
    unsigned int IopBusTimeout            :15;
    unsigned int Reserved                 :16;
} IOP_ARBIT_DESC;


// IOP Endian Descriptor Structure
typedef struct _IOP_ENDIAN_DESC
{
    unsigned int BigEIopSpace0            :1;
    unsigned int BigEIopSpace1            :1;
    unsigned int BigEExpansionRom         :1;
    unsigned int BigEMaster480LCS0        :1;
    unsigned int BigEMaster480LCS1        :1;
    unsigned int BigEMaster480LCS2        :1;
    unsigned int BigEMaster480LCS3        :1;
    unsigned int BigEMaster480Dram        :1;
    unsigned int BigEMaster480Default     :1;
    unsigned int BigEIopBusRegion0        :1;
    unsigned int BigEIopBusRegion1        :1;
    unsigned int BigEIopBusRegion2        :1;
    unsigned int BigEIopBusRegion3        :1;
    unsigned int BigEDramBusRegion        :1;
    unsigned int BigEDefaultBusRegion     :1;
    unsigned int BigEDmaChannel0          :1;
    unsigned int BigEDmaChannel1          :1;
    unsigned int BigEIopConfigRegAccess   :1;
    unsigned int BigEDirectMasterAccess   :1;
    unsigned int BigEIopConfigRegInternal :1;
    unsigned int BigEDirectMasterInternal :1;
    unsigned int BigEByteLaneMode         :1;
    unsigned int BigEByteLaneModeLBR0     :1;
    unsigned int BigEByteLaneModeLBR1     :1;
    unsigned int BigEByteLaneModeLBR2     :1;
    unsigned int BigEByteLaneModeLBR3     :1;
    unsigned int BigEByteLaneModeDRAMBR   :1;
    unsigned int BigEByteLaneModeDefBR    :1;
    unsigned int Reserved1                :4;
} IOP_ENDIAN_DESC;


// Power Management Properties
typedef struct _PM_PROP
{
    unsigned int Version            :3;
    unsigned int PMEClockNeeded     :1;
    unsigned int DeviceSpecialInit  :1;
    unsigned int D1Supported        :1;
    unsigned int D2Supported        :1;
    unsigned int AssertPMEfromD0    :1;
    unsigned int AssertPMEfromD1    :1;
    unsigned int AssertPMEfromD2    :1;
    unsigned int AssertPMEfromD3Hot :1;
    unsigned int Read_Set_State     :2;
    unsigned int PME_Enable         :1;
    unsigned int PME_Status         :1;
    unsigned int PowerDataSelect    :3;
    unsigned int PowerDataScale     :2;
    unsigned int PowerDataValue     :8;
    unsigned int Reserved           :4;
} PM_PROP;


// Local API Initialization Parameters Structure
typedef struct _API_PARMS
{
    ADDRESS          PlxChipBaseAddr;
    PCI_BUS_PROP    *pPciBusProp;
    IOP_BUS_PROP    *pIopBus0Prop;
    IOP_BUS_PROP    *pIopBus1Prop;
    IOP_BUS_PROP    *pIopBus2Prop;
    IOP_BUS_PROP    *pIopBus3Prop;
    IOP_BUS_PROP    *pLcs0Prop;
    IOP_BUS_PROP    *pLcs1Prop;
    IOP_BUS_PROP    *pLcs2Prop;
    IOP_BUS_PROP    *pLcs3Prop;
    IOP_BUS_PROP    *pDramProp;
    IOP_BUS_PROP    *pDefaultProp;
    IOP_BUS_PROP    *pExpRomBusProp;
    IOP_ARBIT_DESC  *pIopArbitDesc;
    IOP_ENDIAN_DESC *pIopEndianDesc;
    PM_PROP         *pPMProp;
    U32             *pVPDBaseAddress;
} API_PARMS;




#ifdef __cplusplus
}
#endif

#endif
