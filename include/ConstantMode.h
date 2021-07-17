//
// Created by dennis on 16-07-21.
//

#ifndef DISCOLEDCONTROLLER_CONSTANTMODE_H
#define DISCOLEDCONTROLLER_CONSTANTMODE_H

#include "globals.h"
#include <FastLED.h>
#include "LedModeController.h"

class ConstantMode : public LedModeController {
public:
    explicit ConstantMode(size_t amountLeds) : LedModeController(amountLeds) {}

    void init(CRGB *leds) override {
    }

    void
    updateLeds(CRGB *leds, uint8_t beatCounter, bool beatUpdated, uint8_t beatBrightness) override {
        int numToFill = static_cast<int>(amountLeds);
        fill_solid(leds, numToFill, CHSV(usedHue, 255, brightness));
    }
};

#endif //DISCOLEDCONTROLLER_CONSTANTMODE_H
