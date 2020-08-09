#ifndef _CONSOLE_STREAM_H_
#define _CONSOLE_STREAM_H_

#include <Stream.h>
#include <WebSocketsServer.h>

/**
 * A Stream implementation for writing to a websocket.
 * This stream is only for writing. It cannot be read from.
 * All writes are broadcasted to all connected websocket clients.
 * Use cases is forwarding logging to webconsole.
 */
class WebSocketStream : public Stream {

  private:
    WebSocketsServer *webSocketServer;

  public:
    WebSocketStream(WebSocketsServer *webSocketServer) : Stream() {
        this->webSocketServer = webSocketServer;
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
        uint8_t values[1];
        values[0] = value;
        webSocketServer->broadcastTXT(values,1);
        return 1;
    }

    size_t write(const uint8_t *buffer, size_t size) {
        webSocketServer->broadcastTXT(buffer,size);
        return size;
    }
};
#endif