/******************************************************************************
* ApsMgmtInterface.h

* This Module contains methods for handling APS management requests like get,
* set,bind,unbind request coming from the NHLE
*
* Copyright (c) 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
******************************************************************************/
#ifndef _APS_MGMT_INTERFACE_H_
#define _APS_MGMT_INTERFACE_H_
#ifndef __IAR_SYSTEMS_ICC__
#ifdef MC13226Included_d
    #undef MC13226Included_d
#endif    
    #define MC13226Included_d 0
#endif
#include "EmbeddedTypes.h"
#include "ZdoApsInterface.h"
/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

#define gDeviceNotInList_c                            0xff

/* Added for security functions to work in the Security hook */
#define mOffsetToExtAddrInNTEntry_c                   2
#define mSelfAddressIndex_c                           254

#define gStartIDForNeighboringTable_c                 100
#define gExtendedAddressNotPresent_c                  0xFF



/******************************************************************************
*******************************************************************************
* Public macros
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/

typedef index_t bindIndex_t;
#define gApsNotInBindingTable_c  gMaxIndex_c
#define gApsBindingTableFull_c   gMaxIndex_c

/*
  Interval APS version of the zbApsmeBindReq_t (over-the-air) structure.
*/
typedef struct apsmeBindReq_tag {
  zbEndPoint_t  srcEndPoint; /* source endpoint to bind */
  zbClusterId_t aClusterId;  /* cluster to bind */
  zbAddrMode_t  dstAddrMode; /* desintation address mode */
  zbIeeeAddr_t  aDstAddr;    /* destination address.. may be group */
  zbEndPoint_t  dstEndPoint; /* destination endpoint (0xff for group) */
} apsmeBindReq_t;

/* bind request */
typedef apsmeBindReq_t apsmeUnbindReq_t;

/* see zbApsmeBindReq_t */
typedef apsmeBindReq_t apsmeBindEntry_t;

/******************************************************************************
*******************************************************************************
* Public memory declarations
*******************************************************************************
******************************************************************************/

/* address map */

#if MC13226Included_d
extern index_t gApsMaxAddressMapEntries;
#else
extern const index_t gApsMaxAddressMapEntries;
#endif

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************
******************************************************************************/

/* is this ourself address? */
bool_t IsSelfIeeeAddress(zbIeeeAddr_t aIeeeAddr);

/* is this nwk address ourselves? */
bool_t IsSelfNwkAddress(zbNwkAddr_t aNwkAddr);

/* Is this one of the broadcast addresses? */
bool_t IsBroadcastAddress(zbNwkAddr_t aNwkAddr);

/* reset the APS information base to power-up state */
void APS_IBReset(zbResetType_t  resetType);

/* Is this id number in the table group?*/
bool_t ApsIsGroupInGroupTable(zbGroupId_t aGroupId);

/* does this binding entry exist? */
bindIndex_t APS_FindBindingEntry(zbApsmeBindEntry_t* pBindEntry);

/* finds the next binding entry from the sourc endpoint */
bindIndex_t APS_FindNextBindingEntry(bindIndex_t iStartIndex, zbEndPoint_t srcEndPoint);

bindIndex_t APS_FindNextEntry(bindIndex_t iStartIndex, apsBindingTable_Ptr_t  *pBindEntry);

/* Remove a device from the Address map if the device is not in use by Aps security material or a Binding*/
bool_t APS_RemoveDeviceFromAddressMap(zbIeeeAddr_t aExtAddr);

/* is there a free entry in the binding table? */
bindIndex_t APS_FindFreeBindingEntry(void);

/* get the binding destination for this bind entry. Cannot fail. */
zbEndPoint_t APS_GetBindDestination(bindIndex_t iBindIndex, zbNwkAddr_t aNwkAddr);

/* add a binding entry to the binding table. Will replace it if it already exists. */
zbStatus_t APSME_BindRequest(zbApsmeBindReq_t* pBindReq);

/* remove a binding entry from the binding table. */
zbStatus_t APSME_UnbindRequest(zbApsmeUnbindReq_t* pUnbindReq);

/*
	APS_ClearBindingTable

	Set free every entry in the binding table.
*/
void APS_ClearBindingTable
(
	void
);

