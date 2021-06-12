#ifndef DISCOLEDCONTROLLER_RAINMODE_H
#define DISCOLEDCONTROLLER_RAINMODE_H

#include <FastLED.h>
#include "LedModeController.h"


class RainMode : public LedModeController {
public:
    explicit RainMode(size_t amountLeds) : LedModeController(amountLeds) {}

    void init(CRGB *leds, uint8_t intensity, uint8_t h, uint8_t bpm, uint8_t colorMode) override {
    }

    void update(CRGB *leds, uint8_t intensity, uint8_t modeOption, uint8_t h, uint8_t bpm, uint8_t colorMode) override {

        uint8_t interval;
        switch (modeOption) {
            case 0:
                interval = 4;
                break;
            case 1:
                interval = 2;
                break;
            case 2:
                interval = 4;
                break;
            case 3:
                interval = 2;
                break;
            default:
                interval = 4;
        }

        if (modeOption < 2) {// go forward
//            for (size_t i = 1; i < amountLeds; i++) {
//                leds[i] = leds[i - 1];
//            }
            memmove(leds + 1, leds, (amountLeds - 1) * 3);
            if (counter == 0) {
                leds[0].setHSV(h, 255, 255);
            } else {
                leds[0].setHSV(h, 255, 0);
            }

        } else { //go the other way
//            for (size_t i = amountLeds - 1; i > 0; i--) {
//                leds[i] = leds[i + 1];
//            }
            memmove(leds, leds + 1, (amountLeds - 1) * 3);
            if (counter == 0) {
                leds[amountLeds - 1].setHSV(h, 255, 255);
            } else {
                leds[amountLeds - 1].setHSV(h, 255, 0);
            }
        }
        counter = addmod8(counter, 1, interval);

    }

private:
    size_t counter = 0;
};

#endif //DISCOLEDCONTROLLER_RAINMODE_H
