#ifndef _UDP_STREAM_H_
#define _UDP_STREAM_H_

#include <Stream.h>
#include <WiFiUdp.h>

class UdpStream : public Stream {

  private:
    WiFiUDP* _udp;

  public:
    UdpStream(WiFiUDP* udp) : Stream() {
      _udp = udp;
    }
  ~UdpStream() {

  }
  int available() {
    return _udp->available() ? _udp->available() : _udp->parsePacket();
  }

  int read() {    
      return _udp->read();
  }

  int peek() {
       return _udp->peek();
  }

  void flush() {
      return _udp->flush();
  }

  size_t write(uint8_t) {
      //Not implemented, because not needed for reading from Stream.
      return 0;
  }
};

#endif