/*
  APS_AddToAddressMap

  Attempts to add this item to the address map.

  If the entry exists (ieee addr), the aNwkAddr won't be updated if 
  aNwkAddr == gApsBindNwkAddrDefault_c (used by binding).

  If a new entry has the aNwkAdddr set to gApsBindNwkAddrDefault_c, then 
  it indicates the short address has not yet been found (can't be used to send).

  Returns the address map index for this extended address -or-
  Returns gAddressMapFull_c if address map is full. 
*/
addrMapIndex_t APS_AddToAddressMap(zbIeeeAddr_t aExtAddr, zbNwkAddr_t aNwkAddr);

/*
  APS_AddToAddressMap

  Attempts to add this item to the address map.

  If the entry exists (ieee addr), the aNwkAddr won't be updated if 
  aNwkAddr == gApsBindNwkAddrDefault_c (used by binding).

  If a new entry has the aNwkAdddr set to gApsBindNwkAddrDefault_c, then 
  it indicates the short address has not yet been found (can't be used to send).

  Once the entry is added, is mark as permanent inthe Aps Address Map Bit mask.

  Returns the address map index for this extended address -or-
  Returns gAddressMapFull_c if address map is full. 
*/
addrMapIndex_t APS_AddToAddressMapPermanent(zbIeeeAddr_t aExtAddr, zbNwkAddr_t aNwkAddr);

void APS_MarkAddrMapEntryPermanent(addrMapIndex_t iIndex, bool_t fPermanent);


/*
  APS_RemoveFromAddressMap

  Remove an entry from the address map, found by its ieee address.
*/
void APS_RemoveFromAddressMap(zbIeeeAddr_t aExtAddr);

/*
  APS_SearchAndRemoveFromAddressMap

  Search using the device's IEEE address, if the address is being used in other table don't remove it.
*/
bool_t APS_SearchAndRemoveFromAddressMap(zbIeeeAddr_t aExtAddr);

/*
  APS_ClearAddressMapTable

  Walks over the whole address map and tries to erase each entry if possible.
*/
void APS_ClearAddressMapTable(void);

/*
  Using the iBindIndex of the binding table gets the map address index and
  copies in aIeeeAddress the Ieee address needed for the binding 
*/
void APS_GetIeeeByIndex(bindIndex_t iBindIndex, zbIeeeAddr_t aIeeeAddr);

/*
  APS_FindIeeeInAddressMap

  Look through the address map for this IEEE address.

  Returns the index into that item if found.
  Returns gNotInAddressMap_c if not found. 
*/
addrMapIndex_t APS_FindIeeeInAddressMap(zbIeeeAddr_t aExtAddr);

/*
  APS_GetNwkAddress

  Returns a pointer to the nwk (short) address where is copied(a pointer to aWhereToCpyNwkAddr), 
  given an ieee (long) address. 
  Returns NULL if not self or in address map (Address map might be in external memory) 
  Always give the location where to copy nwk address.
*/
 uint8_t *APS_GetNwkAddress(uint8_t *pExtAddr, uint8_t aWhereToCpyNwkAddr[]);

/*
  APS_GetIeeeAddress

  Returns a pointer to the ieee (long) address where is copied(a pointer to aWhereToCpyExtAddr), 
  given a nwk (short) address. 
  Returns NULL if not self or in address map(Address map might be in external memory) 
  Always give the location where to copy ieee address.
*/
uint8_t *APS_GetIeeeAddress(uint8_t *pNwkAddr, uint8_t aWhereToCpyExtAddr[]);


/* prototypes for group functions */
void ApsGroupReset(void);

/* internal function to check if this endpoint is a member of this group */
bool_t ApsGroupIsMemberOfEndpoint(zbGroupId_t aGroupId, zbEndPoint_t endPoint);

/*
  See APSME-ADD-GROUP.request, ZigBee Spec Section 2.2.4.5.1

  This function adds endpoint to the given group. The endpoint must be a 
  registered endpoint on this node.

  Note: the endpoint is an endpoint # (1-240), not an index into the endpoint
  array (0-n).

  Returns
  gZbSuccess_c      if worked
  gZbTableFull_c    Group table full
*/
zbStatus_t APSME_AddGroupRequest(zbApsmeAddGroupReq_t *pRequest);

/*
  See APSME-REMOVE-GROUP.request, ZigBee Spec Section 2.2.4.5.3

  Remove the endpoint from the group

  Returns
  gZbSuccess_c    worked
  gZbNoMatch_c    group invalid or endpoint not part of group
*/
zbStatus_t APSME_RemoveGroupRequest(zbApsmeRemoveGroupReq_t *pRequest);

