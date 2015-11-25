/******************************************************************************
* This header file includes the type definitions and function prototypes used 
* for AF-APP interface.
*
* Copyright (c) 2008, Freescale, Inc. All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
* New Feature! Fragmentation
*
* Example Usage:
*
*  // Fragmented example using AF_DataRequestFragmented()
*  uint8_t a300ByteArray[300];
*  void Send300Bytes(afAddrInfo_t *pAddrInfo)
*  {
*    // fill in global array with bytes 100 x 'A', 'B' and 'C'
*    FLib_MemSet(&a300ByteArray[0],   'A', 100);
*    FLib_MemSet(&a300ByteArray[100], 'B', 100);
*    FLib_MemSet(&a300ByteArray[200], 'C', 100);
* 
*    // send the 300 bytes over-the-air
*    AF_DataRequestFragmented(pAddrInfo,300,a300ByteArray,NULL);
*  }
*
*  // Fragmented example using AF_DataRequestFragmentedNoCopy()
*  void Send300BytesNoCopy(afAddrInfo_t *pAddrInfo)
*  {
*    void *pHead;
*    void *pMsg;
*    uint8_t *pData;
*
*    // allocate 300 bytes, and fill with 100 x 'A', 'B' and 'C'
*    pHead = AF_MsgAllocFragment(NULL, 100, &pData);
*    if(pHead)
*    {
*      FLib_MemSet(pData, 'A', 100);
*      pMsg = AF_MsgAllocFragment(pHead, 100, &pData);
*    }
*    if(pMsg)
*    {
*      FLib_MemSet(pData, 'B', 100);
*      pMsg = AF_MsgAllocFragment(pHead, 100, &pData);
*    }
*    if(pMsg)
*      FLib_MemSet(pData, 'C', 100);
*
*    // send the 300 bytes over-the-air
*    AF_DataRequestFragmentedNoCopy(pAddrInfo,300,pHead,NULL);
*  }
*
*  // Fragmented example with different window sizes, interframe delay and max OTA fragment size
*  void Send300BytesDifferentParameters(afAddrInfo_t *pAddrInfo)
*  {
*    // send first fragmented message with default parameters
*    ApsmeSetRequest(gApsMaxWindowSize_c, 1);        // window can be 1 - 8
*    ApsmeSetRequest(gApsInterframeDelay_c, 100);    // interframe delay can be 1 - 255ms
*    ApsmeSetRequest(gApsMaxFragmentLength_c, 100);  // max len can be 1 - ApsmeGetMaxAsduLength()
*    Send300Bytes(pAddrInfo);
*
*    // send next fragmented message with new parameters
*    ApsmeSetRequest(gApsMaxWindowSize_c, 3);
*    ApsmeSetRequest(gApsInterframeDelay_c, 30);
*    ApsmeSetRequest(gApsMaxFragmentLength_c, 48);
*    Send300BytesNoCopy(pAddrInfo);
*  }
*
******************************************************************************/
#ifndef _APP_AF_INTERFACE_H_
#define _APP_AF_INTERFACE_H_


#include "BeeStackFunctionality.h"
#include "BeeStack_Globals.h"
#include "zigbee.h"
#include "nwkcommon.h"
/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/

/* AF SapHandlers events */
#define gAPSDE_AF_c  (1<<0)

/* AF SapHandlers events */
#define gApp_AF_c    (1<<1)

/* AF reset */
#define gAfReset_c  (1<<2)


/******************************************************************************
*******************************************************************************
* Public type definitions
*******************************************************************************
******************************************************************************/
enum{
  gFrameUnsecured_c,
  gFrameSecuredNetworkkey_c,
  gFrameSecuredLinkkey_c  
};

/* enumeration for end point registration status */
enum {
  gRegSuccess_c,
  gInvalidDeRegReq_c,
  gSimpleDescTooLong    = 254,
  gRegFailure_c         = 255
};  


