#include <string.h>
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <stdint.h>

#include "age.h"
#include "movement.h"
//#include "name.h"
//#include "polarity.h"
#include "secrets.h"

int status = WL_IDLE_STATUS;

char ssid[] = SECRET_SSID; // your network SSID (name)
char pass[] = SECRET_PASS; // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0; // your network key Index number (needed only for WEP)

unsigned int localPort = 2390; // local port to listen on

char packetBuffer[255]; // buffer to hold incoming packet
char ReplyBuffer[12]; // a string to send back
char motorBuffer[3]; // buffer to hold motor distance

String reply = "acknowledged";
uint16_t age;

WiFiUDP Udp;

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

  Serial.println("\nStarting connection to server...");
  // If you get a connection, report back via serial:
  Udp.begin(localPort);
  
  motorsetup();
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
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) packetBuffer[len] = 0;
    Serial.println("Contents:");
    Serial.println(packetBuffer);

    switch(packetBuffer[0]){
      // Decode alien's name:
      case 'A':
        reply.toCharArray(ReplyBuffer, 12);
        break;
      
      // Read alien's age:
      case 'B':
        age = readAge();
        itoa(age, ReplyBuffer, 10);
        Serial.println(age);
        break;

      // Measure polarity of alien's magnetic field:
      case 'C':
        reply.toCharArray(ReplyBuffer, 12);
        break;

      // Move rover:
      case 'D':
        reply.toCharArray(ReplyBuffer, 12);
        motorBuffer[0] = packetBuffer[2];
        motorBuffer[1] = packetBuffer[3];
        motorBuffer[2] = packetBuffer[4];
        controlMotors(packetBuffer[1], motorBuffer);
        break;
    }

    // Send a reply, to the IP address and port that sent us the packet we received:
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();
  }
}
