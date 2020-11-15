#include <avr/interrupt.h>
#include <avr/io.h>
#include "lib/mcpDAC.h"

mcpDAC mcp(PB2);

void init_timer(){
    // Jede ms
    TCCR0A = (1 << WGM01); // CTC Modus
    TCCR0B |= (1 << CS00 | 1 << CS02); // Prescaler 1024
    // ((16000000/1024)/1000) = 15.6
    OCR0A = 16 - 1;
    // Compare Interrupt erlauben
    TIMSK0 |= (1 << OCIE0A);
}

ISR (TIMER0_COMPA_vect){
    static uint16_t ms = 0;
    mcp.setDAC(12, ms++, 1, 2, 0);
    if(ms > 4095)
        ms = 0;
}


int main(){
    init_timer();
    mcp.init();
    //Beide ausgaenge ausschalten
    mcp.setDAC(12, 4060, 2, 2, 1);
    mcp.setDAC(12, 4060, 1, 2, 1);
    sei();

    while(1){
        ;
    }


}