/* Enumeration for invalid request */
enum {
  gInvalidRequest_c       = 255
};

/* for address mode in the enumeration */
enum {		                
  /* 0x00 = Indirect data request, Device Address and Endpoint fields are 
  ignored.*/
  gAddrNotPresent_c,		
  /* 0x01 = Direct data request with short address of destination Device */
  gShortAndEPAddrPresent_c,
  /* 0x02 = Direct data request with extended address of destination Device */
  gExtAndEPAddrPresent_c
};
#ifdef __IAR_SYSTEMS_ICC__
#pragma pack(1)
#endif
/* Format for Msg data frame */
typedef struct zbMsgData_tag {	
  uint8_t aReserved[ 27 ]; 
  uint8_t	transSeqNumber;
	uint8_t	dataLength;
	uint8_t aData[ 1 ]; /* aData[ 1 ] is a place holder for the data */
}zbMsgData_t;

/* union for type of data frame */
typedef union zbData_tag {
	zbMsgData_t msgData;
}zbData_t;

/* typedef for Frame type */
typedef uint8_t frameType_t;

/* data request structure defination */
typedef struct zdoDataReq_tag {
  zbEndPoint_t    srcEndPoint;
  zdoAddressInfo_t dstAddr;	
  zbClusterId_t   aClusterID;  /* */
  zbApsTxOption_t  txOptions; 
  uint8_t	        radiusCounter;
  frameType_t     frameType;
  zbData_t        data;
} zdoDataReq_t;

/* describes a single indication handler callback */
typedef zbStatus_t (*pfnIndication_t)(zbApsdeDataIndication_t *pIndication, void *pData);

typedef zbStatus_t (*pfnInterPanIndication_t)(zbInterPanDataIndication_t *pIndication, void *pData);

/* Pointer to an attribute validation function */
typedef bool_t (*pfnValidationFunc_t)(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pAttrDef);

/* Used by BeeKit to pull in only the handling code needed for a set of clusters */
typedef struct afClusterDef_tag
{
  zbClusterId_t    aClusterId;    /* cluster ID */
  pfnIndication_t  pfnServerIndication; /* server indication handler for this cluster */
  pfnIndication_t  pfnClientIndication; /* client indication handler for this cluster */
  void            *pAttrList;     /* ptr to attribute list (zclAttrDefList_t) */
  uint16_t          dataOffset;    /* offset of this cluster's data within instance data */
#if gAddValidationFuncPtrToClusterDef_c  
  pfnValidationFunc_t pfnValidationFunc; /* ptr to the attribute validation function */
#endif  
} afClusterDef_t;

/* one per endpoint, this structure defines cluster handlers and attributes for HA */
typedef struct afDeviceDef_tag
{
  pfnIndication_t     pfnZCL;        /* non-cluster specific handling, ZigBee Cluster Library style */
  uint8_t             clusterCount;  /* # of clusters in the list */
  afClusterDef_t      *pClusterDef;  /* ptr to cluster definitions (a list of clusters) */
  uint8_t             reportCount;   /* # of items in the report */
  void                *pReportList;  /* a list of reportable attributes (see haReportAttrList_t) */
  void                *pData;        /* describes the data for this device (profile specific) */
  void                *pSceneData;   /* pointer to scene data and table */
} afDeviceDef_t;

/******************************************************************************
*******************************************************************************
* Public Memory Declarations
*******************************************************************************
******************************************************************************/

/* work-around for BeeKit not being able to output NULLs */
extern const afDeviceDef_t gNoDeviceDef;

