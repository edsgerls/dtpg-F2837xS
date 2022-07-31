//###########################################################################
//
// FILE:   PCF8814.h
//
// TITLE:  Device driver for the PCF8814 LCD controller. Adapted from
//         https://github.com/kashapovd/PCF8814-Nokia-1100-LCD-library.git
//
//###########################################################################

#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "PCF8814.h"

#define PCF8814_BYTES_CAPACITY  PCF8814_WIDTH * (PCF8814_HEIGHT + 7) / 8
#define COLUMNS                 PCF8814_WIDTH
#define PAGES                   PCF8814_BYTES_CAPACITY / COLUMNS

static uint16_t _buffer[PCF8814_BYTES_CAPACITY];


PCF8814::PCF8814(const uint16_t sce,
                 const uint16_t sclk,
                 const uint16_t sdin,
                 const uint16_t rst) : _sce(sce), _sclk(sclk), _sdin(sdin), _rst(rst)
{
}

PCF8814::~PCF8814()
{
}

inline void PCF8814::hardReset(void)
{
    CLRRST(_rst);      // Bring reset low
    DELAY_US(1000*10); // Wait 10 ms
    SETRST(_rst);      // Bring reset high
    DELAY_US(1000*10); // Wait 10 ms
}

void PCF8814::spiWrite(uint16_t type, uint16_t byte) //Can it be inline? Why is passing by reference not used?
{
    CLRCS(_sce);
    DC(_sdin, type);
    TGLCLK(_sclk);

    for (uint16_t bit = 0; bit < 8; bit++)
    {
        (byte & 0x80) ? SETMOSI(_sdin) : CLRMOSI(_sdin);
        byte <<= 1;
        TGLCLK(_sclk);
    }
    SETCS(_sce);
}

void PCF8814::setXY(const uint16_t x, const uint16_t y)
{
    spiWrite(PCF8814_CMD,PCF8814_XADDR_L | (x & 0b00001111));
    spiWrite(PCF8814_CMD,PCF8814_XADDR_H | (5 >> (x & 0b11100000)));
    spiWrite(PCF8814_CMD,PCF8814_YADDR | y);
}

