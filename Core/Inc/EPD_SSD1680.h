/*****************************************************************************
* | File        :   EPD_SSD1680.h
* | Author      :   
* | Function    :   SSD1680 2.9inch e-paper (128x296, Black/White/Red)
* | Info        :
*                Ported from manufacturer SPI example (STM32L053C8)
*                SSD1680 controller, supports B/W and B/W/R modes
*----------------
* | Date        :   2026-03-02
* | Info        :
******************************************************************************/
#ifndef __EPD_SSD1680_H_
#define __EPD_SSD1680_H_

#include "DEV_Config.h"

// Display resolution
#define EPD_WIDTH       128
#define EPD_HEIGHT      296

// SSD1680 manufacturer example only uses B/W channel (0x24).
// Define NO_RED so main.c uses 4736-byte threshold instead of 9472.
#define NO_RED

void EPD_Init(void);
void EPD_Clear(void);
void EPD_Display(const UBYTE *blackimage, const UBYTE *ryimage);
void EPD_Sleep(void);

void EPD_SendData(UBYTE Data);
void EPD_SendCommand(UBYTE Reg);
void EPD_Reset(void);
void EPD_ReadBusy(void);

void EPD_Start_Red(void);
void EPD_Start_Black(void);
void EPD_TurnOnDisplay(void);
void EPD_ShowTest(void);

#endif
