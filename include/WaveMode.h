//
// Created by dennis on 20-05-21.
//

#ifndef DISCOLEDCONTROLLER_WAVEMODE_H
#define DISCOLEDCONTROLLER_WAVEMODE_H

#include "globals.h"
#include <FastLED.h>
#include "LedModeController.h"

class WaveMode : public LedModeController {
public:
    explicit WaveMode(size_t amountLeds) : LedModeController(amountLeds) {}

    void init(CRGB *leds, uint8_t intensity, uint8_t h, uint8_t getBpm(), uint8_t colorMode) override {
    }

    void
    updateLeds(CRGB *leds, uint8_t beatCounter, bool beatUpdated, uint8_t beatBrightness) override {
        uint8_t brightness = beatsin8(getBpm() / 4, 64);
        switch (getModeOption()) {
            case 1:
                brightness = beatsin8(getBpm() / 2, 64);
                break;
            case 2:
                brightness = beatsin8(getBpm(), 64);
                break;
            case 3:
                brightness = beat8(getBpm() / 4);
                break;
            case 4:
                brightness = beat8(getBpm() / 2);
                break;
            case 5:
                brightness = beat8(getBpm());
                break;
            case 6:
                brightness = 255 - beat8(getBpm() / 4);
                break;
            case 7:
                brightness = 255 - beat8(getBpm() / 2);
                break;
            case 8:
                brightness = 255 - beat8(getBpm());
                break;
            case 9:
                brightness = static_cast<uint8_t>(round(acos(beatBrightness) / 1.5708));
                break;
            default:
                break;
        }
        fill_solid(leds, static_cast<int>(amountLeds), CHSV(usedHue, 255, brightness));
    }
};

#endif //DISCOLEDCONTROLLER_WAVEMODE_H
