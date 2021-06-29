//
// Created by dennis on 17-05-21.
//

#ifndef DISCOLEDCONTROLLER_CYLONMODE_H
#define DISCOLEDCONTROLLER_CYLONMODE_H

#include "globals.h"
#include <FastLED.h>
#include "LedModeController.h"

/**
 * Knight Rider like effect
 */
class CylonMode : public LedModeController {
public:
    explicit CylonMode(size_t amountLeds) : LedModeController(amountLeds) {}


    void init(CRGB *leds) override {
        fill_solid(leds, static_cast<int>(amountLeds), CHSV(usedHue, 255, 0));
    }

    void
    updateLeds(CRGB *leds, uint8_t beatCounter, bool beatUpdated, uint8_t beatBrightness) override {
        this->eyeSize = getModeOption() + 4;
        if (beatUpdated){
            for (size_t i = 0; i < amountLeds; i++) {
                if (i == position || i == position + eyeSize - 1) {
                    leds[i].setHSV(usedHue, 255, 128);
                } else if (i > position && i < position + eyeSize - 1) {
                    leds[i].setHSV(usedHue, 255, 255);
                } else {
                    leds[i].setHSV(usedHue, 255, 0);
                }

            }
//            memmove(leds + position * 3, (leds + direction * 3) + position * 3, eyeSize * 3);
            if (direction > 0) {
                leds[position].setHSV(usedHue, 255, 0);
            } else {
                leds[position].setHSV(usedHue, 255, 255);
            }
            position += direction * (getModeOption() + 1);
            if (position <= 0 || position >= amountLeds - eyeSize) {
                direction *= -1;
            }
        }
    }

private:
    size_t eyeSize = 4;
    size_t position = 0;
    int8 direction = 1;

};

#endif //DISCOLEDCONTROLLER_CYLONMODE_H
