#include "ringBuffer.h"

#define BUFFER_FAIL     0
#define BUFFER_SUCCESS  1

#define BUFFER_SIZE 16 // muss 2^n betragen (8, 16, 32, 64 ...)
#define BUFFER_MASK (BUFFER_SIZE-1) // Klammern auf keinen Fall vergessen

//
// Stellt 1 Byte in den Ringbuffer
//
// Returns:
//     BUFFER_FAIL       der Ringbuffer ist voll. Es kann kein weiteres Byte gespeichert werden
//     BUFFER_SUCCESS    das Byte wurde gespeichert
//
uint8_t ringBuffer::bufferIn(uint8_t byte)
{
    uint8_t next = ((buffer.writeIndex + 1) & BUFFER_MASK);

    if (buffer.readIndex == next)
        return BUFFER_FAIL; // voll

    buffer.data[buffer.writeIndex] = byte;
    // buffer.data[buffer.write & BUFFER_MASK] = byte; // absolut Sicher
    buffer.writeIndex = next;

    return BUFFER_SUCCESS;
}

//
// Holt 1 Byte aus dem Ringbuffer, sofern mindestens eines abholbereit ist
//
// Returns:
//     BUFFER_FAIL       der Ringbuffer ist leer. Es kann kein Byte geliefert werden.
//     BUFFER_SUCCESS    1 Byte wurde geliefert
//
uint8_t ringBuffer::bufferOut(uint8_t *pByte)
{
    if (buffer.readIndex == buffer.writeIndex)
        return BUFFER_FAIL;

    *pByte = buffer.data[buffer.readIndex];

    buffer.readIndex = (buffer.readIndex+1) & BUFFER_MASK;

    return BUFFER_SUCCESS;
}