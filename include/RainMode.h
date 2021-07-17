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

            if (modeOption <= 4) {
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
                    moveForward(leds);

                } else if (modeOption <= 4) { //go the other way
                    moveBackwards(leds);
                }
                counter = addmod8(counter, 1, interval);
            } else if (modeOption <= 16) {
                if (counter == 0 ) {
                    moveForward(leds);
                }
                counter = addmod8(counter, 1, modeOption);
            } else {
                if (random8(128) < modeOption) {
                    moveForward(leds);
                } else {
                    moveBackwards(leds);
                }
            }
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