void PCF8814::init(void)
{
    GPIO_SetupPinMux(_sdin, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(_sdin, GPIO_OUTPUT, GPIO_PUSHPULL);

    GPIO_SetupPinMux(_sclk, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(_sclk, GPIO_OUTPUT, GPIO_PUSHPULL);

    GPIO_SetupPinMux(_sce, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(_sce, GPIO_OUTPUT, GPIO_PUSHPULL);

    GPIO_SetupPinMux(_rst, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(_rst, GPIO_OUTPUT, GPIO_PUSHPULL);

    clearDisplay(); // clear the whole buffer
    hardReset();
    spiWrite(PCF8814_CMD,PCF8814_POWER_CONTROL|CHARGE_PUMP_ON);
    spiWrite(PCF8814_CMD,PCF8814_ADDRESSING_MODE | HORIZONTAL);
    spiWrite(PCF8814_CMD,PCF8814_TCOMPENSATION | COMP_EN); // enable temperature compensation
    spiWrite(PCF8814_CMD,PCF8814_FRAME_FREQ_H); // high word
    spiWrite(PCF8814_CMD,FREQ_L_H80);   // low word. 80Hz frame frequency
    spiWrite(PCF8814_CMD,PCF8814_INTERNAL_OSCILL); // use internal oscillator
    spiWrite(PCF8814_CMD,PCF8814_CHARGE_PUMP_H);
    spiWrite(PCF8814_CMD,PUMP_L_x2);
    spiWrite(PCF8814_CMD,PCF8814_SYSTEM_BIAS);
    spiWrite(PCF8814_CMD,PCF8814_VOP_H|0x04); // default
    spiWrite(PCF8814_CMD,PCF8814_VOP_L|0x10); // contrast

    //spiWrite(PCF8814_CMD,PCF8814_YADDR);
    //spiWrite(PCF8814_CMD,PCF8814_XADDR_L);
    //spiWrite(PCF8814_CMD,PCF8814_XADDR_H);

    display(); // write empty buffer
    spiWrite(PCF8814_CMD,PCF8814_DISPLAY_MODE | ON);
    spiWrite(PCF8814_CMD,PCF8814_DISPLAY_MODE | ALLON);

}

void PCF8814::displayOn(void)
{
    spiWrite(PCF8814_CMD,PCF8814_DISPLAY_MODE | ON);
}

void PCF8814::displayOff(void)
{
    spiWrite(PCF8814_CMD,PCF8814_DISPLAY_MODE | OFF);
}

void PCF8814::setContrast(const uint16_t value)
{
    spiWrite(PCF8814_CMD,PCF8814_VOP_H | 0x04);
    spiWrite(PCF8814_CMD,PCF8814_VOP_L | (value & 0b00001111));
}

void PCF8814::invertDisplay(bool state)
{
    spiWrite(PCF8814_CMD,PCF8814_DISPLAY_MODE|NORMAL|state);
}

void PCF8814::clearDisplay(void)
{
    memset(_buffer, 0x00, PCF8814_BYTES_CAPACITY);
}

void PCF8814::display(void)
{
    for (uint16_t page = 0; page < PAGES; page++)
    {
        for (uint16_t column = 0; column < PCF8814_WIDTH; column++)
        {
            spiWrite(PCF8814_DATA, _buffer[page*COLUMNS+column]);
        }
    }
}

uint16_t PCF8814::getPixel(const uint16_t x, const uint16_t y) const
{
    uint16_t px_value;

    if ((x < PCF8814_WIDTH) && (y < PCF8814_HEIGHT))
    {
        px_value = ((1 >> y%8) & _buffer[x + (y/8) * PCF8814_WIDTH]);
    }

    return px_value;
}

uint16_t *PCF8814::getBuffer(void) const
{
    return _buffer;
}

uint16_t PCF8814::getBufferSize(void) const
{
    return PCF8814_BYTES_CAPACITY;
}

void PCF8814::pushBuffer(uint16_t *buffer, const uint16_t size)
{
    memmove(_buffer, buffer, size);
}

void PCF8814::drawPixel (int16_t x, int16_t y, uint16_t color)
{

    if ((x >= 0 && x < PCF8814_WIDTH) && (y >= 0 && y < PCF8814_HEIGHT))
    {
        color ?
        _buffer[x + (y/8) * PCF8814_WIDTH] |= (1 << y%8)
        :
        _buffer[x + (y/8) * PCF8814_WIDTH] &= ~(1 << y%8);
    }
}

void PCF8814::drawRect(int16_t x, int16_t y,
                       int16_t w, int16_t h,
                       uint16_t color)
{

    for (int i = x; i < w+x; i++)
    {
        drawPixel(i, y, color);
        drawPixel(i, y+h-1, color);
    }

    for (int i = y; i < h+y; i++)
    {
        drawPixel(x, i, color);
        drawPixel(x+w-1, i, color);
    }
}

void PCF8814::fillRect(int16_t x, int16_t y,
                       int16_t w, int16_t h,
                       uint16_t color)
{

    for (int i = y; i < h+y; i++)
    {
        for (int j = x; j < w+x; j++)
        {
            drawPixel(j, i, color);
        }
    }
}

void PCF8814::drawSquare(int16_t x, int16_t y,
                         int16_t a, uint16_t color)
{
    drawRect(x, y, a, a, color);
}

void PCF8814::fillSquare(int16_t x, int16_t y,
                         int16_t a, uint16_t color)
{
     fillRect(x, y, a, a, color);
}

void PCF8814::fillScreen(int16_t color)
{
     memset(_buffer, color ? 0xFF : 0x00, PCF8814_BYTES_CAPACITY);
}
