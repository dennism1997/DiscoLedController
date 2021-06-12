//
// Created by dennis on 20-05-21.
//

#ifndef DISCOLEDCONTROLLER_BEATMODE_H
#define DISCOLEDCONTROLLER_BEATMODE_H

#include <FastLED.h>
#include "LedModeController.h"

class BeatMode : public LedModeController {
public:
    explicit BeatMode(size_t amountLeds) : LedModeController(amountLeds) {}

    void init(CRGB *leds, uint8_t intensity, uint8_t h, uint8_t bpm, uint8_t colorMode) override {
    }

    void update(CRGB *leds, uint8_t intensity, uint8_t modeOption, uint8_t h, uint8_t bpm, uint8_t colorMode) override {
        uint8_t wave = beat8(bpm);
        fill_solid(leds, static_cast<int>(amountLeds), CHSV(h, 255, wave));
        counter = addmod8(counter, 1, 16);
    }

private:
    size_t counter = 0;
};

#endif //DISCOLEDCONTROLLER_BEATMODE_H
