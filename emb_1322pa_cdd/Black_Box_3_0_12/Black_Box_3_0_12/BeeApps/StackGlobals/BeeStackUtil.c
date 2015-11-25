/******************************************************************************
* This is the public Source file is for the functions related to the NVM Save,
* Restore and update.
*
* (c) Copyright 2008, Freescale, Inc. All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
******************************************************************************/

#include "EmbeddedTypes.h"
#include "ZdoApsInterface.h"
#include "AppZdoInterface.h"
#ifndef gHostApp_d
#include "AppAspInterface.h"
#endif
#include "ZdoNwkInterface.h"
#include "MsgSystem.h"
#include "PublicConst.h"
#include "NVM_Interface.h"
#include "BeeStackConfiguration.h"
#include "ZdoMain.h"
#include "BeeStack_Globals.h"
#include "BeeStackUtil.h"
#ifndef gHostApp_d
#include "MacPhyInit.h"
#endif
/******************************************************************************
*******************************************************************************
* Private data
*******************************************************************************
******************************************************************************/

uint8_t maRandomNumbers[] = { 0x22, 0x18, 0x16 };

/* used to limit what nodes any given node can hear. For demo and debug purposes. */
#if gICanHearYouTableCapability_d
extern zbNwkAddr_t gaICanHearYouTable[];
extern uint8_t gaICanHearYouLqi[];
extern uint8_t gICanHearYouCounter;
extern const uint8_t gICanHearYouMax;
#endif
extern uint8_t giIcanHearLinkCost;


/******************************************************************************
*******************************************************************************
* Private Prototypes
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private types
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/
/*******************************************************************************
* Swaps the two bytes of the uint16 received
********************************************************************************/
#ifdef __IAR_SYSTEMS_ICC__   
#if !MC13226Included_d
uint16_t Swap2Bytes(uint16_t iOldValue)
{
  uint16_t temp;
  temp = (iOldValue & 0x00FF) << 8;
  temp = ((iOldValue & 0xFF00)>>8) + temp;
  return temp;
}
#endif
#endif

/*******************************************************************************
* Create the extended address (if all 0xffffffffff and not in NVM, then 
********************************************************************************/
void BUtl_CreateExtendedAddress( void )
{
  uint16_t randomValue;
  /* constant MSB in generated MAC address */
  uint8_t aExtenAddr[3] = { 0xc2,0x50,0x00 };
  index_t i;
  
  /* If self ieee address was not initialized with a valid value within
  Platform_Init()	then check if there is a valid address in the NVM */
  /* Note: MAC resets the address to 0xffffffff */
  if(Cmp8BytesToFs( NlmeGetRequest(gNwkIeeeAddress_c))) {
    
    /* If the ieee address contained in NVM is valid, then use it as self 
    ieee address */
    if( !Cmp8BytesToZero( gNwkData.aExtendedAddress ) ) {
      NlmeSetRequest( gNwkIeeeAddress_c, gNwkData.aExtendedAddress );
#ifdef PROCESSOR_MC1323X
      MacPhyInit_WriteExtAddress(aExtendedAddress);
#endif //PROCESSOR_MC1323X      
    }
    /* If ieee address in NVM is not valid then generate a random one */
    else {
      
      for(i = 0; i<3; ++i) {
        /*Generating Random extended address for the 3rd and 4th byte*/
        /*The next line use a specifc value taken from the Hardware in this case the radio so is very important
        to have a specific seed for each borad */
#ifdef __IAR_SYSTEMS_ICC__ 
        randomValue = GetRandomNumber();
#else
#ifndef gHostApp_d
        randomValue = BeestackGetRandomNum(maRandomNumbers[ i ]);
#else
        randomValue = GetRandomNumber();
#endif        

#endif        
        /* The function used to get the values form the hardware is used to seed the new random number genertor */
        FLib_MemCpy(NlmeGetRequest(gNwkIeeeAddress_c) + (i<<1),&randomValue, sizeof(randomValue) );
      }
      
      /* copy in hard-coded MAC addr */
      FLib_MemCpy( NlmeGetRequest(gNwkIeeeAddress_c)+(8-sizeof(aExtenAddr)),aExtenAddr,sizeof(aExtenAddr));
    }
  }
  
  /* Sync the NVM structure with the MAC IEEE address  */
  Copy8Bytes(gNwkData.aExtendedAddress, NlmeGetRequest(gNwkIeeeAddress_c));
#ifdef PROCESSOR_MC1323X
      MacPhyInit_WriteExtAddress(gNwkData.aExtendedAddress);
#endif //PROCESSOR_MC1323X       
#ifdef __IAR_SYSTEMS_ICC__   
#ifndef gHostApp_d
  MacPhyInit_WriteExtAddress(NlmeGetRequest(gNwkIeeeAddress_c));
#endif  
#endif  
}

/*****************************************************************************/
#ifndef gHostApp_d
#define gCapInfoRcvrOnIdle_c 0x08
#define mNumRetries_c        500
#include "nwkmacinterface.h"
#include "appaspinterface.h"

static bool_t mBUtl_ReceiverIsTurnedOn = TRUE;

