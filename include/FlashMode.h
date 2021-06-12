//
// Created by dennis on 18-05-21.
//

#ifndef DISCOLEDCONTROLLER_FLASHMODE_H
#define DISCOLEDCONTROLLER_FLASHMODE_H

#include <FastLED.h>
#include "LedModeController.h"

class FlashMode : public LedModeController {
public:
    explicit FlashMode(size_t amountLeds) : LedModeController(amountLeds) {}

    void init(CRGB *leds, uint8_t intensity, uint8_t h, uint8_t bpm, uint8_t colorMode) override {
    }

    void update(CRGB *leds, uint8_t intensity, uint8_t modeOption, uint8_t h, uint8_t bpm, uint8_t colorMode) override {
        int numToFill = static_cast<int>(amountLeds);
        if (counter == 0) {
            fill_solid(leds, numToFill, CHSV(h, 255, 255));
        } else {
            fill_solid(leds, numToFill, CHSV(h, 255, 0));
        }
        int16_t mod = modeOption;
        if (mod < 2) {
            mod = 4;
        }
        counter = addmod8(counter, 1, mod);
    }

private:
    size_t counter = 0;
};

#endif //DISCOLEDCONTROLLER_FLASHMODE_H
