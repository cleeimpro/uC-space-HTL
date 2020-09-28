#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define BAUD 9600UL
#define BAUD_PRESCALE (((F_CPU / (BAUD * 16UL))) - 1)

/**
 * Der Arduino Uno soll alle empfangenen seriellen Daten
 * wieder an das Ursprungsgeraet zurueckschicken.
 *
 * PinConf Arduino:
 * PD0: RX
 * PD1: TX
 */

/**
 * Interrupt Service Routine
 * wird aufgerufen wenn des Speicherregister UDR0 voll mit
 * empfangenen Daten ist.
 */
ISR(USART_RX_vect){
    if(UCSR0A&(1<<RXC0)) { // Check ob wirklich was empfangen wurde
        char resData = UDR0; // Empfangene Daten auslesen
        while (!(UCSR0A & (1<<UDRE0)));  // warten bis UDR0 leer ist und Senden moeglich
        UDR0 = resData; //
    }
}

int main(){
    DDRD |= (1 << 1); // Data Direktory Register für den TX-pin

    // Baudrate in das entsprechende UART-Register schreiben
    UBRR0H = ( BAUD_PRESCALE >> 8);
    UBRR0L = BAUD_PRESCALE;

    UCSR0B |= (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);  // UART TX, RX und RX Interrupt einschalten
    UCSR0C = (3 << UCSZ00); // Asynchron 8N1

    sei(); // Enable Global Interrupts

    while(1){
        _delay_ms(1000);
    }
}

