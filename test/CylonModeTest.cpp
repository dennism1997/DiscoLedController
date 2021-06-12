#include <Arduino.h>


#define SPI_DATA D6
#define SPI_CLOCK D5
#define FASTLED_ALLOW_INTERRUPTS 0 // https://forum.makerforums.info/t/im-having-trouble-just-getting-fastled-to-work-properly-with-a-wemos-d1-mini/62964/5
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#include <FastLED.h>

#include "CylonMode.h"
#include <unity.h>


const byte amount_leds = 10;
CRGB leds[amount_leds];
CylonMode cylonMode;

void setUp(void) {
    cylonMode = CylonMode(2, 2, 2);
    cylonMode.init(leds, 0);
}

void testUpdateOnce(void) {
    cylonMode.update(leds, 0);
    TEST_ASSERT_EQUAL(1, cylonMode.getPosition());
}

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);

    UNITY_BEGIN();
    RUN_TEST(testUpdateOnce);
    UNITY_END();
}

void loop() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
}



