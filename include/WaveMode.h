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

    void init(CRGB *leds) override {
    }

    void
    updateLeds(CRGB *leds, uint8_t beatCounter, bool beatUpdated, uint8_t beatBrightness) override {
        uint8_t value = beatsin8(bpm / 4, 64);
        switch (modeOption) {
            case 1:
                value = beatsin8(bpm / 2, 64);
                break;
            case 2:
                value = beatsin8(bpm, 64);
                break;
            case 3:
                value = beat8(bpm / 4);
                break;
            case 4:
                value = beat8(bpm / 2);
                break;
            case 5:
                value = beat8(bpm);
                break;
            case 6:
                value = 255 - beat8(bpm / 4);
                break;
            case 7:
                value = 255 - beat8(bpm / 2);
                break;
            case 8:
                value = 255 - beat8(bpm);
                break;
            case 9:
                value = static_cast<uint8_t>(round(acos(beatBrightness) / 1.5708));
                break;
            default:
                break;
        }
        fill_solid(leds, static_cast<int>(amountLeds), CHSV(usedHue, 255, value));
    }
};

#endif //DISCOLEDCONTROLLER_WAVEMODE_H
