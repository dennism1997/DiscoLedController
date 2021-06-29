//
// Created by dennis on 17-05-21.
//

#ifndef DISCOLEDCONTROLLER_SPARKLEMODE_H
#define DISCOLEDCONTROLLER_SPARKLEMODE_H
#include "globals.h"
#include <FastLED.h>
#include "LedModeController.h"
#include <set>

class SparkleMode : public LedModeController {
public:
    explicit SparkleMode(size_t amountLeds) : LedModeController(amountLeds) {}

    void init(CRGB *leds) override {
        for (size_t i = 0; i < amountLeds; i++) {
            leds[i].setHSV(usedHue, 255, 128);
        }
    }

    void
    updateLeds(CRGB *leds, uint8_t beatCounter, bool beatUpdated, uint8_t beatBrightness) override {
        if (beatUpdated) {

            if (modeOption == 0) {

                leds[oldSelectedPixel].setHSV(usedHue, 255, beatBrightness / 2);
                leds[selectedPixel].setHSV(usedHue, 255, beatBrightness);
                oldSelectedPixel = selectedPixel;
                selectedPixel = random16(amountLeds);

            } else if (modeOption == 1) {
                if (coveredPixels.size() >= amountLeds) {
                    coveredPixels.clear();
                    fill_solid(leds, static_cast<int>(amountLeds), CHSV(usedHue, 255, 100));
                }

                do {
                    selectedPixel = random16(amountLeds);
                } while (coveredPixels.find(selectedPixel) != coveredPixels.end());
                leds[selectedPixel].setHSV(usedHue, 255, 255);
                coveredPixels.insert(selectedPixel);
            }
        }

    }

private:
    uint16_t selectedPixel = 0;
    uint16_t oldSelectedPixel = 0;
    std::set<uint16_t> coveredPixels;
};

#endif //DISCOLEDCONTROLLER_SPARKLEMODE_H
