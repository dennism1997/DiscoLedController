//
// Created by dennis on 17-05-21.
//

#ifndef DISCOLEDCONTROLLER_SPARKLEMODE_H
#define DISCOLEDCONTROLLER_SPARKLEMODE_H

#include <FastLED.h>
#include "LedModeController.h"
#include <set>

class SparkleMode : public LedModeController {
public:
    explicit SparkleMode(size_t amountLeds) : LedModeController(amountLeds) {}

    void init(CRGB *leds, uint8_t intensity, uint8_t h, uint8_t bpm, uint8_t colorMode) override {
        for (size_t i = 0; i < amountLeds; i++) {
            leds[i].setHSV(h, 255, 128);
        }
    }

    void update(CRGB *leds, uint8_t intensity, uint8_t modeOption, uint8_t h, uint8_t bpm, uint8_t colorMode) override {

        if (modeOption == 0) {
            leds[selectedPixel].setHSV(h, 255, 128);
            selectedPixel = random16(amountLeds);
            leds[selectedPixel].setHSV(h, 255, 255);
        } else if (modeOption == 1) {
            if (coveredPixels.size() >= amountLeds) {
                coveredPixels.clear();
                fill_solid(leds, static_cast<int>(amountLeds), CHSV(h, 255, 128));
            }

            do {
                selectedPixel = random16(amountLeds);
            } while (coveredPixels.find(selectedPixel) != coveredPixels.end());
            leds[selectedPixel].setHSV(h, 255, 255);
            coveredPixels.insert(selectedPixel);

        }
    }

private:
    uint16_t selectedPixel = 0;
    std::set<uint16_t> coveredPixels;
};

#endif //DISCOLEDCONTROLLER_SPARKLEMODE_H
