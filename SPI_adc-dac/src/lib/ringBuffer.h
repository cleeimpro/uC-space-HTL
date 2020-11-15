#ifndef RINGBUFFERTYPE_H_
#define RINGBUFFERTYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#define BUFFER_FAIL     0
#define BUFFER_SUCCESS  1

#define BUFFER_SIZE 16 // muss 2^n betragen (8, 16, 32, 64 ...)
#define BUFFER_MASK (BUFFER_SIZE-1) // Klammern auf keinen Fall vergessen


class ringBuffer{
public:

    ringBuffer();
    struct Buffer {
        uint8_t data[BUFFER_SIZE];
        uint8_t readIndex; // zeigt auf das Feld mit dem ältesten Inhalt
        uint8_t writeIndex; // zeigt immer auf leeres Feld
    } buffer = {{}, 0, 0};
    //
    // Stellt 1 Byte in den Ringbuffer
    //
    // Returns:
    //     BUFFER_FAIL       der Ringbuffer ist voll. Es kann kein weiteres Byte gespeichert werden
    //     BUFFER_SUCCESS    das Byte wurde gespeichert
    //
    uint8_t bufferIn(uint8_t byte);
    //
    // Holt 1 Byte aus dem Ringbuffer, sofern mindestens eines abholbereit ist
    //
    // Returns:
    //     BUFFER_FAIL       der Ringbuffer ist leer. Es kann kein Byte geliefert werden.
    //     BUFFER_SUCCESS    1 Byte wurde geliefert
    //
    uint8_t bufferOut(uint8_t *pByte);
};

#ifdef __cplusplus
}
#endif

#endif /* RINGBUFFERTYPE_H_ */