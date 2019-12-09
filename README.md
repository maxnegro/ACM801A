# ACM801A
An Arduino Open Source library for communicating with ACM801A long range UHF rfid reader.

At the moment only a subset of full communication protocol is implemented. In
particular you can read and set reader configuration parameters and decode tag
identities in timed reading mode (continuous scan).

Reader must be connected to Arduino via RS232<->TTL hardware (ie MAX232). Default
reader baud rate is 9600 (and this is good for Arduino UNO boards where control
is done via SoftwareSerial).

I have not tested or implemented RS485 and Wiegand protocol for reading tag 
identities.
