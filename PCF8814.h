//###########################################################################
//
// FILE:   PCF8814.h
//
// TITLE:  Device driver for the PCF8814 LCD controller. Adapted from
//         https://github.com/kashapovd/PCF8814-Nokia-1100-LCD-library.git
//
//###########################################################################

#ifndef PCF8814_H_
#define PCF8814_H_

#include <stdint.h>
#include <string.h>

#define PCF8814_WIDTH              (96)
#define PCF8814_HEIGHT             (65)

#define SETPIN(pin)                GPIO_WritePin(pin, 1) // Perhaps not as fast as GpioDataRegs.GPADAT.bit.GPIO13 = 0;
#define CLRPIN(pin)                GPIO_WritePin(pin, 0)
#define DC(pin, type)              GPIO_WritePin(pin, type)

#define SETRST(pin)                SETPIN(pin)
#define CLRRST(pin)                CLRPIN(pin)

#define SETMOSI(pin)               SETPIN(pin)
#define CLRMOSI(pin)               CLRPIN(pin)

#define SETCS(pin)                 SETPIN(pin)
#define CLRCS(pin)                 CLRPIN(pin)

#define SETCLK(pin)                SETPIN(pin)
#define CLRCLK(pin)                CLRPIN(pin)
#define TGLCLK(pin)                CLRCLK(pin);\
                                   SETCLK(pin)

#define BLACK                      (1)
#define WHITE                      (0)

#define PCF8814_DATA               (1)
#define PCF8814_CMD                (0)

#define PCF8814_SOFTRST            (0b11100010)

#define PCF8814_RAM_DATA_ORDER     (0b10101000)
    #define SWAP_ORDER             (0b00000000)

#define PCF8814_ADDRESSING_MODE    (0b10101010)
    #define VERTICAL               (0b00000001)
    #define HORIZONTAL             (0b00000000)

#define PCF8814_BIAS               (0b00110000)

#define PCF8814_TCOMPENSATION      (0b11101010)
    #define COMP_EN                (0b00000001)
    #define COMP_DIS               (0b00000000)

#define PCF8814_POWER_CONTROL      (0b00101000)
    #define CHARGE_PUMP_ON         (0b00000111)
    #define CHARGE_PUMP_OFF        (0b00000000)

// H - high word, L - low word
#define PCF8814_CHARGE_PUMP_H      (0b00111101)
    #define PUMP_L_x2              (0b00000000)
    #define PUMP_L_x3              (0b00000001)
    #define PUMP_L_x4              (0b00000010)
    #define PUMP_L_x5              (0b00000011)

#define PCF8814_FRAME_FREQ_H       (0b11101111)
    #define FREQ_L_H80             (0b00000000)
    #define FREQ_L_H70             (0b00000001)
    #define FREQ_L_H60             (0b00000010)
    #define FREQ_L_H40             (0b00000011)

#define NOP                        (0b11100011)
#define CONTROL_BYTE               (0b00000000)

#define PCF8814_INTERNAL_OSCILL    (0b00111010)

#define PCF8814_MIRRORY            (0b11000010)

#define PCF8814_DISPLAY_MODE       (0b10100000)
    #define ALLON                  (0b00000100)
    #define _NORMAL                (0b00000101)
    #define OFF                    (0b00001110)
    #define ON                     (0b00001111)
    #define INVERT                 (0b00000111)
    #define NORMAL                 (0b00000110)

#define PCF8814_YADDR              (0b10110000)
#define PCF8814_XADDR_L            (0b00000000)
#define PCF8814_XADDR_H            (0b00010000)

#define PCF8814_SYSTEM_BIAS        (0b00110000)
#define PCF8814_VOP_H              (0b00100000)
#define PCF8814_VOP_L              (0b10000000)



class PCF8814
{
public:
    PCF8814(const uint16_t sce,
            const uint16_t sclk,
            const uint16_t sdin,
            const uint16_t rst);
    virtual ~PCF8814();

    void init(void);
    void displayOn(void);
    void displayOff(void);
    void setContrast(const uint16_t value);
    void invertDisplay(bool state);
    void clearDisplay(void);
    void display(void);

    uint16_t *getBuffer(void) const;
    uint16_t getBufferSize(void) const;
    uint16_t getPixel(const uint16_t x, const uint16_t y) const;
    void pushBuffer(uint16_t *buffer, uint16_t size);

    void drawPixel(int16_t x, int16_t y,
                   uint16_t color);
    void drawRect(int16_t x, int16_t y,
                  int16_t w, int16_t h,
                  uint16_t color);
    void fillRect(int16_t x, int16_t y,
                  int16_t w, int16_t h,
                  uint16_t color);
    void drawSquare(int16_t x, int16_t y,
                    int16_t a, uint16_t color);
    void fillSquare(int16_t x, int16_t y,
                    int16_t a, uint16_t color);
    void fillScreen(int16_t color);


private:
    uint16_t _sce;
    uint16_t _sclk;
    uint16_t _sdin;
    uint16_t _rst;

    void hardReset(void);
    void spiWrite(uint16_t type, uint16_t byte);
    void setXY(const uint16_t x, const uint16_t y);

};

#endif /* PCF8814_H_ */
