#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#define BAUD 9600UL
#define BAUD_PRESCALE (((F_CPU / (BAUD * 16UL))) - 1)
#define LEN(arr) ((int) (sizeof (arr) / sizeof (arr)[0]))

#define LED_PIN (1<<PB5)
#define LED_DDR DDRB
#define LED_PORT PORTB

/**
 * Der Arduino Uno soll alle empfangenen seriellen Daten
 * wieder an das Ursprungsgeraet zurueckschicken.
 *
 * PinConf Arduino:
 * PD0: RX
 * PD1: TX
 */

void print(char s[]){
    for (int i = 0; i< strlen(s); i++) {
        while (!(UCSR0A & (1 << UDRE0)));  // warten bis UDR0 leer ist und Senden moeglich
        UDR0 = s[i]; //
    }
}

void println(char s[]){
    print(s);
    while (!(UCSR0A & (1 << UDRE0)));  // warten bis UDR0 leer ist und Senden moeglich
    UDR0 = '\r';
}

/**
Der Compare Interrupt Handler
wird aufgerufen, wenn
TCNT0 = OCR0A = 16-1
alle 10ms
*/
ISR (TIMER0_COMPA_vect){
    static unsigned int millisekunden = 0;
    millisekunden += 10;
    if (millisekunden == 1000){
        LED_PORT ^= LED_PIN;
        millisekunden = 0;
    }
}

/**
 * Interrupt Service Routine
 * wird aufgerufen wenn des Speicherregister UDR0 voll mit
 * empfangenen Daten ist.
 */

ISR(USART_RX_vect){
    static char input[8];
    static unsigned char pos_input = 0;
    // echo
    char res_data = UDR0; // Empfangene Daten auslesen
    while (!(UCSR0A & (1<<UDRE0)));  // warten bis UDR0 leer ist und Senden moeglich
    UDR0 = res_data;
    if(res_data == '\r'){
        // Compare Interrupt ausschalten
        TIMSK0 &= ~(1 << OCIE0A);
        if (!strcmp(input, "on")){
            LED_PORT |= LED_PIN;
            println("LED on");
        } else if (!strcmp(input, "off")) {
            LED_PORT &= ~LED_PIN;
            println("LED off");
        } else if (!strcmp(input, "toggle")){
            LED_PORT ^= LED_PIN;
            println("LED toggled");
        } else if (!strcmp(input, "blink")){
            // Compare Interrupt erlauben
            TIMSK0 |= (1 << OCIE0A);
            println("LED blinked");
        } else {
            print("unknown command: ");
            println(input);
        }
        print("> ");
        pos_input = 0;
        for(int i = 0; (i-1)<LEN(input); i++){
            input[i] = NULL;
        }
    } else if (res_data >= 33 && res_data <= 126){
        input[pos_input] = res_data;
        pos_input = (pos_input + 1) % LEN(input);
    }
}

int main(){
    LED_DDR |= LED_PIN;
    LED_PORT &= ~LED_PIN;

    // Baudrate in das entsprechende UART-Register schreiben
    UBRR0H = ( BAUD_PRESCALE >> 8);
    UBRR0L = BAUD_PRESCALE;

    UCSR0B |= (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);  // UART TX, RX und RX Interrupt einschalten
    UCSR0C = (3 << UCSZ00); // Asynchron 8N1

    // Timer
    // Timer 0 konfigurieren
    TCCR0A = (1 << WGM01); // CTC Modus
    TCCR0B |= (1 << CS00 | 1 << CS02); // Prescaler 1024
    // ((16000000/1024)/1000) = 15.6
    OCR0A = 16 - 1;

    sei(); // Enable Global Interrupts

    print("> ");

    while(1);
}
