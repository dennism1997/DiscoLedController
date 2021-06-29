//
// Created by dennis on 17-05-21.
//

#ifndef DISCOLEDCONTROLLER_LEDMODECONTROLLER_H
#define DISCOLEDCONTROLLER_LEDMODECONTROLLER_H

#include "FastLED.h"

class LedModeController {
public:
    explicit LedModeController(size_t amountLeds) : amountLeds(amountLeds) {}

    virtual void init(CRGB *leds) = 0;

    virtual void
    updateLeds(CRGB *leds, uint8_t beatCounter, bool beatUpdated, uint8_t beatBrightness) = 0;

protected:
    size_t amountLeds;
};

#endif //DISCOLEDCONTROLLER_LEDMODECONTROLLER_H
