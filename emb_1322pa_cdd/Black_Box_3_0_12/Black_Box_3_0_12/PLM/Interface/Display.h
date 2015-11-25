/************************************************************************************
* This header file is for LCD Driver Interface.
*
*
* (c) Copyright 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
************************************************************************************/
#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "EmbeddedTypes.h"
#include "AppToPlatformConfig.h"
#include "LCD_Font.h"

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/
#if (gTargetMC1322xNCB == TRUE)  
  #define gLCDSupported_d FALSE
#else
  #define gLCDSupported_d FALSE
#endif
/* number of defined fonts */
#define gLCDNumFonts_c 2

/* gLCDGpioPinMode_c = 1 --> GPIO pins are used */
/* gLCDGpioPinMode_c = 0 --> GPIO ports are used */
#define gLCDGpioPinMode_c 0

#define gLCDMaxColumns_c         128
#define gLCDMaxLines_c            64
#define gLCDMaxRamDataPages_c      8

/* for backward compatibility functions */
#define gMAX_LCD_CHARS_c 16

/* Graphic LCD Commands */
/* command for selecting 1/7 bias */
#define gLCD17BiasCommand_c    0xA3
/* command for selecting 1/9 bias */
#define gLCD19BiasCommand_c    0xA2
/* command for setting ADC to normal */
#define gLCDADCNormalCommand_c        0xA0
/* command for setting ADC to reverse */
#define gLCDADCReverseCommand_c        0xA1
/* command for setting COM to normal */
#define gLCDComNormalCommand_c        0xC0
/* command for setting COM to reverse */
#define gLCDComReverseCommand_c       0xC8
/* command for setting display to normal */
#define gLCDDisplayNormalCommand_c   0xA6
/* command for setting display to reverse */
#define gLCDDisplayReverseCommand_c   0xA7
/* default command for setting the V5 voltage regulator internal resistor ratio */
#define gLCDDefaultRatioCommand_c   0x20
/* command for preparing to set the Electronic Volume */
#define gLCDElectronicVolumeModeSetCommand_c 0x81
/* command for preparing to set the Electronic Volume */
#define gLCDElectronicVolumeInitCommand_c 0x30
/* command for setting the power supply functions (by default all functions are disabled) */
#define gLCDPowerControllerDefaultCommand_c 0x28
/* command for setting all points off (by default all points are off) */
#define gLCDDisplayAllPointsOffCommand_c 0xA4
#define gLCDDisplayAllPointsDefaultCommand_c 0xA4
/* command for setting all points on (by default all points are off) */
#define gLCDDisplayAllPointsOnCommand_c 0xA5
/* command for turning off the display */
#define gLCDDisplayDefaultCommand_c 0xAE
/* command for turning on the display */
#define gLCDDisplayOnCommand_c 0xAF
/* command for turning off the static indicator */
#define gLCDStaticIndicatorDefaultCommand_c 0xAC
/* command for setting the page address to page 0 */
#define gLCDPageAddressDefaultCommand_c 0xB0
/* command for setting the first nibble of column address (0)*/
#define gLCDNibble0ColumnAddressDefaultCommand_c 0x10
/* command for setting the second nibble of column address (0)*/
#define gLCDNibble1ColumnAddressDefaultCommand_c 0x00
/* command for setting the display line to 0*/
#define gLCDDisplayStartLineDefaultCommand_c 0x40
/* command for resetting the display */
#define gLCDResetCommand_c 0xE2
/* command for read/modify/write */
#define gLCDRMWCommand_c 0xE0
/* command for read/modify/write end*/
#define gLCDEndCommand_c 0xEE


/* Rb/Ra ratio must have a value between 0 and 7 */
#define gLCDRatioValue_c    4
/* if electronic volume is enabled the brightness must take a value between 0 and 63 */
/* the electronic vlume is disabled when the brightness value is 0x20 */
#define gLCDDefaultBrightnessValue_c        0x0F

/* Events that are passed to the LCD task. 
   Are defined as byte masks to make possible 
   send multiple events to the task */
#define gLCDOpEvt_c          (1 << 0)


/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

typedef enum {
    gLCD_HexFormat_c,
    gLCD_DecFormat_c
}LCD_t;

typedef enum lcdFontType_tag
{
  gLCD_NormalFont_c = 0,
  gLCD_VerdanaFont_c,
  gLCD_MaxFont_c = gLCDNumFonts_c
}lcdFontType_t;

typedef enum lcdError_tag
{
  gLCD_NoError_c = 0,
  gLCD_QueueFull_c
}lcdError_t;

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

