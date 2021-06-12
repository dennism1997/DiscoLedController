//
// Created by dennis on 17-05-21.
//

#ifndef DISCOLEDCONTROLLER_CYLONMODE_H
#define DISCOLEDCONTROLLER_CYLONMODE_H

#include <FastLED.h>
#include "LedModeController.h"

/**
 * Knight Rider like effect
 */
class CylonMode : public LedModeController {
public:
    explicit CylonMode(size_t amountLeds) : LedModeController(amountLeds) {}


    void init(CRGB *leds, uint8_t intensity, uint8_t h, uint8_t bpm, uint8_t colorMode) override {
        fill_solid(leds, static_cast<int>(amountLeds), CHSV(h, 255, 0));
    }

    void update(CRGB *leds, uint8_t intensity, uint8_t modeOption, uint8_t h, uint8_t bpm, uint8_t colorMode) override {
        size_t nLeds = amountLeds;
        for (size_t i = 0; i < nLeds; i++) {
            if (i == position || i == position + eyeSize - 1) {
                leds[i].setHSV(h, 255, 128);
            } else if (i > position && i < position + eyeSize - 1) {
                leds[i].setHSV(h, 255, 255);
            } else {
                leds[i].setHSV(h, 255, 0);
            }

        }
        memmove(leds + position * 3, (leds + direction * 3) + position * 3, eyeSize * 3);
        if (direction > 0) {
            leds[position].setHSV(h, 255, 0);
        } else {
            leds[position].setHSV(h, 255, 255);
        }
        position += direction * (modeOption + 1);
        if (position <= 0 || position >= nLeds - eyeSize) {
            direction *= -1;
        }
    }

private:
    size_t eyeSize = 4;
    size_t position = 0;
    int8 direction = 1;

};

#endif //DISCOLEDCONTROLLER_CYLONMODE_H
