#ifndef __EPD_WF0213T1PCZ04_H_
#define __EPD_WF0213T1PCZ04_H_

#include "DEV_Config.h"

// Display resolution
#define EPD_WIDTH   104
#define EPD_HEIGHT  212

void EPD_Init(void);

void EPD_Display(const UBYTE *Image);

void EPD_SendData(UBYTE Data);
void EPD_SendCommand(UBYTE Reg);
void EPD_Reset(void);
void EPD_ReadBusy_ME(uint8_t time);
void EPD_ReadBusy(void);
void EPD_Start_Black(void);
void EPD_TurnOnDisplay(void);
void WriteMultiData(const uint8_t *lut, uint8_t length);
void EPD_write64(uint8_t *data);
#define NO_RED
#endif
