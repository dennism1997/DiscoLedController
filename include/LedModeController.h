//
// Created by dennis on 17-05-21.
//

#ifndef DISCOLEDCONTROLLER_LEDMODECONTROLLER_H
#define DISCOLEDCONTROLLER_LEDMODECONTROLLER_H

#include "FastLED.h"

class LedModeController {
public:
    explicit LedModeController(size_t amountLeds) : amountLeds(amountLeds) {}

    virtual void init(CRGB *leds, uint8_t intensity, uint8_t h, uint8_t bpm, uint8_t colorMode) = 0;

    virtual void
    update(CRGB *leds, uint8_t intensity, uint8_t modeOption, uint8_t h, uint8_t bpm, uint8_t colorMode) = 0;

protected:
    size_t amountLeds;
};

#endif //DISCOLEDCONTROLLER_LEDMODECONTROLLER_H
