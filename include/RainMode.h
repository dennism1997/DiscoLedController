#ifndef DISCOLEDCONTROLLER_RAINMODE_H
#define DISCOLEDCONTROLLER_RAINMODE_H
#include "globals.h"
#include <FastLED.h>
#include "LedModeController.h"


class RainMode : public LedModeController {
public:
    explicit RainMode(size_t amountLeds) : LedModeController(amountLeds) {}

    void init(CRGB *leds) override {
    }

    void
    updateLeds(CRGB *leds, uint8_t beatCounter, bool beatUpdated, uint8_t beatBrightness) override {
        if (beatUpdated) {

            uint8_t interval;
            switch (getModeOption()) {
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

            if (getModeOption() < 2) {// go forward
                moveForward(leds);

            } else if (getModeOption() <= 4) { //go the other way
                moveBackwards(leds);
            } else {
                if (random8(10) < getModeOption()) {
                    moveForward(leds);
                } else {
                    moveBackwards(leds);
                }
            }
            counter = addmod8(counter, 1, interval);
        }

    }



private:
    size_t counter = 0;

    void moveForward(CRGB *leds) {
        memmove(leds + 1, leds, (amountLeds - 1) * 3);
        if (counter == 0) {
            leds[0].setHSV(usedHue, 255, 255);
        } else {
            leds[0].setHSV(usedHue, 255, 0);
        }
    }

    void moveBackwards(CRGB *leds) {
        memmove(leds, leds + 1, (amountLeds - 1) * 3);
        if (counter == 0) {
            leds[amountLeds - 1].setHSV(usedHue, 255, 255);
        } else {
            leds[amountLeds - 1].setHSV(usedHue, 255, 0);
        }
    }


};

#endif //DISCOLEDCONTROLLER_RAINMODE_H
