#include <avr/interrupt.h>
#include <avr/io.h>
#include <inttypes.h>

#define CS_ADC PB0
#define CS_DAC PB1

#define ADC_CHANNEL 0

#define IDLE 0
#define ADC_3 1
#define ADC_2 2
#define DAC_HI 3
#define DAC_LO 4
#define CS_RESET 5

volatile uint8_t state = IDLE;
volatile uint16_t spiVal = 0, old_spiVal = 0;

void init_timer(){
    // Jede ms
    TCCR0A = (1 << WGM01); // CTC Modus
    TCCR0B |= (3 << CS00); // Prescaler 64
    // ((16000000/64)/1000) = 8
    OCR0A = 8;
    // Compare Interrupt erlauben
    TIMSK0 |= (1 << OCIE0A);
}

/**
 * Timer0 Compare Interrupt wird jede 1ms aufgerufen
 * auf ADC wird geschrieben/gelesen
 * auf DAC geschrieben
 */
ISR (TIMER0_COMPA_vect){
    PORTB &= ~(1 << CS_ADC); //set CSpin
    SPDR = 0b01100000 | ((ADC_CHANNEL & 0b111) << 2);
    state = ADC_2;
}

ISR (SPI_STC_vect){
    switch (state) {
        case ADC_2:
            SPDR = 0;
            state = ADC_3;
            break;
        case ADC_3:
            spiVal = SPDR << 4;
            SPDR = 0;
            state = DAC_HI;
            break;
        case DAC_HI:
            spiVal |= SPDR >> 4;
            PORTB |= (1 << CS_ADC);
            state = IDLE;
            if(old_spiVal != spiVal) {
                old_spiVal = spiVal;
                PORTB &= ~(1 << CS_DAC);
                SPDR = ((spiVal>>8)&0xff) | (1 << 4);
                state = DAC_LO;
            }
            break;
        case DAC_LO:
            SPDR = (spiVal & 0xFF);
            state = CS_RESET;
            break;
        case CS_RESET:
            PORTB |= (1 << CS_DAC);
            state = IDLE;
            break;
        default: break;
    }
}

int main(){
    // ADC und DAC initialisieren
    DDRB |= (1 << PB3) | (1 << PB5) | (1 << PB2) | (1 << CS_ADC) | (1 << CS_DAC);
    SPCR |= (1 << SPE) | (1 << SPIE) | (1 << MSTR);
    PORTB |= (1 << CS_ADC) | (1 << CS_DAC);

    // Timer0 initialisieren
    init_timer();

    // Enable Global Interrupts
    sei();

    while(1);
}