//
// Created by dennis on 17-05-21.
//

#ifndef DISCOLEDCONTROLLER_STROBEMODE_H
#define DISCOLEDCONTROLLER_STROBEMODE_H
#include "globals.h"
#include <FastLED.h>
#include "LedModeController.h"

class StrobeMode : public LedModeController {
public:
    explicit StrobeMode(size_t amountLeds) : LedModeController(amountLeds) {}

    void init(CRGB *leds) override {
    }

    void
    updateLeds(CRGB *leds, uint8_t beatCounter, bool beatUpdated, uint8_t beatBrightness) override {
        if (beatUpdated) {

            size_t interval = amountLeds / steps;
            uint8_t randomModifier = random(-10 * intensity, 10 * intensity);
            for (size_t i = 0; i < amountLeds; i++) {
                if (i >= interval * counter && i < interval * (counter + 1)) {
                    leds[i].setHSV(qadd8(usedHue, randomModifier), 255, 255);
                } else {
//                memset(leds, 0, amountLeds * 3);
                    leds[i].setHSV(0, 0, 0);
                }
            }


            if (modeOption == 0) {
                if (counter < 2) {
                    counter += 2;
                } else if (counter % 2 == 0) {
                    counter -= 1;
                } else {
                    counter += 1;
                }
            } else if (modeOption == 1) {
                counter += random8(steps);
            } else if (modeOption == 2) {
                counter += 1;
            } else {
                counter -= 1;
            }

            if (counter < 0) {
                counter += steps;
            }
            counter = mod8(counter, steps);
        }
    }

private:
    size_t counter = 0;
    size_t steps = 4;
};

#endif //DISCOLEDCONTROLLER_STROBEMODE_H
