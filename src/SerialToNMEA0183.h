#include <Arduino.h>
#include <functional>
#define MAX_NMEA0183_MSG_BUF_LEN 81  //According to NMEA 3.01. Can not contain multi message as in AIS

class SerialToNMEA0183 {
    
private:
  //Variables to keep track on received message
  std::function<void (char*)> _handler_func;
  size_t MsgCheckSumStartPos;
  char MsgInBuf[MAX_NMEA0183_MSG_BUF_LEN];
  size_t MsgInPos;
  bool MsgInStarted;

  Stream* serialIn;
  bool parseMessage();

public:
  SerialToNMEA0183(Stream* serialIn, std::function<void (char*)> handler_func);
  void loop();
};
