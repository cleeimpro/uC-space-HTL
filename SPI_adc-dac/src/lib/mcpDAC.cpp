//
// Created by Clemens Freudenthaler on 14.11.20.
//

#include "mcpDAC.h"

mcpDAC::mcpDAC(int cs_Pin) {
    _cPin = cs_Pin;
}

void mcpDAC::init(){
    DDRB |= (1 << PB3)|(1 << PB5)|(1 << _cPin);
    PORTB |= (1 << _cPin);
    SPCR = (1 << SPE) | (1<<SPIE) | (1<<MSTR) | (1<<SPR0);
}

void mcpDAC::setDAC(uint8_t resolution, uint16_t valueToSet, uint8_t channel, uint8_t gain, uint8_t shutdown){
    PORTB &= ~(1 << _cPin); //set CSpin
    switch (resolution) {
        case 12:
            spiSendByteMCPDAC(((valueToSet>>8)&0xff)
                              | ((channel-1) << 7)
                              | ((2-gain) << 5)
                              | ((1-shutdown) << 4)
            );
            spiSendByteMCPDAC( valueToSet & 0xff );
            break;
        case 10:
            spiSendByteMCPDAC(((valueToSet>>6)&0xff)
                              | ((channel-1) << 7)
                              | ((2-gain) << 5)
                              | ((1-shutdown) << 4)
            );
            spiSendByteMCPDAC( (valueToSet<<2) & 0xff );
            break;
        case 8:
            spiSendByteMCPDAC(((valueToSet>>4)&0xff)
                              | ((channel-1) << 7)
                              | ((2-gain) << 5)
                              | ((1-shutdown) << 4)
            );
            spiSendByteMCPDAC( (valueToSet<<4) & 0xff );
            break;
        default:
            break;
    }
    PORTB |= (1 << _cPin);
}

void mcpDAC::spiSendByteMCPDAC(uint8_t valueToSend){
    SPDR = valueToSend;
    while(!(SPSR & (1<<SPIF)));
    // TODO
}
