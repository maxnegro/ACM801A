#ifndef Template_h
#define Template_h

// If you want to support Arduino versions older than 1.0 you need to do this if
#if ARDUINO >= 100
// If you just want to support Arduino 1.0 or newer you just need to #include "Arduino.h"
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class ACM801A {
public:
  ACM801A(Stream& serial): _serial(serial) {}

  enum CardType {
    CT_UNKNOWN = -1,
    CT_ACMG2 = 0x00,
    CT_ISO18000 = 0xE0,
    CT_ACMG2CLOCK = 0xFF
  };

  void begin();

  uint8_t Checksum(uint8_t *uBuff, uint8_t uBuffLen);

  bool isTagPresent();

  void getID(char *output);
  void getReaderVersion();
  uint8_t getSingleSetting(uint8_t settingAddress);
  bool setSingleSetting(uint8_t settingAddress, uint8_t settingValue);
  uint8_t getTransmitPower();
  bool setTransmitPower(uint8_t value);
  bool reset();

private:
  Stream& _serial;
  CardType _cardtype = CT_UNKNOWN;
  uint8_t _pktType = 0;
  uint8_t _idStart = 0;
  uint8_t _idLen = 0;
  uint8_t _data[40];

  uint8_t _pktBuffer[40];

  void sprintHex(char *output, uint8_t *arrayData, uint8_t arrayStart, uint8_t arrayLen);
};

#endif
