#include "SerialToNMEA0183.h"

SerialToNMEA0183::SerialToNMEA0183(Stream* serialIn) {
  this->serialIn = serialIn;
}

char* SerialToNMEA0183::getMessage() {
  return MsgInBuf;
}

bool SerialToNMEA0183::parseMessage() {
  
  bool result=false;
  while (serialIn->available() > 0 && !result) {
    int NewByte=serialIn->read();
      if (NewByte=='$' || NewByte=='!') { // Message start
        MsgInStarted=true;
        MsgInPos=0;
        MsgInBuf[MsgInPos]=NewByte;
        MsgInPos++;
      } else if (MsgInStarted) {
        MsgInBuf[MsgInPos]=NewByte;
        if (NewByte=='*') MsgCheckSumStartPos=MsgInPos;
        MsgInPos++;
        if (MsgCheckSumStartPos!=SIZE_MAX and MsgCheckSumStartPos+3==MsgInPos) { // We have full checksum and so full message
            MsgInBuf[MsgInPos]=0; // add null termination
            result=true;
            MsgInStarted=false;
            MsgInPos=0;
            MsgCheckSumStartPos=SIZE_MAX;  
        }
        if (MsgInPos>=MAX_NMEA0183_MSG_BUF_LEN) { // Too may chars in message. Start from beginning
          MsgInStarted=false;
          MsgInPos=0;
          MsgCheckSumStartPos=SIZE_MAX;  
        }
      }
  }  
  return result;
}