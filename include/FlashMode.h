#ifndef DISCOLEDCONTROLLER_FLASHMODE_H
#define DISCOLEDCONTROLLER_FLASHMODE_H
#include "globals.h"
#include <FastLED.h>
#include "LedModeController.h"

class FlashMode : public LedModeController {
public:
    explicit FlashMode(size_t amountLeds) : LedModeController(amountLeds) {}

    void init(CRGB *leds) override {
    }

    void
    updateLeds(CRGB *leds, uint8_t beatCounter, bool beatUpdated, uint8_t beatBrightness) override {
        int numToFill = static_cast<int>(amountLeds);
        uint16_t mod = modeOption;
        if (mod <= 4) {
            if (counter == 0) {
                fill_solid(leds, numToFill, CHSV(usedHue, 255, 255));
            } else {
                fill_solid(leds, numToFill, CHSV(usedHue, 255, 0));
            }
            if (mod < 2) {
                mod = 4;
            }
            if (beatUpdated) {
                counter = addmod8(counter, 1, mod);
            }
        } else {
            fill_solid(leds, numToFill, CHSV(usedHue, 255, brightness));
        }

    }

private:
    size_t counter = 0;
};

#endif //DISCOLEDCONTROLLER_FLASHMODE_H
