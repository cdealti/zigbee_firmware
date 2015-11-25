/******************************************************************************
* HcProfile.h
*
* Types, definitions and prototypes for the ZigBee Health Care Profile.
*
* Copyright (c) 2009, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
* Documents used in this specification:
* [R1] - 075360r07ZB_AFG-PHHC-Profile.pdf
* [R2] - 075123r00ZB_AFG-ZigBee_Cluster_Library_Specification.pdf
******************************************************************************/
#ifndef _HCPROFILE_H
#define _HCPROFILE_H

/* header files needed by health care profile */
#include "EmbeddedTypes.h"
#include "AfApsInterface.h"
#include "ZclOptions.h"
#include "ZCL.h"
#include "ZCLGeneral.h"
#include "ZclHVAC.h"
#include "ZclProtocInterf.h"

/******************************************************************************
*******************************************************************************
* Public macros & type definitions
*******************************************************************************
******************************************************************************/

/* Health Care Profile - little endian (0x0108) */
#if gBigEndian_c
#define gHcProfileId_c	0x0801
#else
#define gHcProfileId_c	0x0108
#endif

#define gHcProfileIdb_c	0x08,0x01



/******************************************
	device definitions 
*******************************************/
 
typedef uint16_t hcDeviceId_t;

#if gBigEndian_c

/* Data Management */
#define gHcDcu_c                0x0000

/* Multifunction */
#define GenericMultifunctionHc_c    0x0f00


/* Disease Management */
#define gHcPulseOximeter_c 	    0x1004
#define gHcECG_c 	              0x1006
#define gHcBPMonitor_c 	        0x1007
#define gHcThermometer_c 	      0x1008
#define gHcWeightScale_c        0x100f
#define gHcGlucoseMeter_c	      0x1011
#define gHcINR_c                0x1012
#define gHcInsulinPump_c	      0x1013
#define gHcPeakFlowMonitor_c	  0x1015

/* Health and Fitness */
#define gHcCardioFitnessMonitor_c	    0x1029
#define gHcStrengthFitnessEquipment_c 0x102a
#define gHcPhysicalActivityMonitor_c  0x102b
#define gHcStepCounter_c              0x1068

/* Aging Independently */
#define gHcILAH_c             	    0x1047
#define gHcAdherenceMonitor_c     	0x1048
#define gHcFallSensor_c	            0x1075
#define gHcPersSensor_c	            0x1076
#define gHcSmokeSensor_c          	0x1077
#define gHcCOSensor_c              	0x1078
#define gHcWaterSensor_c          	0x1079
#define gHcGasSensor_c              0x107a
#define gHcMotionSensor_c       	  0x107b
#define gHcPropertyExitSensor_c 	  0x107c
#define gHcEnuresisSensor_c   	    0x107d
#define gHcContactClosureSensor_c	  0x107e
#define gHcUsageSensor_c          	0x107f
#define gHcSwitchUseSensor_c    	  0x1080
#define gHcDosageSensor_c           0x1081
#define gHcTemperatureSensor_c     	0x1082

#else /* #if gBigEndian_c */

/* Data Management */
#define gHcDcu_c                0x0000

/* Multifunction */
#define GenericMultifunctionHc_c    0x000f


/* Disease Management */
#define gHcPulseOximeter_c 	    0x0410
#define gHcECG_c 	              0x0610
#define gHcBPMonitor_c 	        0x0710
#define gHcThermometer_c 	      0x0810
#define gHcWeightScale_c        0x0f10
#define gHcGlucoseMeter_c	      0x1110
#define gHcINR_c                0x1210
#define gHcInsulinPump_c	      0x1310
#define gHcPeakFlowMonitor_c	  0x1510

/* Health and Fitness */
#define gHcCardioFitnessMonitor_c	    0x2910
#define gHcStrengthFitnessEquipment_c 0x2a10
#define gHcPhysicalActivityMonitor_c  0x2b10
#define gHcStepCounter_c              0x6810

/* Aging Independently */
#define gHcILAH_c             	    0x4710
#define gHcAdherenceMonitor_c     	0x4810
#define gHcFallSensor_c	            0x7510
#define gHcPersSensor_c	            0x7610
#define gHcSmokeSensor_c          	0x7710
#define gHcCOSensor_c              	0x7810
#define gHcWaterSensor_c          	0x7910
#define gHcGasSensor_c              0x7a10
#define gHcMotionSensor_c       	  0x7b10
#define gHcPropertyExitSensor_c 	  0x7c10
#define gHcEnuresisSensor_c   	    0x7d10
#define gHcContactClosureSensor_c	  0x7e10
#define gHcUsageSensor_c          	0x7f10
#define gHcSwitchUseSensor_c    	  0x8010
#define gHcDosageSensor_c           0x8110
#define gHcTemperatureSensor_c     	0x8210



#endif /* #if gBigEndian_c */


/******************************************
  HC location types (specified by ILAH)
*******************************************/
#define gHcLocationToilet_c                    0x49
#define gHcLocationToiletMain_c                0x4a
#define gHcLocationOutsideToilet_c             0x4b
#define gHcLocationShowerRoom_c                0x4c
#define gHcLocationStudy_c                     0x4d
#define gHcLocationFrontGarden_c               0x4e
#define gHcLocationBackGarden_c                0x4f
#define gHcLocationKettle_c                    0x50
#define gHcLocationTelevision_c                0x51
#define gHcLocationStove_c                     0x52
#define gHcLocationMicrowave_c                 0x53
#define gHcLocationToaster_c                   0x54
#define gHcLocationVacuum_c                    0x55
#define gHcLocationAppliance_c                 0x56
#define gHcLocationFrontDoor_c                 0x57
#define gHcLocationBackDoor_c                  0x58
#define gHcLocationFridgeDoor_c                0x59
#define gHcLocationMedicationCabinetDoor_c     0x60
#define gHcLocationWardrobeDoor_c              0x61
#define gHcLocationFrontCupboardDoor_c         0x62
#define gHcLocationOtherDoor_c                 0x63
#define gHcWaitingRoom_c                       0x64
#define gHcTriageRoom_c                        0x65
#define gHcDoctorOffice_c                      0x66
#define gHcPatientrivateRoom_c                 0x67
#define gHcConsultationRoom_c                  0x68
#define gHcNurseStation_c                      0x69
#define gHcWard_c                              0x6a
#define gHcCorridor_c                          0x6b
#define gHcOperatingTheatre_c                  0x6c
#define gHcDentalSurgeryRoom_c                 0x6d
#define gHcMedicalImagingRoom_c                0x6e
#define gHcDecontaminationRoom_c               0x6f

/******************************************
	HC Internal Types (implementation specific)
*******************************************/


/******************************************************************************
*******************************************************************************
* Public prototypes
*******************************************************************************
******************************************************************************/
#endif /* _HCPROFILE_H */

