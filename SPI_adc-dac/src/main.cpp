#include <avr/interrupt.h>
#include <util/delay.h>

#define DDR_SPI DDRD
#define DD_MOSI 3
#define DD_SCK  6


init_SPI(){
    /* Set MOSI and SCK output, all others input */
    DDR_SPI = (1<<DD_MOSI) | (1 << DD_SCK);
    /* Enable SPI, SPI-Interrupt, Master, set clock rate fck/16 */
    SPCR = (1 << SPE) | (1 << SPIE) | (1 << MSTR) | (1 << SPR0);
}

init_timer0(){
    Jede ms
    TCCR0A = (1 << WGM01); // CTC Modus
    TCCR0B |= (1 << CS00 | 1 << CS02); // Prescaler 1024
    // ((16000000/1024)/1000) = 15.6
    OCR0A = 16 - 1;
    // Compare Interrupt erlauben
    TIMSK0 |= (1 << OCIE0A);
}

/*ISR (TIMER0_COMPA_vect){
    SPDR = 255
}*/

void main(){
   // init_timer0();
    init_SPI();

    sei();

    unsigned int valueToSet = 4095;
    while(1){
        SPDR = ((valueToSet>>8)&0xff)
               | (0 << 7) // Channel 1
               | (0 << 5) // Gain 2
               | (0 << 4)); // Chip is on
               while(!(SPSR & (1<<SPIF)));
        SPDR = valueToSet & 0xff;
        while(!(SPSR & (1<<SPIF)));
    }
}