/*
  Remove this endpoint from all groups. To remove all groups, call ApsGroupReset().

  See APSME-REMOVE-ALL-GROUPS.request, ZigBee 1.1 R10 2.2.4.5.5

  Returns
  gZbSuccess_c              Worked
  gZbInvalidEndpoint_c      Failed
*/
zbStatus_t APSME_RemoveAllGroupsRequest(zbApsmeRemoveAllGroupsReq_t *pRequest);



/*
  APS_RemoveEntryFromApsDuplicateTable( zbNwkAddr_t pShortAddress )
 
  reset duplicate table entry when a child has rejoined

  Arguments: pShortAddress: address of interest
 
  Returns: void
*/
void APS_RemoveEntryFromApsDuplicateTable( zbNwkAddr_t pShortAddress );


/******************************************************************************
* This function Handles the Bind Request coming from ZDO & Binding table is
* updated accordingly.
*
* Interface assumptions:
*   The execution of this function modifies the binding table.
*
* Return value:
*   uint8_t. Result of the binding service.
*
******************************************************************************/
#if gBindCapability_d

/* 
  APSME_BindRequest

  Performs a local bind request. No over-the-air behavior. Binding relies on 
  the address map to associate ieee (long) and nwk (short) addresses.

  Returns 
  gZbSuccess_t          if worked
  gZbIllegalDevice_t    if the short or long address not valid
  gZbIllegalRequest_t   if device not on a network
  gZbTableFull_t        if the table is full
  gZbNotSupported_t     if binding not supported
*/
zbStatus_t APSME_BindRequest(zbApsmeBindReq_t* pBindReq);

/* 
  APS_UnbindRequest

  Performs a local unbind request. No over-the-air behavior. Binding relies on 
  the address map to associate ieee (long) and nwk (short) addresses.

  Returns 
  gZbSuccess_t          if worked
  gZbIllegalDevice_t    if the short or long address not valid
  gZbIllegalRequest_t   if device not on a network
  gZbTableFull_t        if the table is full
  gZbNotSupported_t     if binding not supported
*/
zbStatus_t APSME_UnbindRequest(zbApsmeBindReq_t* pBindReq);

#endif /* gBindCapability_d == 1 */

/*
  ZDO_APSME_SapHandler

  SAP Handler for ZDO to access Management Services Provided by the APS
*/
uint8_t ZDO_APSME_SapHandler(apsmeMessage_t *pMsg); 

/* used to signal don't change NwkAddress in address map if exists */
#define gApsBindNwkAddrDefault_c  0xfeff

/*
  APS_FindNwkAddrInAddressMap

  Look through the address map for this network address.

  Returns the index into that item if found.
  Returns gNotInAddressMap_c if not found. 
*/
addrMapIndex_t APS_FindNwkAddrInAddressMap(uint8_t * pNwkAddr);

#if gStandardSecurity_d || gHighSecurity_d
/************************************************************************************
* 4.4.3.1 APSME-TRANSPORT-KEY.request
* The APSME-TRANSPORT-KEY.request primitive is used for transporting a key to
* another device.
*
* 4.4.3.1.1 Semantics of the Service Primitive This primitive shall provide the
* following interface:
*   APSME-TRANSPORT-KEY.request {
*                               DestAddress,
*                               KeyType,
*                               TransportKeyData
*                               }
*
* The ZDO on an initiator device shall generate this primitive when it requires
* a key to be transported to a responder device.
*
* The receipt of an APSME-TRANSPORT-KEY.request primitive shall cause the APSME to
* create a transport-key command packet (see 053474r17 sub-clause 4.4.9.2).
*
* Interface assumptions:
*   The incoming parameter pTransportKeyData is not a null pointer.
*   The KeyType has a value between 0x00 and 0x04.
*   The parameter aDestinationAddress is a valid 64-bit address.
*
* Return value:
*   NONE.
*
************************************************************************************/
extern void ZDO_APSME_Transport_Key_request
(
  zbIeeeAddr_t           aDestinationAddress,
  zbKeyType_t            keyType,
  zbTransportKeyData_t  *pTransportKeyData
);
#endif


#endif /* _APS_MGMT_INTERFACE_H_ */


