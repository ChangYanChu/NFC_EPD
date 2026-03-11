/*****************************************************************************
* | File        :   EPD_SSD1680.c
* | Author      :   
* | Function    :   SSD1680 2.9inch e-paper (128x296, Black/White/Red)
* | Info        :
*                Ported from manufacturer SPI example (STM32L053C8)
*                SSD1680 controller, supports B/W and B/W/R modes
*----------------
* | Date        :   2026-03-02
* | Info        :
*   Command reference (SSD1680):
*     0x12 - SW Reset
*     0x01 - Driver Output Control
*     0x11 - Data Entry Mode Setting
*     0x44 - Set RAM X Address Start/End Position
*     0x45 - Set RAM Y Address Start/End Position
*     0x3C - Border Waveform Control
*     0x21 - Display Update Control 1
*     0x18 - Temperature Sensor Control
*     0x4E - Set RAM X Address Counter
*     0x4F - Set RAM Y Address Counter
*     0x24 - Write RAM (Black/White)
*     0x26 - Write RAM (Red)
*     0x22 - Display Update Control 2
*     0x20 - Activate Display Update Sequence
*     0x10 - Deep Sleep Mode
******************************************************************************/
#if defined(EPD_SSD1680)
#include "EPD_SSD1680.h"
static void EPD_ResetRAMPointer(void);

/******************************************************************************
function :  Software reset
parameter:
******************************************************************************/
void EPD_Reset(void)
{
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(20);
    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(20);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(20);
}

/******************************************************************************
function :  send command
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
function :  send data
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
function :  Wait until the busy_pin goes LOW (idle)
parameter:
    SSD1680 BUSY pin: LOW = idle, HIGH = busy
******************************************************************************/
void EPD_ReadBusy(void)
{
    UDOUBLE timeout = 0;
    while (1)
    {
        if (DEV_Digital_Read(EPD_BUSY_PIN) == 0)
            break;
        DEV_Delay_ms(1);
        timeout++;
        if (timeout > 5000)  // 5s timeout, avoid dead-lock on NFC power loss
            break;
    }
}

/******************************************************************************
function :  Initialize the e-Paper register (SSD1680)
parameter:
    Resolution: 128 x 296
    Data entry mode: Y decrement, X increment
******************************************************************************/
void EPD_Init(void)
{
    EPD_Reset();

    EPD_ReadBusy();
    EPD_SendCommand(0x12);  // SW Reset
    EPD_ReadBusy();

    EPD_SendCommand(0x01);  // Driver Output Control
    EPD_SendData((EPD_HEIGHT - 1) % 256);   // 0x27 = 295 low byte
    EPD_SendData((EPD_HEIGHT - 1) / 256);   // 0x01 = 295 high byte
    EPD_SendData(0x01);                     // GD=0, SM=0, TB=1

    EPD_SendCommand(0x11);  // Data Entry Mode Setting
    EPD_SendData(0x01);     // Y decrement, X increment

    EPD_SendCommand(0x44);  // Set RAM X Address Start/End Position
    EPD_SendData(0x00);                     // X start = 0
    EPD_SendData(EPD_WIDTH / 8 - 1);       // X end = 15 -> (15+1)*8 = 128

    EPD_SendCommand(0x45);  // Set RAM Y Address Start/End Position
    EPD_SendData((EPD_HEIGHT - 1) % 256);   // Y start = 295 low byte
    EPD_SendData((EPD_HEIGHT - 1) / 256);   // Y start = 295 high byte
    EPD_SendData(0x00);                     // Y end = 0 low byte
    EPD_SendData(0x00);                     // Y end = 0 high byte

    EPD_SendCommand(0x3C);  // Border Waveform Control
    EPD_SendData(0x05);

    EPD_SendCommand(0x21);  // Display Update Control 1
    EPD_SendData(0x00);
    EPD_SendData(0x80);     // Source output mode: available source from S8 to S167

    EPD_SendCommand(0x18);  // Temperature Sensor Control
    EPD_SendData(0x80);     // Use internal temperature sensor

    EPD_SendCommand(0x4E);  // Set RAM X Address Counter
    EPD_SendData(0x00);     // Initial X counter = 0

    EPD_SendCommand(0x4F);  // Set RAM Y Address Counter
    EPD_SendData((EPD_HEIGHT - 1) % 256);   // Initial Y counter = 295 low byte
    EPD_SendData((EPD_HEIGHT - 1) / 256);   // Initial Y counter = 295 high byte

    EPD_ReadBusy();
}

/******************************************************************************
function :  Clear screen (fill all white)
parameter:
******************************************************************************/
void EPD_Clear(void)
{
    UWORD Width = (EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1);
    UWORD Height = EPD_HEIGHT;

    // Send black/white data (0xFF = white)
    EPD_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(0xFF);
        }
    }

    // Send red data (0xFF = no red)
    EPD_SendCommand(0x26);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(0x00);
        }
    }

    EPD_TurnOnDisplay();
}

