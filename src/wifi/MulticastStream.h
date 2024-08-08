#ifndef _MULTICAST_STREAM_H_
#define _MULTICAST_STREAM_H_

#include <Stream.h>

/**
 * Stream implementation which writes to two backing streams.
 * Use case is logging to both serial console and web console.
 * This stream cannot be read from.
 */
class MulticastStream : public Stream {

  private:
    Stream *stream1;
    Stream *stream2;

  public:
    MulticastStream(Stream *stream1, Stream *stream2) : Stream() {
        this->stream1 = stream1;
        this->stream2 = stream2;
    }

    int available() {
        return 0;
    }

    int read() {
        return -1;
    }

    int peek() {
        return -1;
    }

    void flush() {
    }

    size_t write(uint8_t value) {
        stream1->write(value);
        stream2->write(value);
        return 1;
    }

    size_t write(const uint8_t *buffer, size_t size) {
        stream1->write(buffer,size);
        stream2->write(buffer,size);
        return size;
    }
};
#endif