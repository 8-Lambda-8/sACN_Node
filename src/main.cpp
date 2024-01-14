#include <Arduino.h>
#include <MultiDMX.h>

#include "Ethernet.h"
#include "sACN.h"

uint8_t mac[] = {0x90, 0xA2, 0xDA, 0x10, 0x14, 0x48};  // MAC Adress of your device
IPAddress ip(10, 0, 0, 199);                           // IP address of your device
IPAddress dns(10, 0, 0, 2);                            // DNS address of your device

EthernetUDP udp;
Receiver sacnRecievers[] = {Receiver(udp, 1),   //
                            Receiver(udp, 2)};  //

// Receiver(udp, 3),  //
// Receiver(udp, 4)};

PinName pins[DMX_UNIVERSES] = {PB_1, PB_0};  //{32, 33};  //, 25, 26};  //, PA_4, PA_5, PB_0, PB_1};

void dmxReceived_(uint8_t u) {
  //
  Serial.println("Recieved");
  sacnRecievers[u].dmx((MultiDmx.getBuffer(u)));
}

void dmxReceived_0() { dmxReceived_(0); }
void dmxReceived_1() { dmxReceived_(1); }
// void dmxReceived_2() { dmxReceived_(2); }
// void dmxReceived_3() { dmxReceived_(3); };

void (*dmxRecieved[])() = {dmxReceived_0, dmxReceived_1};  //, dmxReceived_2, dmxReceived_3};

void newSource() {
  Serial.print("new soure name: ");
  Serial.println(sacnRecievers[0].name());
}

/* void framerate() {
  Serial.print("Framerate fps: ");
  Serial.println(sacnRecievers[0].framerate());
} */

// void timeOut() { Serial.println("Timeout!"); }

void setup() {
  Serial.begin(115200);
  Serial.println("begin1");

  if (Ethernet.begin(mac) == 0) {
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("hardware Error");
      while (true) {
        delay(1);  // do nothing, no point running without Ethernet hardware
      }
    }
    Ethernet.begin(mac, ip, dns);
  } else {
    Serial.println(Ethernet.localIP());
  }
  sacnRecievers[0].callbackSource(newSource);
  for (uint8_t i = 0; i < DMX_UNIVERSES; i++) {
    sacnRecievers[i].callbackDMX(dmxRecieved[i]);
    // sacnRecievers[i].callbackTimeout(timeOut);
    // sacnRecievers[i].callbackSource(newSource);
    sacnRecievers[i].begin();
  }

  Serial.println("sACN start");

  MultiDmx.usePins(pins);

  MultiDmx.write(0, 6, 255);
  MultiDmx.write(1, 6, 255);
  // MultiDmx.write(2, 6, 255);
  // MultiDmx.write(3, 6, 255);
}

uint32_t loops = 0;
uint32_t timer = 0;

void loop() {
  for (uint8_t i = 0; i < DMX_UNIVERSES; i++) {
    sacnRecievers[i].receive();
  }
  if (millis() - timer > 1000) {
    timer = millis();
    Serial.print("loops: ");
    Serial.println(loops);
    loops = 0;
  }
  loops++;

  /* MultiDmx.write(0, 1, 0);
  MultiDmx.write(1, 3, 0);
  MultiDmx.write(2, 10, 0);
  MultiDmx.write(3, 12, 0);

  Serial.println(0);
  delay(2000);
  MultiDmx.write(0, 1, 255);
  MultiDmx.write(1, 3, 255);
  MultiDmx.write(2, 10, 255);
  MultiDmx.write(3, 12, 255);

  Serial.println(255);
  delay(2000); */
}
