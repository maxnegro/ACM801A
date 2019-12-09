#include <SoftwareSerial.h>
#include <ACM801A.h>

class RfidNotify {

};

SoftwareSerial acmSerial(4,3);
ACM801A rfid(acmSerial);

char tagID[30] = "";

void setup() {
  Serial.begin(115200);
  acmSerial.begin(9600);
  rfid.begin();
  rfid.getReaderVersion();
  if (rfid.setTransmitPower(50)) { // Transmit power range is 0..150
    if (rfid.reset()) {
      Serial.print("Transmit power: ");
      Serial.println(rfid.getTransmitPower());
    } else {
      Serial.println("Error resetting reader");
    }
  } else {
    Serial.println("Error setting transmit power");
  }
  Serial.print("Operating mode: ");
  Serial.println(rfid.getSingleSetting(0x70));
}

void loop() {
  if (rfid.isTagPresent()) {
    rfid.getID(tagID);
    Serial.println(tagID);
  }
}
