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
}

void loop() {
  if (rfid.isTagPresent()) {
    rfid.getID(tagID);
    Serial.println(tagID);
  }
}
