#if defined(EPD_WF0213T1PCZ04)
#include "EPD_WF0213T1PCZ04.h"

const uint8_t bw2grey[] = {0x00,0x03,0x0C,0x0F,0x30,0x33,0x3C,0x3F,0xC0,0xC3,0xCC,0xCF,0xF0,0XF3,0xFC,0xFF};
const uint8_t lut_20_vcom0[]  = {0x0E,0x14,0x01,0x0A,0x06,0x04,0x0A,0x0A,0x0F,0x03,0x03,0x0C,0x06,0x0A,0x00};
const uint8_t lut_21_w[]  = {0x0E,0x14,0x01,0x0A,0x46,0x04,0x8A,0x4A,0x0F,0x83,0x43,0x0C,0x86,0x0A,0x04};
const uint8_t lut_22_b[]  = {0x0E,0x14,0x01,0x8A,0x06,0x04,0x8A,0x4A,0x0F,0x83,0x43,0x0C,0x06,0x4A,0x04};
const uint8_t lut_23_g1[]  = {0x8E,0x94,0x01,0x8A,0x06,0x04,0x8A,0x4A,0x0F,0x83,0x43,0x0C,0x06,0x0A,0x04};
const uint8_t lut_24_g2[]  = {0x8E,0x94,0x01,0x8A,0x86,0x04,0x8A,0x4A,0x0F,0x83,0x43,0x0C,0x86,0x0A,0x04};

const uint8_t lut_25_vcom1[]  = {0x03,0x1D,0x00,0x01,0x08,0x00,0x37,0x37,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t lut_26_red0[]  = {0x83,0x5D,0x00,0x81,0x48,0x00,0x77,0x77,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t lut_27_red1[]  = {0x03,0x1D,0x00,0x01,0x08,0x00,0x37,0x37,0x00,0x00,0x00,0x00,0x00,0x00,0x00};


void EPD_Reset(void)
{
    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(100);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(100);
	
	  DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(10);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(10);
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
void EPD_ReadBusy()
{
    UBYTE busy;
    do {
        DEV_Delay_ms(1);
        busy = DEV_Digital_Read(EPD_BUSY_PIN);
        busy =!(busy & 0x01);
    } while(busy);
}

void EPD_ReadBusy_ME(uint8_t time)
{
	/*
    UBYTE busy;
		uint8_t time_cnt = 0;
    do {
        DEV_Delay_ms(1);
        busy = DEV_Digital_Read(EPD_BUSY_PIN);
        busy =!(busy & 0x01);
        time_cnt += 1;
    } while(busy && (time_cnt < time));
		*/
	DEV_Delay_ms(20);
}



void WriteMultiData(const uint8_t *p, uint8_t length)
{
    for(uint8_t count=0; count < length; count++) {
        EPD_SendData(p[count]);
    }
}

/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
void EPD_TurnOnDisplay(void)
{
    EPD_SendCommand(0x12); //display refresh
    EPD_ReadBusy_ME(10);
}


void EPD_Init()
{
  EPD_Reset();
  EPD_SendCommand(0x01);
  EPD_SendData(0x07);
  EPD_SendData(0x00);
  EPD_SendData(0x08);
  EPD_SendData(0x00);
  EPD_SendCommand(0x06);
  EPD_SendData(0x07);
  EPD_SendData(0x07);
  EPD_SendData(0x07);
	
  EPD_SendCommand(0x04);
	DEV_Delay_ms(20);
	
  EPD_SendCommand(0x00);
  EPD_SendData(0xcf);
  EPD_SendCommand(0x50);
  EPD_SendData(0x37);
  EPD_SendCommand(0x30);
  EPD_SendData(0x39);
  EPD_SendCommand(0x61);
  EPD_SendData(EPD_WIDTH);
  EPD_SendData(0x00);
  EPD_SendData(EPD_HEIGHT);
  EPD_SendCommand(0x82);
  EPD_SendData(0x0E);

  EPD_SendCommand(0x20);
  WriteMultiData(lut_20_vcom0,15);
  EPD_SendCommand(0x21);
  WriteMultiData(lut_21_w,15);
  EPD_SendCommand(0x22);
  WriteMultiData(lut_22_b, 15);
  EPD_SendCommand(0x23);
  WriteMultiData(lut_23_g1, 15);
  EPD_SendCommand(0x24);
  WriteMultiData(lut_24_g2, 15);
  EPD_SendCommand(0x25);
  WriteMultiData(lut_25_vcom1, 15);
  EPD_SendCommand(0x26);
  WriteMultiData(lut_26_red0, 15);
  EPD_SendCommand(0x27);
  WriteMultiData(lut_27_red1, 15);
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void EPD_Display(const UBYTE *Image)
{

}

void EPD_Start_Black(void){

    EPD_SendCommand(0x10);
}

void EPD_write64(uint8_t *data){
		uint8_t data128[128];
		for(uint8_t count=0; count < 64; count++) {
        data128[count*2] = bw2grey[(*data & 0xF0) >> 4]; 
				data128[count*2+1] = bw2grey[(*data & 0x0F) >> 4];
				data ++;
    }
		DEV_Digital_Write(EPD_DC_PIN, 1);
		DEV_Digital_Write(EPD_CS_PIN, 0);
		DEV_SPI_Write_nByte(data128, 128);
		DEV_Digital_Write(EPD_CS_PIN, 1);
}
#endif /* EPD_WF0213T1PCZ04 */
