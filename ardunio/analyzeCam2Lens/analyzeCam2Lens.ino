/*
  Software serial multiple serial test

 Receives from the two software serial ports,
 sends to the hardware serial port.

 In order to listen on a software port, you call port.listen().
 When using two software serial ports, you have to switch ports
 by listen()ing on each one in turn. Pick a logical time to switch
 ports, like the end of an expected transmission, or when the
 buffer is empty. This example switches ports when there is nothing
 more to read from a port

 The circuit:
 Two devices which communicate serially are needed.
 * First serial device's TX attached to digital pin 10(RX), RX to pin 11(TX)
 * Second serial device's TX attached to digital pin 8(RX), RX to pin 9(TX)

 Note:
 Not all pins on the Mega and Mega 2560 support change interrupts,
 so only the following can be used for RX:
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

 Not all pins on the Leonardo support change interrupts,
 so only the following can be used for RX:
 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

 created 18 Apr. 2011
 modified 19 March 2016
 by Tom Igoe
 based on Mikal Hart's twoPortRXExample

 This example code is in the public domain.

 */

#include <SoftwareSerial.h>
// software serial #1: RX = digital pin 10, TX = digital pin 11
SoftwareSerial portOne(10, 11);

// software serial #2: RX = digital pin 8, TX = digital pin 9
// on the Mega, use other pins instead, since 8 and 9 don't work on the Mega
SoftwareSerial portTwo(8, 9);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  // Start each software serial port
  portOne.begin(115200);
  portTwo.begin(115200);

  Serial.println("init done");
}

void loop() {
  // By default, the last initialized port is listening.
  // when you want to listen on a port, explicitly select it:
  portOne.listen();
   // Serial.println("Data from port one:");
  // while there is data coming in, read it
  // and send to the hardware serial port:
  String message;
  char inByteCharArray[12] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  int i = 0;
  bool dataDetected = false;
  bool firstRun = true;
  bool printOut = false;
  while (portOne.available() > 0) {
    //dataDetected = true;

    byte data = portOne.read();

    dataDetected = true; 
    char hexStr[3]; // Buffer to store hexadecimal string representation
    // Convert byte to hexadecimal string
    sprintf(hexStr, "%02X", data);

    // Print hexadecimal string to Serial Monitor
    //Serial.print("Byte value: ");
    //Serial.print(data);
    //Serial.print(", Hexadecimal: 0x");
    Serial.print(hexStr);
    i++;
  }
    //Serial.write(inByteCharArray);

/*
  // blank line to separate data from the two ports:
  Serial.println();

  // Now listen on the second port
  portTwo.listen();
  // while there is data coming in, read it
  // and send to the hardware serial port:
  Serial.println("Data from port two:");
  while (portTwo.available() > 0) {
    char inByte = portTwo.read();
    Serial.write(inByte);
  }

*/
  // blank line to separate data from the two ports:
  if (dataDetected)
  {
  Serial.println();
  }
  //delay(30); // Delay for readability
}