/* used for BeeKit autogeneration of HA endpoints */
extern const afDeviceDef_t gHaOnOffLightDeviceDef;
extern const afDeviceDef_t gHaDimmableLightDeviceDef;
extern const afDeviceDef_t gHaOnOffSwitchDeviceDef;
extern const afDeviceDef_t gHaDimmerSwitchDeviceDef;
extern const afDeviceDef_t gHaDoorLockDeviceDef;
extern const afDeviceDef_t gHaDoorLockControllerDeviceDef;
extern const afDeviceDef_t gHaShadeDeviceDef;
extern const afDeviceDef_t gHaShadeControllerDeviceDef;
extern const afDeviceDef_t gHaRangeExtenderDeviceDef;
extern const afDeviceDef_t gHaThermostatDeviceDef;
extern const afDeviceDef_t gHaConsumptionAwernessDeviceDeviceDef;
extern const afDeviceDef_t gHaTemperatureSensorDeviceDef;
extern const afDeviceDef_t gHaHeatingCoolingUnitDeviceDef;
extern const afDeviceDef_t gHaIASZoneDeviceDef;
extern const afDeviceDef_t gHaIASACEDeviceDef;
extern const afDeviceDef_t gHaIASCIEDeviceDef;
extern const afDeviceDef_t gHaIASWDDeviceDef;
extern const afDeviceDef_t gTp2DeviceDef;
extern const afDeviceDef_t gHcThermometerDeviceDef;
extern const afDeviceDef_t gHcDataCollectionUnitDeviceDef;
extern const afDeviceDef_t gHcBpMonitorDeviceDef;
extern const afDeviceDef_t gHcGlucoseMeterDeviceDef;
extern const afDeviceDef_t gHcWeightscaleDeviceDef;
extern const afDeviceDef_t gHcGenericManagerDeviceDef;
extern const afDeviceDef_t gHcGenericAgentDeviceDef;
extern const afDeviceDef_t gHaGenericDeviceDef;
extern const afDeviceDef_t gHaConfigurationToolDeviceDef;
extern const afDeviceDef_t gHaCombinedInterfaceDeviceDef;
extern const afDeviceDef_t gHaSimpleSensorDeviceDef;

/* used for BeeKit autogeneration of private profile endpoints */
extern const afDeviceDef_t gAccelerometerDeviceDef;    /* accelerometer */
extern const afDeviceDef_t gAccelDisplayDeviceDef;     /* accelerometer display */

extern afDeviceDef_t const gSEEnergyServicePortalDeviceDef;
extern afDeviceDef_t const gSEEnergyServiceInterfaceMirrorDeviceDef;
extern afDeviceDef_t const gSEInPremiseDisplayDeviceDef;
extern afDeviceDef_t const gSELoadControlDeviceDef;
extern afDeviceDef_t const gSEMeteringDeviceDef;
extern afDeviceDef_t const gSEPCTDeviceDef;
extern afDeviceDef_t const gSEPrepaymentTerminalDeviceDef;
extern afDeviceDef_t const gSERangeExtenderDeviceDef;
extern afDeviceDef_t const gSESmartApplianceDeviceDef;
/******************************************************************************
*******************************************************************************
* Public Prototypes
*******************************************************************************
******************************************************************************/

/******************************************************************************
  AF_RegisterEndPoint

  Include: AppAfInterface.h

  Register the endpoint. Data can only be recieved on registered endpoints. 
  Call this once for each endpoint upon application task init.

  IN: pEndPoint   must point to a valid endPointDesc_t, which describes the 
                  callbacks for data indications and confirms, as well as 
                  includes any special ZCL data for that endpoint.

  Returns 
  gZbSuccess_c    worked
  gZbFailed_c     already registered or problem with the parameters
  gZbTableFull_c  Table is full
  gZbTooLarge_c   Means there are too many clusters in the simple descriptor. 
                  Reduce # of input or output clusters in the list.
******************************************************************************/
zbStatus_t AF_RegisterEndPoint
(
  const endPointDesc_t *pEndPoint  /* IN: endpoint description to register */
);

/*****************************************************************************
  AF_DeRegisterEndPoint

  Remove an endpoint from the registration table. Doesn't free any memory.
  
  Returns
  gZbSuccess_c  if worked
  gZbFailed_c   if endpoint not present in the table
*****************************************************************************/
zbStatus_t AF_DeRegisterEndPoint
  (
  zbEndPoint_t endPoint /* IN: endpoint number */
  );

