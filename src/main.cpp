#include <Arduino.h>
#include <MultiDMX_STM32.h>

PinName pins[DMX_UNIVERSES] = {PA_0, PA_1, PA_2, PA_3};  //, PA_4, PA_5, PB_0, PB_1};

void setup() {
  Serial.begin(115200);

  MultiDmx.usePins(pins);
}

void loop() {
}

