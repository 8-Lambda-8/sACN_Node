#include <Arduino.h>
#include <esp_dmx.h>

#include "Ethernet.h"
#include "sACN.h"

uint8_t mac[] = {0x90, 0xA2, 0xDA, 0x10, 0x14, 0x48};  // MAC Adress of your device
IPAddress ip(10, 0, 0, 199);                           // IP address of your device
IPAddress dns(10, 0, 0, 2);                            // DNS address of your device

EthernetUDP udp;
Receiver sacnRecievers[] = {Receiver(udp, 1),   //
                            Receiver(udp, 2)};  //

#define DMX_UNIVERSES 2

uint8_t pins[DMX_UNIVERSES] = {15, 21};

void dmxReceived_(uint8_t u) {
  Serial.println("Recieved");
  dmx_write(u + 1, sacnRecievers[u].dmx() - 1, DMX_PACKET_SIZE);
}

void dmxReceived_0() { dmxReceived_(0); }
void dmxReceived_1() { dmxReceived_(1); }

void (*dmxRecieved[])() = {dmxReceived_0, dmxReceived_1};

void newSource() {
  Serial.print("new soure name: ");
  Serial.println(sacnRecievers[0].name());
}

void setup() {
  Serial.begin(115200);
  Serial.println("begin");

  Ethernet.init(5);
  if (Ethernet.begin(mac) == 0) {
    while (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("hardware Error");
      delay(10);
      Ethernet.begin(mac, ip, dns);
    }
  }
  Serial.println(Ethernet.localIP());

  for (uint8_t i = 0; i < DMX_UNIVERSES; i++) {
    sacnRecievers[i].callbackDMX(dmxRecieved[i]);
    sacnRecievers[i].callbackSource(newSource);
    sacnRecievers[i].begin();
  }

  Serial.println("sACN start");

  dmx_config_t config = DMX_CONFIG_DEFAULT;

  for (uint8_t i = 0; i < DMX_UNIVERSES; i++) {
    dmx_driver_install(i + 1, &config, DMX_INTR_FLAGS_DEFAULT);
    dmx_set_pin(i + 1, pins[i], DMX_PIN_NO_CHANGE, DMX_PIN_NO_CHANGE);
  }
}

uint32_t loops = 0;
uint32_t timer = 0;

void loop() {
  for (uint8_t i = 0; i < DMX_UNIVERSES; i++) {
    sacnRecievers[i].receive();
    dmx_send(i + 1, DMX_PACKET_SIZE);
  }
  if (millis() - timer > 1000) {
    timer = millis();
    Serial.print("loops: ");
    Serial.println(loops);
    loops = 0;
  }
  loops++;
}
