
#if defined(EPD_2IN13BC)
#include "EPD_2in13bc.h"

/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
void EPD_Reset(void)
{
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(200);
    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(2);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(200);
}

/******************************************************************************
function :	send command
parameter:
     Reg : Command register
******************************************************************************/
void EPD_SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Reg);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
void EPD_SendData(UBYTE Data)
{
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Data);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	Wait until the busy_pin goes LOW
parameter:
******************************************************************************/
void EPD_ReadBusy(void)
{
   // while(DEV_Digital_Read(EPD_BUSY_PIN) == 0) {
        DEV_Delay_ms(100);
    //}
}

/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
void EPD_TurnOnDisplay(void)
{
    EPD_SendCommand(0x12);		 //DISPLAY REFRESH
    DEV_Delay_ms(10);

    EPD_ReadBusy();
}

/******************************************************************************
function :	Initialize the e-Paper register
parameter:
******************************************************************************/
void EPD_Init(void)
{
    EPD_Reset();

    EPD_SendCommand(0x06); // BOOSTER_SOFT_START
    EPD_SendData(0x17);
    EPD_SendData(0x17);
    EPD_SendData(0x17);
	
    EPD_SendCommand(0x04); // POWER_ON
    EPD_ReadBusy();
	
    EPD_SendCommand(0x00); // PANEL_SETTING
    EPD_SendData(0x8F);
	
    EPD_SendCommand(0x50); // VCOM_AND_DATA_INTERVAL_SETTING
    EPD_SendData(0xF0);
    EPD_SendCommand(0x61); // RESOLUTION_SETTING
    EPD_SendData(EPD_2IN13BC_WIDTH); // width: 104
    EPD_SendData(EPD_2IN13BC_HEIGHT >> 8); // height: 212
    EPD_SendData(EPD_2IN13BC_HEIGHT & 0xFF);
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void EPD_Clear(void)
{
    UWORD Width = (EPD_2IN13BC_WIDTH % 8 == 0)? (EPD_2IN13BC_WIDTH / 8 ): (EPD_2IN13BC_WIDTH / 8 + 1);
    UWORD Height = EPD_HEIGHT;
    
    //send black data
    EPD_SendCommand(0x10);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(0xFF);
        }
    }
    EPD_SendCommand(0x92); 

    //send red data
    EPD_SendCommand(0x13);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(0xFF);
        }
    }
    EPD_SendCommand(0x92); 
    
    EPD_TurnOnDisplay();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void EPD_Display(const UBYTE *blackimage, const UBYTE *ryimage)
{
    UWORD Width, Height;
    Width = (EPD_2IN13BC_WIDTH % 8 == 0)? (EPD_2IN13BC_WIDTH / 8 ): (EPD_2IN13BC_WIDTH / 8 + 1);
    Height = EPD_HEIGHT;
    

    
    EPD_SendCommand(0x13);
    //EPD_2IN13BC_SendCommand(0x10);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(ryimage[i + j * Width]);
        }
    }
    //EPD_2IN13BC_SendCommand(0x92); 
    
    EPD_TurnOnDisplay();
}

/******************************************************************************
function :	Enter sleep mode
parameter:
******************************************************************************/
void EPD_Sleep(void)
{
    EPD_SendCommand(0x02); // POWER_OFF
    EPD_ReadBusy();
    EPD_SendCommand(0x07); // DEEP_SLEEP
    EPD_SendData(0xA5); // check code
}

void EPD_Start_Black(void){
  EPD_SendCommand(0x13);
}
#endif /* EPD_2IN13BC */