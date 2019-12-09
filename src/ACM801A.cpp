#include <ACM801A.h>

void ACM801A::begin() {
  // _serial.begin(9600);
  _serial.setTimeout(1000);
}

uint8_t ACM801A::Checksum(uint8_t *uBuff, uint8_t uBuffLen)  {
  uint8_t uSum = 0;
  for(uint8_t i = 0; i < uBuffLen; i++) {
    uSum = uSum + uBuff[i];
  }
  uSum = (~uSum) + 1;
  return uSum;
}

bool ACM801A::isTagPresent() {
  uint8_t read;
  _idStart = 0;
  _idLen = 0;
  _cardtype = CT_UNKNOWN;
  if (_serial.available()) {
    _data[0] = _serial.read();
    switch (_data[0]) {
      case CT_ACMG2:
        // ACM G2 tag - 17 bytes
        // 0  head sign
        // 1  user code
        // 2..14 ID number
        // 15 antenna
        // 16 0xFF
        read = _serial.readBytes(&(_data[1]), 16);
        if (16 != read) { return false; }
        if (Checksum(_data, 15) != _data[15]) { return false; }
        if (0xFF != _data[16]) { return false; }
        _idStart = 2;
        _idLen = 12;
        _cardtype = CT_ACMG2;
        return true;
        break;
      case CT_ISO18000:
        // ISO18000-6B - 14 bytes total
        read = _serial.readBytes(&(_data[1]), 13);
        if (13 != read) { return false; }
        if (Checksum(_data, 13) != _data[13]) { return false; }
        if (0x0C != _data[1]) { return false; }
        _idStart = 5;
        _idLen = 8;
        _cardtype = CT_ISO18000;
        return true;
        break;
      case CT_ACMG2CLOCK:
        // ACM G2 tag with clock function
        read = _serial.readBytes(&(_data[1]), 19);
        if (19 != read) { return false; }
        if (Checksum(_data, 19) != _data[19]) { return false; }
        _idStart = 2;
        _idLen = 12;
        _cardtype = CT_ACMG2CLOCK;
        return true;
        break;
      default:
        return false;
    }
  }
  return false;
}

void ACM801A::getID(char *output) {
  sprintHex(output, _data, _idStart, _idLen);
}

void ACM801A::sprintHex(char *output, uint8_t *arrayData, uint8_t arrayStart, uint8_t arrayLen) {
  uint8_t j = 0;
  uint8_t first;
  for (uint8_t i=arrayStart; i < (arrayStart + arrayLen); ++i) {
    first = (arrayData[i] >> 4) | 48;
    if (first > 57) {
      output[j] = first + (uint8_t)39;
    } else {
      output[j] = first;
    }
    ++j;
    first = (arrayData[i] & 0x0F) | 48;
    if (first > 57) {
      output[j] = first + (uint8_t)39;
    } else {
      output[j] = first;
    }
    ++j;
  }
  output[j] = 0;
}

void ACM801A::getReaderVersion() {
  _pktBuffer[0] = 0xA0;
  _pktBuffer[1] = 0x02; // Length
  _pktBuffer[2] = 0x6A;
  _pktBuffer[3] = Checksum(_pktBuffer, 3);
  _serial.write(_pktBuffer, 4); // Send identify command to reader
  _serial.readBytes(_pktBuffer, 6); // Read response
  if (_pktBuffer[5] != Checksum(_pktBuffer, 5)) { Serial.println("CRC Error"); return; }
  if (_pktBuffer[0] != 0xE0) { Serial.println("Unexpected response code"); return; }
  if (_pktBuffer[1] != 0x04) { Serial.println("Wrong response length byte"); return; }
  if (_pktBuffer[2] != 0x6A) { Serial.println("Wrong response cmd byte"); return; }
  char versionInfo[5];
  sprintHex(versionInfo, _pktBuffer, 3, 2);
  Serial.print("SW Version: ");
  Serial.println(versionInfo);
}

uint8_t ACM801A::getTransmitPower() {
  return getSingleSetting(0x65);
}

bool ACM801A::setTransmitPower(uint8_t value) {
  if (value > 150) return false;
  return setSingleSetting(0x65, value);
}

bool ACM801A::reset() {
  _pktBuffer[0] = 0xA0;
  _pktBuffer[1] = 0x02; // Packet length
  _pktBuffer[2] = 0x65; // Reader reset
  _pktBuffer[3] = Checksum(_pktBuffer, 3); // CRC
  _serial.write(_pktBuffer, 4); // Send command to reader
  _serial.readBytes(_pktBuffer, 5);
  if (_pktBuffer[4] != Checksum(_pktBuffer, 4)) { return false; }
  if (_pktBuffer[0] != 0xE4) { return false; }
  if (_pktBuffer[1] != 0x03) { return false; }
  if (_pktBuffer[2] != 0x65) { return false; }
  if (_pktBuffer[3] != 0x00) { return false; }
  return true;
}

uint8_t ACM801A::getSingleSetting(uint8_t settingAddress) {
  _pktBuffer[0] = 0xA0;
  _pktBuffer[1] = 0x04; // Packet length
  _pktBuffer[2] = 0x61; // Read single setting parameter
  _pktBuffer[3] = 0x00; // Setting parameter MSB
  _pktBuffer[4] = settingAddress; // Setting parameter LSB
  _pktBuffer[5] = Checksum(_pktBuffer, 5); // CRC
  _serial.write(_pktBuffer, 6); // Send command to reader
  _serial.readBytes(_pktBuffer, 7); // Read response
  if (_pktBuffer[6] != Checksum(_pktBuffer, 6)) { return 0; }
  if (_pktBuffer[0] != 0xE0) { return 0; }
  if (_pktBuffer[1] != 0x05) { return 0; }
  if (_pktBuffer[2] != 0x61) { return 0; }
  if (_pktBuffer[4] != settingAddress) { return 0; }
  return _pktBuffer[5];
}

bool ACM801A::setSingleSetting(uint8_t settingAddress, uint8_t settingValue) {
  _pktBuffer[0] = 0xA0;
  _pktBuffer[1] = 0x05; // Packet length
  _pktBuffer[2] = 0x60; // Write single setting parameter
  _pktBuffer[3] = 0x00; // Setting parameter MSB
  _pktBuffer[4] = settingAddress; // Setting parameter LSB
  _pktBuffer[5] = settingValue;
  _pktBuffer[6] = Checksum(_pktBuffer, 6); // CRC
  _serial.write(_pktBuffer, 7); // Send command to reader
  _serial.readBytes(_pktBuffer, 5);
  if (_pktBuffer[4] != Checksum(_pktBuffer, 4)) { return false; }
  if (_pktBuffer[0] != 0xE4) { return false; }
  if (_pktBuffer[1] != 0x03) { return false; }
  if (_pktBuffer[2] != 0x60) { return false; }
  if (_pktBuffer[3] != 0x00) { return false; }
  return true;
}
