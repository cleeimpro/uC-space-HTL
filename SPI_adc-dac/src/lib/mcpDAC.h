#ifndef mcpDAC_h
#define mcpDAC_h

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "ringBuffer.h"

//
// Created by Clemens Freudenthaler on 14.11.20.
//

class mcpDAC{
public:

    mcpDAC(int cs_Pin);

    /**********************************************
    Public Function: init

    Purpose: Initialise SPI interface

    Return Value: none
    **********************************************/
    void init();

    void setDAC(uint8_t resolution, uint16_t valueToSet, uint8_t channel, uint8_t gain, uint8_t shutdown);

    uint8_t _cPin;



private:
    void spiSendByteMCPDAC(uint8_t valueToSend);

};

#ifdef __cplusplus
}
#endif

#endif /* mcpDAC_hpp */