/* AF_GetEndPointDevice */
afDeviceDef_t *AF_GetEndPointDevice(zbEndPoint_t endPoint);

/* AF_DeviceDefToEndPoint */
zbEndPoint_t AF_DeviceDefToEndPoint(afDeviceDef_t *pDeviceDef);

/* return endpoint descriptor from endpoint */
zbSimpleDescriptor_t* AF_FindEndPointDesc(uint8_t endPoint);

/* Return TRUE if the given is valid, FALSE otherwise.*/
bool_t ApsIsValidAppEndpoint(zbEndPoint_t endPoint);

/* default radius = 2 * maxdepth from stack profile */
#define afDefaultRadius_c gDefaultRadiusCounter

/* Address Info, used to generate data requests (transmit data over the air) */
/* Note: see also zbApsdeDataReq_t */
typedef struct afAddrInfo_tag
{
	zbAddrMode_t  dstAddrMode;    /* indirect, group, direct-16, direct-64 */
	zbApsAddr_t   dstAddr;        /* short address, long address or group (ignored on indirect mode) */
	zbEndPoint_t  dstEndPoint;    /* destination endpoint (ignored if group mode or indirect) */
	zbClusterId_t aClusterId;      /* what cluster to send from/to? */
	zbEndPoint_t  srcEndPoint;    /* what source endpoint to send from? */
	zbApsTxOption_t txOptions;    /* to ACK or not to ACK, that is the question... */
	uint8_t       radiusCounter;  /* radius */
} afAddrInfo_t;

#ifdef __IAR_SYSTEMS_ICC__
#pragma pack()
#endif
/* 
  AF_MsgAlloc
  
  Allocate a message buffer for building a packet to be sent via 
  AF_DataRequestNoCopy(). Generally used when building larger payloads in-place.
  Alternately, use AF_DataRequest(), where it will make a copy of the payload.
*/
void *AF_MsgAlloc
  (
  void
  );

/*
  AF_Payload

  Use to find the payload location 
*/
#define AF_Payload(pMsg) (void *)(((uint8_t *)(pMsg)) + gAsduOffset_c)

/*
  AF_MaxPayloadLen

  Returns the maximum payload length
*/
#define AF_MaxPayloadLen()  ApsmeGetMaxAsduLength(0)

/*
  AF_DataRequest

  Send data out the radio. Will allocate a message and copy the payload. 
  Use afDataRequestNoCopy if creating a long message in place

  afAddrInfo_t *pAddrInfo,    IN: source and destination information
  uint8_t payloadLen,         IN: length of payload
  void *pPayload,             IN: payload (will be copied from here to a MSG)
  zbApsConfirmId_t *pConfirmId  IN: pass NULL if don't care about confirm

  Returns zbSuccess_c           if worked
  Returns zbBusy_c              if not enough memory to process request
  Returns gZbInvalidParameter_c if invalid parameter
*/
zbStatus_t AF_DataRequest
  (
  afAddrInfo_t *pAddrInfo, 
  uint8_t payloadLen, 
  void *pPayload, 
  zbApsConfirmId_t *pConfirmId
  );

/*
  AF_DataRequestNoCopy

  Send data out the radio. Assumes pMsg has already been allocated,
  and the payload has been filled in.
  
  afAddrInfo_t *pAddrInfo,    IN: source and destination information
  uint8_t payloadLen,         IN: length of payload
  afToApsdeMessage_t *pMsg    IN: allocate msg structure
  zbApsConfirmId_t *pConfirmId  IN: pass NULL if don't care about confirm

  Returns zbSuccess_c           if worked
  Returns zbBusy_c              if not enough memory to process request
  Returns gZbInvalidParameter_c if invalid parameter
*/
zbStatus_t AF_DataRequestNoCopy
  (
  afAddrInfo_t *pAddrInfo,
  uint8_t payloadLen,
  afToApsdeMessage_t *pMsg, 
  zbApsConfirmId_t *pConfirmId
  );