extern const font_t aLCDFont[gLCDNumFonts_c];

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
#if gLCDSupported_d

/******************************************************************************
* This function initializes the display
*
* Interface assumptions:
*
*
* Return value:
* None
*
******************************************************************************/
extern void LCD_Init(void);

/******************************************************************************
* This function clears the display. In case an error occured during add to queue
* operation, the clear display operation will not execute.
*
* Interface assumptions:
*
*
* Return value:
* None
*
******************************************************************************/
extern void LCD_ClearDisplay(void);

/******************************************************************************
* This function writes a string to the display
*
* Interface assumptions:
*
*
* Return value:
* None
*
*
******************************************************************************/
extern void LCD_WriteString(
  uint8_t line,  /* IN: Line in display */
  uint8_t *pstr	 /* IN: Pointer to text string */
);

/******************************************************************************
* This function writes a string and a value in decimal or hexdecimal
* to the display
*
* Interface assumptions:
*
*
* Return value:
* None
*
*
******************************************************************************/
extern void LCD_WriteStringValue
  (
  uint8_t *pstr, 		 /* IN: Pointer to text string */
  uint16_t value, 	 /* IN: Value */
  uint8_t line, 		 /* IN: Line in display. */ 
  LCD_t numberFormat /* IN: Value to show in HEX or DEC */
  );

/******************************************************************************
* This functions allows to write raw bytes to the LCD, the maximum number of bytes
*	capable per line is 8, this functions transfors every hex simbol in a byte to a char.
*
* Interface assumptions:
*
*
* Return value:
* None
*
*
******************************************************************************/
extern void LCD_WriteBytes
(
	uint8_t   *pstr,   /* IN: The pointer to the label to print with the bytes. */
	uint8_t   *value,  /* IN: The bytes to print. */
	uint8_t   line,    /* IN: The line in the LCD where the bytes with the label. */
	uint8_t   length   /* IN: The number of bytes to print in the LCD. */
);

/******************************************************************************
* This function sends a command to the display controller
*
* Interface assumptions:
*
*
* Return value:
* None
*
*
******************************************************************************/
extern void LCD_WriteCommand(uint8_t command);

/******************************************************************************
* This function sends a data to the display controller
*
* Interface assumptions:
*
*
* Return value:
* None
*
*
******************************************************************************/
extern void LCD_WriteData(uint8_t data);

/******************************************************************************
* This function sets the font received as parameter as default font
*
* Interface assumptions:
*
*
* Return value:
*      TRUE:  when the font is correctly set
*      FALSE: when an unsupported font is received as parameter
* char
*
*
******************************************************************************/
extern bool_t LCD_SetFont(lcdFontType_t font);

/******************************************************************************
* This function checks if an error occured during an interface function call.
* User must always call this function to verify if an interface function executed 
* successufully.
*
* Interface assumptions:
*
*
* Return value:
*      0x01:  an error occured during an interface function call 
*      0x00:  no error
* char
*
*
******************************************************************************/
extern lcdError_t LCD_CheckError(void);

/******************************************************************************
* This function writes a bitmap to a line of the display. A bitmap is made up of symbols,
* which are 8 pixels in height and 8 pixels in width. 
* In a bitmap, a symbol requires 8 bytes of storage, one for each pixel column.
* Symbols are provided to the function as indexes in a user defined symbol table.
* Index i points to bytes [8*i .. 8*i+7] in the symbol table.
*
* Interface assumptions:
*      pstr - is an array of symbol indexes to be represented on "line"
*      pstr, bitmap pointers different from NULL
*
*
* Return value:
* None
*
*
******************************************************************************/
extern void LCD_WriteBitmap(                      
  uint8_t *pstr,	  /* IN: Pointer to symbol index list to display */
  uint8_t len,      /* IN: Length in bytes of the symbol index list */
  uint8_t line,     /* IN: Line in display */
  uint8_t *bitmap 	/* IN: Pointer to symbol table */    
);

#else

  #define LCD_ClearDisplay()
  #define LCD_Init()
#ifdef __IAR_SYSTEMS_ICC__
  #define LCD_WriteString(line, pstr) ((void)pstr)
#else
  #define LCD_WriteString(line, pstr)
#endif
  #define LCD_WriteBitmap(pstr, len, line, bitmap)
  #define LCD_WriteStringValue(pstr, value, line, display)
  #define LCD_WriteBytes(pstr, value, line, length)
  #define LCD_WriteCommand(command)
  #define LCD_WriteData(data)
  #define LCD_SetFont(font) TRUE
  #define LCD_CheckError()  FALSE

#endif /* gLCDSupported_d */


#endif /* _DISPLAY_H_ */
