#include <string.h>
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <stdint.h>
#include <Wire.h>
#include <Adafruit_FXOS8700.h>

#include "secrets.h"
//#include "name.h"
#include "age.h"
//#include "polarity.h"
#include "movement.h"

int status = WL_IDLE_STATUS;

char ssid[] = SECRET_SSID; // your network SSID (name)
char pass[] = SECRET_PASS; // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0; // your network key Index number (needed only for WEP)

unsigned int localPort = 2390; // local port to listen on

char packetBuffer[8]; // buffer to hold incoming packet
char replyBuffer[32]; // buffer to hold outgoing packet
char motorBuffer[6]; // buffer to hold motor commands
byte InChar[10]; // buffer to hold incoming name

String reply; // string to send back

WiFiUDP Udp;

/* Assign a unique ID to this sensor at the same time */
Adafruit_FXOS8700 accelmag = Adafruit_FXOS8700(0x8700A, 0x8700B);

sensors_event_t aevent, mevent;
float y_ref = 0, y; // reference value for magnetic field

void printWiFiStatus() {
  // Print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // Print your WiFi 101 Shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // Print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB port only.
  }

  // Check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi 101 Shield not present");
    // Don't continue:
    while (true);
  }

  // Attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // Wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to WiFi");
  printWiFiStatus();

  motorSetup();

  /* Initialise the sensor */
  if (!accelmag.begin()) {
    /* There was a problem detecting the FXOS8700 ... check your connections */
    Serial.println("Ooops, no FXOS8700 detected ... Check your wiring!");
    while (1)
      ;
  }

  Udp.begin(localPort);
  Serial.println("Now listening...");
}

void loop() {
  // If there's data available, read a packet:
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // Read the packet into packetBufffer:
    int len = Udp.read(packetBuffer, 8);
    if (len > 0) packetBuffer[len] = 0;
    Serial.print("Contents: ");
    Serial.println(packetBuffer);

    switch(packetBuffer[0]){
      // Move rover:
      case 'A':
        motorBuffer[0] = packetBuffer[1];
        motorBuffer[1] = packetBuffer[2];
        motorBuffer[2] = packetBuffer[3];
        motorBuffer[3] = packetBuffer[4];
        motorBuffer[4] = packetBuffer[5];
        motorBuffer[5] = packetBuffer[6];
        controlMotors(motorBuffer);
        Serial.print("Moving rover with input: ");
        Serial.print(motorBuffer[0]);
        Serial.print(motorBuffer[1]);
        Serial.print(motorBuffer[2]);
        Serial.print(motorBuffer[3]);
        Serial.print(motorBuffer[4]);
        Serial.println(motorBuffer[5]);
        break;
      // Decode alien's name:
      case 'B':
        Serial1.begin(600);
        Serial1.readBytes(InChar,10);
        reply = "";
        reply.toCharArray(replyBuffer, 16);
        replyBuffer[0] = char(InChar[0]);
        replyBuffer[1] = char(InChar[1]);
        replyBuffer[2] = char(InChar[2]);
        replyBuffer[3] = char(InChar[3]);
        replyBuffer[4] = char(InChar[4]);
        replyBuffer[5] = char(InChar[5]);
        replyBuffer[6] = char(InChar[6]);
        replyBuffer[7] = char(InChar[7]);
        replyBuffer[8] = char(InChar[8]);
        replyBuffer[9] = char(InChar[9]);
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write(replyBuffer);
        Udp.endPacket();
        Serial.println("Sent name");
        break;
      
      // Read alien's age:
      case 'C':
        reply = String(readAge());
        reply.toCharArray(replyBuffer, 16);
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write(replyBuffer);
        Udp.endPacket();
        Serial.println("Sent age");
        break;

      // Callibrate magnetic field reference values:
      case 'D':
        for(int i=0; i<100; i++){
          accelmag.getEvent(&aevent, &mevent);
          y = mevent.magnetic.y;
        }
        y_ref = y;
        reply = String(y_ref);
        reply.toCharArray(replyBuffer, 16);
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write(replyBuffer);
        Udp.endPacket();
        Serial.println("Sent reference value");
        break;
      // Measure polarity of alien's magnetic field:
      case 'E':
        for(int i=0; i<100; i++){
          accelmag.getEvent(&aevent, &mevent);
          y = mevent.magnetic.y;
        }
        //Serial.println(y - y_ref);

        // Determine the magnet's polarity
        if (y - y_ref > 50) {
          reply = "North Up";
        } else if (y - y_ref < -50) {
          reply = "South Up";
        } else {
          reply = "Neutral";
        }
        reply.toCharArray(replyBuffer, 16);
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write(replyBuffer);
        Udp.endPacket();
        Serial.println("Sent polarity");
        break;
    }
  }
}