/* Turn the 802.15.4 receiver off. */
void BUtl_SetReceiverOn(void) {
  bool_t status = TRUE;

  /* Don't turn it on if its already on. */
  if (mBUtl_ReceiverIsTurnedOn) {
    return;
  }

#if gRouterCapability_d || gCoordinatorCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
    if (NlmeGetRequest(gDevType_c) != gEndDevice_c)
#endif
      (void) SetPibAttributeValue( gMPibRxOnWhenIdle_c, &status);
#endif

#if gEndDevCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
    if (NlmeGetRequest(gDevType_c) == gEndDevice_c)
#endif
    {
      if ( IsLocalDeviceReceiverOnWhenIdle() )
      {
        (void) SetPibAttributeValue( gMPibRxOnWhenIdle_c, &status);
      }
    }
#endif

  mBUtl_ReceiverIsTurnedOn = TRUE;
}                                       /* BUtl_SetReceiverOn() */

/* Turn the 802.15.4 receiver off. */
void BUtl_SetReceiverOff(void) {
  bool_t status = FALSE;
  uint16_t i = 0;

  /* Don't turn it off twice. */
  if (!mBUtl_ReceiverIsTurnedOn) {
    return;
  }

  (void) SetPibAttributeValue(gMPibRxOnWhenIdle_c, &status);

  while ((Asp_GetMacStateReq() != gAspMacStateIdle_c) && (i++ < mNumRetries_c)) {
    Mlme_Main(gMacEventMemMlmeMsgQueued_c | gMacEventNwkMcpsMsgQueued_c |
              gMacEventNwkMlmeMsgQueued_c | gMacEventMemAspMsgQueued_c);
  }

  mBUtl_ReceiverIsTurnedOn = FALSE;
}                                       /* BUtl_SetReceiverOff() */

#endif                                      

#if gMsgDebugEnabled_d

/* this function is called to free a previous allocated message, but before that
it fills that memory with 0xFE */
void MSG_DebugFree(void* pMsg)
{
  FLib_MemSet(pMsg,0xFE,sizeof(block1sizeOrigin_t));
  MM_Free(pMsg);
}


/* this function is called to allocate a message, after that
it fills that memory with 0xA1 */
void* MSG_DebugAlloc(uint8_t size)
{ void* pMsg;
  pMsg = MM_Alloc(size);
  if(pMsg)
    FLib_MemSet(pMsg,0xA1,sizeof(block1sizeOrigin_t));
  return pMsg;
}

#endif

/********************************************************************************

  Check is a specific address is within the ICanHearYouTable, return true if it 
  is within the table, it also returns true if global variable 
  gICanHearYouCounter is equeal to zero which means it can hear all addresses 
  returns false if gICanHearYouCounter is different than 0 and the given 
  address was not found within the table

*********************************************************************************/
bool_t CanIHearThisShortAddress(zbNwkAddr_t aSourceAddress)
{
#if gICanHearYouTableCapability_d
  uint8_t iIndex=0;

  /* assume no LQI override */
  giIcanHearLinkCost = 0;

  /* nothing in table, assume we can hear all nodes */
  if(!gICanHearYouCounter)
    return TRUE;

  /* look for it in the table */
	for(iIndex=0;iIndex<gICanHearYouCounter;iIndex++)
	{
		if(IsEqual2Bytes(aSourceAddress, gaICanHearYouTable[iIndex]))
		{
#if gICanHearYouTableCapability_d
      /* user want to override the incoming LQI */
      /* see NwkDataServices.c, ProcessMcpsDataIndication() */
      giIcanHearLinkCost = gaICanHearYouLqi[iIndex];
#endif
      return TRUE;
    }
	}

	/*If not found in table return False */
	return FALSE;
#else
  (void)aSourceAddress;
  return TRUE;
#endif
}


#if gICanHearYouTableCapability_d
/********************************************************************************

  Sets up an i-can-hear-you table, which allows for table-top routing.

  If a NwkAddr is NOT in the i-can-hear-you-table, then the incoming MCPS-DATA.indication is dropped.

  The pLqiList list must have the same count as the pAddressList. If the entry is 0, then LQI is
  not overridden for that entry. If non-zero, then that hard-coded value is used for LQI for incoming
  packets from that node.

  Set addressCounter to 0 to disable the table completely.

*********************************************************************************/
void SetICanHearYouTable
  (
  index_t addressCounter, 
  zbNwkAddr_t *pAddressList,
  uint8_t *pLqiList
  )
{
  /* limit so we don't overrun the array */
  if(addressCounter > gICanHearYouMax)
    addressCounter = gICanHearYouMax;
	gICanHearYouCounter = addressCounter;

  /* copy in the addresses and LQI overrides */
  FLib_MemCpy(gaICanHearYouTable, pAddressList, sizeof(zbNwkAddr_t) * addressCounter);
  FLib_MemCpy(gaICanHearYouLqi, pLqiList, addressCounter);
}

/********************************************************************************

  Receives a pointer to the destination buffer where the table is going to be
  copy to, and the size of destination buffer. Returns number of table entries
  copied to destination buffer. 

*********************************************************************************/
index_t GetICanHearYouTable
  (
  zbNwkAddr_t *pDstTable,
  index_t maxElementsInDstTable
  )
{
  index_t count;

  /* determine # to transfer */
  count = gICanHearYouCounter;
  if(count > maxElementsInDstTable)
    count = maxElementsInDstTable;

  /* return the */
	FLib_MemCpy(pDstTable, gaICanHearYouTable, count * sizeof(zbNwkAddr_t));
	return count;;
}
#endif /*gICanHearYouTableCapability_d*/



/******************************************************************************
*******************************************************************************
* Private Functions
*******************************************************************************
******************************************************************************/