/******************************************************************************
function :  Sends the image buffer in RAM to e-Paper and displays
parameter:
    blackimage : black/white image data buffer
    ryimage    : red/yellow image data buffer (NULL to skip)
******************************************************************************/
void EPD_Display(const UBYTE *blackimage, const UBYTE *ryimage)
{
    UWORD Width, Height;
    Width = (EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1);
    Height = EPD_HEIGHT;

    // Send black/white data
    EPD_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(blackimage[i + j * Width]);
        }
    }

    // Send red data
    if (ryimage != 0) {
        EPD_SendCommand(0x26);
        for (UWORD j = 0; j < Height; j++) {
            for (UWORD i = 0; i < Width; i++) {
                EPD_SendData(ryimage[i + j * Width]);
            }
        }
    }

    EPD_TurnOnDisplay();
}

/******************************************************************************
function :  Enter sleep mode (Deep Sleep Mode 1)
parameter:
******************************************************************************/
void EPD_Sleep(void)
{
    EPD_SendCommand(0x10);  // Deep Sleep Mode
    EPD_SendData(0x01);     // Enter Deep Sleep Mode 1
}

/******************************************************************************
function :  Activate Display Update Sequence (refresh)
parameter:
******************************************************************************/
void EPD_TurnOnDisplay(void)
{
    EPD_SendCommand(0x22);  // Display Update Control 2
    EPD_SendData(0xF7);     // Load temperature, Load LUT, Display, Disable clock/analog
    EPD_SendCommand(0x20);  // Activate Display Update Sequence
    EPD_ReadBusy();
    DEV_Delay_ms(100);
}

/******************************************************************************
function :  Start writing Red channel RAM data
parameter:
******************************************************************************/
void EPD_Start_Red(void)
{
    EPD_ResetRAMPointer();
    EPD_SendCommand(0x26);
}

/******************************************************************************
function :  Start writing Black/White channel RAM data
parameter:
******************************************************************************/
void EPD_Start_Black(void)
{
    EPD_ResetRAMPointer();
    EPD_SendCommand(0x24);
}

/******************************************************************************
function :  Reset RAM address counters to initial position
parameter:
******************************************************************************/
static void EPD_ResetRAMPointer(void)
{
    EPD_SendCommand(0x4E);
    EPD_SendData(0x00);
    EPD_SendCommand(0x4F);
    EPD_SendData((EPD_HEIGHT - 1) % 256);
    EPD_SendData((EPD_HEIGHT - 1) / 256);
}

/******************************************************************************
function :  Display "Test" text on screen to verify driver & circuit
parameter:
    Renders 8x16 font directly to EPD RAM without framebuffer.
    Font format: column-first, 16 bytes/char.
      bytes[0..7]  = columns 0-7 for rows 0-7  (bit0=row0 .. bit7=row7)
      bytes[8..15] = columns 0-7 for rows 8-15 (bit0=row8 .. bit7=row15)
******************************************************************************/
void EPD_ShowTest(void)
{
    /* 8x16 font data for 'T','e','s','t' (from standard ASCII 1608 font) */
    static const UBYTE font[4][16] = {
        {0x18,0x08,0x08,0xF8,0x08,0x08,0x18,0x00,
         0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00}, /* T */
        {0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,
         0x00,0x1F,0x22,0x22,0x22,0x22,0x13,0x00}, /* e */
        {0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,
         0x00,0x33,0x24,0x24,0x24,0x24,0x19,0x00}, /* s */
        {0x00,0x80,0x80,0xE0,0x80,0x80,0x00,0x00,
         0x00,0x00,0x00,0x1F,0x20,0x20,0x00,0x00}, /* t */
    };

    UWORD row_bytes  = EPD_WIDTH / 8;            /* 16 */
    UWORD txt_y0     = (EPD_HEIGHT - 16) / 2;    /* 140  vertical center   */
    UWORD txt_x0     = (row_bytes - 4) / 2;      /* 6    horizontal center */

    /* --- Write Black/White RAM (0x24) --- */
    EPD_ResetRAMPointer();
    EPD_SendCommand(0x24);

    for (UWORD r = 0; r < EPD_HEIGHT; r++) {
        for (UWORD c = 0; c < row_bytes; c++) {
            UBYTE px = 0xFF;                     /* white */

            if (r >= txt_y0 && r < txt_y0 + 16 &&
                c >= txt_x0 && c < txt_x0 + 4) {
                UBYTE tr   = (UBYTE)(r - txt_y0); /* text row 0-15 */
                UBYTE ci   = (UBYTE)(c - txt_x0); /* char index 0-3 */
                UBYTE half = (tr < 8) ? 0 : 8;
                UBYTE bit  = tr & 7;

                px = 0xFF;
                for (UBYTE col = 0; col < 8; col++) {
                    if (font[ci][half + col] & (1 << bit))
                        px &= ~(0x80 >> col);    /* black pixel */
                }
            }
            EPD_SendData(px);
        }
    }

    /* --- Write Red RAM (0x26) – all clear --- */
    EPD_ResetRAMPointer();
    EPD_SendCommand(0x26);
    for (UWORD r = 0; r < EPD_HEIGHT; r++) {
        for (UWORD c = 0; c < row_bytes; c++) {
            EPD_SendData(0x00);
        }
    }

    /* Refresh display */
    EPD_TurnOnDisplay();

    /* Leave counters ready for next operation */
    EPD_ResetRAMPointer();
}
#endif /* EPD_SSD1680 */