/*
  AF_DataRequestFragmented

  Only available if gFragmentedCapability_d property is enabled.

  Send a large (or small) array of bytes using the ZigBee fragmented method. The total 
  size than can be sent depends on the field gApsMaxFragmentLength_c settable through 
  ApsmeSetRequest(): a total of 256 fragments may be sent in a single data requst. If the 
  fragment size is 100, with would be 25,600 bytes. The packets are confirmed, with 3 
  retries.

  Use the optional confirmId to track multiple in-the-air data requests.

  Returns
  zbSuccess_c           If request created and sent. Confirm comes back to the application
                        through BeeAppDataConfirm().
  gZbNoMem_c            Not enough memory to create the request.
  gZbInvalidEndpoint_c  The source endpoint was never registered.
*/
zbStatus_t AF_ActualDataRequestFragmented
  (
  afAddrInfo_t *pAddrInfo,      /* IN: source and destination information */
  uint16_t payloadLen,          /* IN: length of payload */
  void *pPayload,               /* IN: payload (will be copied from here to a MSG) */
  zbApsConfirmId_t  *pConfirmId   /* IN: point to a confirm ID if needed, NULL if not */
  );

/*
  AF_DataRequestFragmentedNoCopy

  Only available if gFragmentedCapability_d property is enabled.

  Use AF_MsgAllocFragment() to create the linked list of fragments for use by this function.

  Send a large (or small) set of bytes using the ZigBee fragmented method. The total 
  size than can be sent depends on the field gApsMaxFragmentLength_c settable through 
  ApsmeSetRequest(): a total of 256 fragments may be sent in a single data requst. If the 
  fragment size is 100, with would be 25,600 bytes. The packets are confirmed, with 3 
  retries.

  Use the optional confirmId to track multiple in-the-air data requests.

  Returns
  zbSuccess_c           If request created and sent. Confirm comes back to the application 
                        through BeeAppDataConfirm().
  gZbNoMem_c            Not enough memory to create the request.
  gZbInvalidEndpoint_c  The source endpoint was never registered.
*/
zbStatus_t AF_ActualDataRequestFragmentedNoCopy
  (
  afAddrInfo_t *pAddrInfo,      /* IN: source and destination address information */
  afToApsdeMessage_t *pMsg,     /* IN: linked list of fragments starting with data request msg */
  zbApsConfirmId_t  *pConfirmId   /* IN: point to a coinfirm ID if needed */
  );

/* 
  AF_MsgAllocFragment
  
  Only available if gFragmentedCapability_d property is enabled.

  Allocate a single buffer for a fragmented message. Can allocate both the head of the 
  list (afToApsdeMessage_t) and subsequent fragments (zbTxFragmentedHdr_t).

  See example at top of this file.

*/
void *AF_ActualMsgAllocFragment
  (
  void *pHead,      /* IN: set to NULL for first fragment. Set to ptr to first allocated fragment */
                    /*     for other calls */
  uint8_t iFragLen, /* IN: may be up to ApsmeGetMaxAsduLength() for first fragment, */
                    /*     ApsmeGetMaxFragBufferSize() for all other calls*/
  uint8_t **ppData  /* IN/OUT: pointer returned where data should go (up to iFragLen) */
  );

/*
  Free the entire data indication, including all fragmented blocks (if any). Note: the input
  is note the pointer to the data indication, but ptr to the message itself that was DeQueued.
*/
void AF_ActualFreeDataIndicationMsg
  (
  apsdeToAfMessage_t * pApsDataIndMsg /* IN: data indication message */
  );

/*
  Free a linked list of buffers created by AF_MsgAllocFragment()
*/
void AF_ActualFreeDataRequestFragments
  (
  void *pHead     /* IN: head of linked list created by AF_MsgAllocFragment() */
  );

