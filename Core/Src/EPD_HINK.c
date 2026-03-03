/*****************************************************************************
* | File      	:   EPD_2in9b_V3.c
* | Author      :   Waveshare team
* | Function    :   2.9inch e-paper b V3
* | Info        :
*----------------
* |	This version:   V1.1
* | Date        :   2020-12-03
* | Info        :
* -----------------------------------------------------------------------------
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "EPD_HINK.h"

/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
void EPD_Reset(void)
{
    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(10);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(10);
    EPD_ReadBusy();  

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
    uint32_t timeout = 30000; // 最多等待 30 秒（BWR全刷新约 15-25秒）
    while(timeout--)
    {
        UBYTE busy = DEV_Digital_Read(EPD_BUSY_PIN);
        if((busy & 0x01) == 0) return; // BUSY=0 表示空闲
        DEV_Delay_ms(1);
    }
    // 超时也返回，避免死锁
}

/******************************************************************************
function :	Initialize the e-Paper register
parameter:
******************************************************************************/
void EPD_Init(void)
{
    EPD_Reset();

    EPD_SendCommand(0x12);  //SWRESET
    EPD_ReadBusy();   
        
    EPD_SendCommand(0x01); //Driver output control      
    EPD_SendData((EPD_HEIGHT-1)%256);    
    EPD_SendData((EPD_HEIGHT-1)/256);
    EPD_SendData(0x00);

    EPD_SendCommand(0x11); //data entry mode       
    EPD_SendData(0x01);

    EPD_SendCommand(0x44); //set Ram-X address start/end position   
    EPD_SendData(0x00);
    EPD_SendData(EPD_WIDTH/8-1);   

    EPD_SendCommand(0x45); //set Ram-Y address start/end position          
    EPD_SendData((EPD_HEIGHT-1)%256);    
    EPD_SendData((EPD_HEIGHT-1)/256);
    EPD_SendData(0x00);
    EPD_SendData(0x00); 

    EPD_SendCommand(0x3C); //BorderWavefrom
    EPD_SendData(0x05);    

    EPD_SendCommand(0x21); //  Display update control
    EPD_SendData(0x00);        
  EPD_SendData(0x80);  
    
  EPD_SendCommand(0x18); //Read built-in temperature sensor
    EPD_SendData(0x80);    

    EPD_SendCommand(0x4E);   // set RAM x address count to 0;
    EPD_SendData(0x00);
    EPD_SendCommand(0x4F);   // set RAM y address count to 0X199;    
    EPD_SendData((EPD_HEIGHT-1)%256);    
    EPD_SendData((EPD_HEIGHT-1)/256);
  EPD_ReadBusy();
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void EPD_Clear(void)
{
    UWORD Width = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    UWORD Height = EPD_HEIGHT;

    //send black data
    EPD_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(0xFF);
        }
    }

    //send red data
    EPD_SendCommand(0x26);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(0xFF);
        }
    }
    
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void EPD_Display(const UBYTE *blackimage, const UBYTE *ryimage)
{
    UWORD Width, Height;
    Width = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    Height = EPD_HEIGHT;

	//send black data
    EPD_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(blackimage[i + j * Width]);
        }
    }
    
	//send red data
    EPD_SendCommand(0x26);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(ryimage[i + j * Width]);
        }
    }
    EPD_TurnOnDisplay();
}

/******************************************************************************
function :	Enter sleep mode
parameter:
******************************************************************************/
void EPD_Sleep(void)
{
    EPD_SendCommand(0x10); // POWER_OFF
    EPD_SendCommand(0x01); // DEEP_SLEEP
}

void EPD_TurnOnDisplay(){
    EPD_SendCommand(0x22); //Display Update Control
    EPD_SendData(0xF7);   
    EPD_SendCommand(0x20); //Activate Display Update Sequence
    EPD_ReadBusy();
}

void EPD_Start_Red(void){
    EPD_SendCommand(0x26);
}

void EPD_Start_Black(void){
    EPD_SendCommand(0x24);
}
