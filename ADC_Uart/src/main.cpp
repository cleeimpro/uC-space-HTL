#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#define ASCII_SHIFT 48

#define BAUD 9600UL
#define BAUD_PRESCALE (((F_CPU / (BAUD * 16UL))) - 1)
#define LEN(arr) ((int) (sizeof (arr) / sizeof (arr)[0]))

char out[] = "0x00\r";
volatile unsigned char cur_item = 0;
volatile unsigned char old_val = 0;
volatile unsigned char rec_item = 0;
volatile unsigned char rec_val = 0;

void init_ADC(){
    DDRC &= ~(1<<0);
    ADMUX = (1<<REFS0 | (0 << MUX0));   // Versorgung AVcc als Ref - Channel 0
    ADCSRA = (4<<ADPS0) | (1<<ADEN) | (1<<ADIE) | (1<<ADSC) | (1<<ADATE);
            // Prescaler // Enable // Interrupt Enable // Single Conversation
    ADCSRB = (3<<ADTS0);
            // ADC wandelt auf ausloesen von Timer0 Compare Match A
}

void init_timer(){
    // Timer 0 auf Compare Interrupt mit 10ms
    TCCR0A = (1 << WGM01); // CTC Modus
    TCCR0B |= (1 << CS00 | 1 << CS02); // Prescaler 1024
    // ((16000000/1024)/100) = 155.25
    OCR0A = 155;
    // Compare Interrupt erlauben
    TIMSK0 |= (1 << OCIE0A);

    // Timer 2 fuer PWM der LED
    // Clear OCR2A by compare | Mode Fast-PWM
    TCCR2A = (2 << COM2A0) | (3 << WGM20);
    // Prescaler clk/8
    TCCR2B = (2 << CS20);
}

void init_UART(){
    // Baudrate in das entsprechende UART-Register schreiben
    UBRR0H = ( BAUD_PRESCALE >> 8);
    UBRR0L = BAUD_PRESCALE;

    UCSR0B |= (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0) | (1 << TXCIE0);  // UART TX, RX und RX Interrupt einschalten
    UCSR0C = (3 << UCSZ00); // Asynchron 8N1
}

ISR(ADC_vect){
    unsigned char new_val = ADCW/4;
    if(new_val != old_val) {
        out[2] = (new_val >> 4) + ASCII_SHIFT + (new_val >> 4 > 9?7:0);
        out[3] = (new_val & 0x0f) + ASCII_SHIFT + ((new_val & 0x0f) > 9?7:0);
        UDR0 = out[cur_item++];
        old_val = new_val;
    }
}

/**
 * Interrupt nach fertigem Senden
 */
ISR(USART_TX_vect){
    if(cur_item < 5) {
        UDR0 = out[cur_item++];
    } else {
        cur_item = 0;
    }
}

/**
 * Interrupt beim Empfangen neuer Daten
 */
ISR(USART_RX_vect){
    unsigned char r = UDR0;
    if(r == 'x'){
        rec_item = 1;
    }else if(rec_item == 1){
        rec_val = (r - ASCII_SHIFT - ((r >= 'A') ? 7 : 0)) << 4;
        rec_item = 2;
    }else if(rec_item == 2){
        rec_val |= r - ASCII_SHIFT - (r >= 'A' ? 7 : 0);
        rec_item = 0;
        OCR2A = rec_val;
    }
}

ISR(TIMER0_COMPA_vect){
    ;
}

int main(){
    DDRB = 1<<3;
    DDRD = 1<<1;

    init_ADC();
    init_timer();
    init_UART();

    // Enable global interrupts
    sei();

    while(1)
        ;
}