/*
  Returns total size of data indication. Only need if the packet is fragmented.
*/
uint16_t AF_ActualGetSizeOfIndication
  (
  zbApsdeDataIndication_t *pIndication  /* IN: data indication  */
  );

/*
  Is this data indication fragmented or not?
*/
bool_t AF_ActualIsFragmentedDataIndication
  (
  zbApsdeDataIndication_t *pIndication  /* IN: data indication  */
  );

#if (gFragmentationCapability_d)

/* fragmentation enabled, call real functions */
#define AF_DataRequestFragmented(pAddrInfo, payloadLen,pPayload,pConfirmId) \
        AF_ActualDataRequestFragmented(pAddrInfo, payloadLen,pPayload,pConfirmId)
#define AF_DataRequestFragmentedNoCopy(pAddrInfo, pMsg, pConfirmId) \
        AF_ActualDataRequestFragmentedNoCopy(pAddrInfo, pMsg, pConfirmId)
#define AF_MsgAllocFragment(pHead, iFragLen, ppData) \
        AF_ActualMsgAllocFragment(pHead, iFragLen, ppData)
#define AF_FreeDataRequestFragments(pMsg) \
        AF_ActualFreeDataRequestFragments(pMsg)
#define AF_FreeDataIndicationMsg(pMsg) \
        AF_ActualFreeDataIndicationMsg(pMsg)
#define AF_GetSizeOfIndication(pIndication) \
        AF_ActualGetSizeOfIndication(pIndication)
#define AF_IsFragmentedDataIndication(pIndication) \
        AF_ActualIsFragmentedDataIndication(pIndication)

#else
/* fragmentation disabled, stub the functions so they still work in code */
#define AF_DataRequestFragmented(pAddrInfo, payloadLen,pPayload,pConfirmId) gApsNotSupported_c
#define AF_DataRequestFragmentedNoCopy(pAddrInfo, pMsg, pConfirmId) gApsNotSupported_c
#define AF_MsgAllocFragment(pHead, iFragLen, ppData)  0
#define AF_FreeDataRequestFragments(pMsg) MSG_Free(pMsg)
#define AF_FreeDataIndicationMsg(pMsg) MSG_Free(pMsg)
#define AF_GetSizeOfIndication(pIndication) (pIndication)->asduLength
#define AF_IsFragmentedDataIndication(pIndication) FALSE
#endif

/*
  AF_PrepareForReply

  Prepares the address information for a reply to a data indication. Helper 
  function for replies to data indications. Cannot fail.
*/
void AF_PrepareForReply
  (
  afAddrInfo_t *pAddrInfo,              /* OUT: addr info for reply */
  zbApsdeDataIndication_t *pIndication  /* IN: data indication */
  );

/*
  AF_InterPanDataRequest

  Sends out an InterPan Data request out of the radio. Note only 1 request pending
  is supported.
  

  Returns
  zbSuccess_c           If request created and sent. Confirm comes back to 
                        the application through InterPan_APP_SapHandler().
  gZbNoMem_c            Not enough memory to create the request. Wait and try
                        again later.
  gZbBusy_c             Message can not be sent (1 request is already pending.
  gInvalidParameter_c   Invalid paramter supplied in address information
*/
zbStatus_t AF_InterPanDataRequest
  (
  InterPanAddrInfo_t *pAddrInfo,/* IN: source and destination information */
  uint8_t payloadLen,           /* IN: length of payload */
  void *pPayload,               /* IN: payload (will be copied from here to a MSG) */
  zbApsConfirmId_t  *pConfirmId   /* IN: point to a confirm ID if needed */
  );



/* Application Framework's main task */  
void TS_AfTask(event_t events); 

/* Application Framework init code */
void TS_AfTaskInit(void);


#endif /* _APP_AF_INTERFACE_H_ */

