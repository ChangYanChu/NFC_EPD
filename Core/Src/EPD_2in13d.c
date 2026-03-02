#if defined(EPD_2IN13D)
#include "EPD_2in13d.h"

/**
 * full screen update LUT
**/
static const unsigned char EPD_lut_vcomDC[] = {
    0x00, 0x08, 0x00, 0x00, 0x00, 0x02,
    0x60, 0x28, 0x28, 0x00, 0x00, 0x01,
    0x00, 0x14, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x12, 0x12, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00,
};
static const unsigned char EPD_lut_ww[] = {
    0x40, 0x08, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
    0x40, 0x14, 0x00, 0x00, 0x00, 0x01,
    0xA0, 0x12, 0x12, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static const unsigned char EPD_lut_bw[] = {
    0x40, 0x17, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x0F, 0x0F, 0x00, 0x00, 0x03,
    0x40, 0x0A, 0x01, 0x00, 0x00, 0x01,
    0xA0, 0x0E, 0x0E, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static const unsigned char EPD_lut_wb[] = {
    0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
    0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
    0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static const unsigned char EPD_lut_bb[] = {
    0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
    0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
    0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};


/**
 * partial screen update LUT
**/
static const unsigned char EPD_lut_vcom1[] = {
    0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    ,0x00, 0x00,
};
static const unsigned char EPD_lut_ww1[] = {
    0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static const unsigned char EPD_lut_bw1[] = {
    0x80, 0x19, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static const unsigned char EPD_lut_wb1[] = {
    0x40, 0x19, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static const unsigned char EPD_lut_bb1[] = {
    0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};


/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
void EPD_Reset(void)
{
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(10);
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
void EPD_ReadBusy(void)
{
    UBYTE busy;
    do {
        EPD_SendCommand(0x71);
        busy = DEV_Digital_Read(EPD_BUSY_PIN);
        busy =!(busy & 0x01);
    } while(busy);
    DEV_Delay_ms(200);
}

void EPD_ReadBusy_no_wait(void)
{
    UBYTE busy;
    do {
        EPD_SendCommand(0x71);
        busy = DEV_Digital_Read(EPD_BUSY_PIN);
        busy =!(busy & 0x01);
    } while(false);
    DEV_Delay_ms(200);
}

/******************************************************************************
function :	LUT download
parameter:
******************************************************************************/
static void EPD_SetFullReg(void)
{
		EPD_SendCommand(0X82);	
    EPD_SendData(0x00);	
    EPD_SendCommand(0X50);	
    EPD_SendData(0x97);	

    unsigned int count;
    EPD_SendCommand(0x20);
    for(count=0; count<44; count++) {
        EPD_SendData(EPD_lut_vcomDC[count]);
    }

    EPD_SendCommand(0x21);
    for(count=0; count<42; count++) {
        EPD_SendData(EPD_lut_ww[count]);
    }

    EPD_SendCommand(0x22);
    for(count=0; count<42; count++) {
        EPD_SendData(EPD_lut_bw[count]);
    }

    EPD_SendCommand(0x23);
    for(count=0; count<42; count++) {
        EPD_SendData(EPD_lut_wb[count]);
    }

    EPD_SendCommand(0x24);
    for(count=0; count<42; count++) {
        EPD_SendData(EPD_lut_bb[count]);
    }
}

/******************************************************************************
function :	LUT download
parameter:
******************************************************************************/
static void EPD_SetPartReg(void)
{
    EPD_SendCommand(0x82);			//vcom_DC setting
    EPD_SendData(0x00);
    EPD_SendCommand(0X50);
    EPD_SendData(0xb7);
	
    unsigned int count;
    EPD_SendCommand(0x20);
    for(count=0; count<44; count++) {
        EPD_SendData(EPD_lut_vcom1[count]);
    }

    EPD_SendCommand(0x21);
    for(count=0; count<42; count++) {
        EPD_SendData(EPD_lut_ww1[count]);
    }

    EPD_SendCommand(0x22);
    for(count=0; count<42; count++) {
        EPD_SendData(EPD_lut_bw1[count]);
    }

    EPD_SendCommand(0x23);
    for(count=0; count<42; count++) {
        EPD_SendData(EPD_lut_wb1[count]);
    }

    EPD_SendCommand(0x24);
    for(count=0; count<42; count++) {
        EPD_SendData(EPD_lut_bb1[count]);
    }
}

/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
void EPD_TurnOnDisplay(void)
{
    EPD_SendCommand(0x12);		 //DISPLAY REFRESH
    DEV_Delay_ms(10);     //!!!The delay here is necessary, 200uS at least!!!

    EPD_ReadBusy();
}

/******************************************************************************
function :	Initialize the e-Paper register
parameter:
******************************************************************************/
void EPD_Init()
{
    EPD_Reset();

    EPD_SendCommand(0x01);	//POWER SETTING
    EPD_SendData(0x03);
    EPD_SendData(0x00);
    EPD_SendData(0x2b);
    EPD_SendData(0x2b);
    EPD_SendData(0x03);

    EPD_SendCommand(0x06);	//boost soft start
    EPD_SendData(0x17);     //A
    EPD_SendData(0x17);     //B
    EPD_SendData(0x17);     //C

    EPD_SendCommand(0x04);
    EPD_ReadBusy_no_wait();

    EPD_SendCommand(0x00);	//panel setting
  //  EPD_SendData(0xbf);     //LUT from OTP，128x296
  //  EPD_SendData(0x0e);     //VCOM to 0V fast
    EPD_SendData(0x8f);


    EPD_SendCommand(0x30);	//PLL setting
    EPD_SendData(0x3a);     // 3a 100HZ   29 150Hz 39 200HZ	31 171HZ

    EPD_SendCommand(0x61);	//resolution setting
    EPD_SendData(EPD_WIDTH);
    EPD_SendData((EPD_HEIGHT >> 8) & 0xff);
    EPD_SendData(EPD_HEIGHT& 0xff);

    EPD_SendCommand(0x82);	//vcom_DC setting
    EPD_SendData(0x28);
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void EPD_Clear(void)
{
    UWORD Width, Height;
    Width = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    Height = EPD_HEIGHT;

    EPD_SendCommand(0x10);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(0x00);
        }
    }

    EPD_SendCommand(0x13);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(0xFF);
        }
    }

    EPD_SetFullReg();
    EPD_TurnOnDisplay();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void EPD_Display(const UBYTE *Image)
{
    UWORD Width, Height;
    Width = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    Height = EPD_HEIGHT;

    EPD_SendCommand(0x10);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(0x00);
        }
    }
    // Dev_Delay_ms(10);

    EPD_SendCommand(0x13);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(Image[i + j * Width]);
        }
    }
    // Dev_Delay_ms(10);

		EPD_SetFullReg();
    EPD_TurnOnDisplay();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void EPD_DisplayPart(const UBYTE *Image)
{
    /* Set partial Windows */
    EPD_SetPartReg();
    EPD_SendCommand(0x91);		//This command makes the display enter partial mode
    EPD_SendCommand(0x90);		//resolution setting
    EPD_SendData(0);           //x-start
    EPD_SendData(EPD_WIDTH - 1);       //x-end

    EPD_SendData(0);
    EPD_SendData(0);     //y-start
    EPD_SendData(EPD_HEIGHT / 256);
    EPD_SendData(EPD_HEIGHT % 256 - 1);  //y-end
    EPD_SendData(0x28);

    UWORD Width;
    Width = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    
    /* send data */
    EPD_SendCommand(0x10);
    for (UWORD j = 0; j < EPD_HEIGHT; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(~Image[i + j * Width]);
        }
    }

    EPD_SendCommand(0x13);
    for (UWORD j = 0; j < EPD_HEIGHT; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(Image[i + j * Width]);
        }
    }

    /* Set partial refresh */    
    EPD_TurnOnDisplay();
}


/******************************************************************************
function :	Enter sleep mode
parameter:
******************************************************************************/
void EPD_Sleep(void)
{
    EPD_SendCommand(0X50);
    EPD_SendData(0xf7);
    EPD_SendCommand(0X02);  	//power off
    EPD_SendCommand(0X07);  	//deep sleep
    EPD_SendData(0xA5);
}

void EPD_Start_Black(void){
//      UWORD Width, Height;
//    Width = (EPD_2IN13D_WIDTH % 8 == 0)? (EPD_2IN13D_WIDTH / 8 ): (EPD_2IN13D_WIDTH / 8 + 1);
//    Height = EPD_HEIGHT;
    /*
    EPD_SendCommand(0x10);
   
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(0x00);
        }
    }
    */
    // Dev_Delay_ms(10);

    EPD_SendCommand(0x13);
}


void EPD_MY_2IN13D_TurnOnDisplay(void){
    EPD_SetFullReg();
    EPD_SendCommand(0x12);		 //DISPLAY REFRESH
    DEV_Delay_ms(10);     //!!!The delay here is necessary, 200uS at least!!!
    //EPD_2IN13D_ReadBusy_no_wait();
}
#endif /* EPD_2IN13D */
