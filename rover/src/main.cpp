#include <string.h>
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <stdint.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS1.h>

#include "secrets.h"
#include "name.h"
#include "age.h"
#include "polarity.h"
#include "movement.h"

int status = WL_IDLE_STATUS;

char ssid[] = SECRET_SSID; // your network SSID (name)
char pass[] = SECRET_PASS; // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0; // your network key Index number (needed only for WEP)

unsigned int localPort = 2390; // local port to listen on

char packetBuffer[8]; // buffer to hold incoming packet
char replyBuffer[32]; // buffer to hold outgoing packet
char motorBuffer[6]; // buffer to hold motor commands

String reply; // string to send back

WiFiUDP Udp;

// Create an instance of the LSM9DS1 sensor
Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();

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

  // Initialize the sensor
  if (!lsm.begin()) {
    Serial.println("Failed to initialize the LSM9DS1 sensor. Please check your wiring.");
    while (1);
  }

  // Set magnetometer data rate
  lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);

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
      // Decode alien's name:
      case 'A':
        reply = "NAME";
        reply.toCharArray(replyBuffer, 16);
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write(replyBuffer);
        Udp.endPacket();
        Serial.println("Sent name");
        break;
      
      // Read alien's age:
      case 'B':
        reply = String(readAge());
        reply.toCharArray(replyBuffer, 16);
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write(replyBuffer);
        Udp.endPacket();
        Serial.println("Sent age");
        break;

      // Measure polarity of alien's magnetic field:
      case 'C':
        reply = readPolarity(lsm);
        reply.toCharArray(replyBuffer, 16);
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write(replyBuffer);
        Udp.endPacket();
        Serial.println("Sent polarity");
        break;

      // Move rover:
      case 'D':
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
    }
  }
}
