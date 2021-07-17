//
// Created by dennis on 29-06-21.
//

#ifndef DISCOLEDCONTROLLER_GLOBALS_H
#define DISCOLEDCONTROLLER_GLOBALS_H


enum LedMode {
    Rain, Strobe, Cylon, Sparkle, Flash, Wave, Constant, AMOUNT_LEDMODE
};

enum ColorMode {
    Single, Complement, RainbowFade, RainbowSplash, Duo, Palette, Close, AMOUNT_COLORMODE
};
#define AMT_DATA_VALUES 10
// mode, modeOption, bpm, brightness, intensity, colorH1, colorH2, colorMode, paletteIndex, sendLedsSocket
extern LedMode ledMode;

extern uint8_t modeOption;
extern uint8_t bpm;
extern uint8_t brightness;
extern uint8_t intensity;
extern uint8_t hue1;
extern uint8_t hue2;
extern ColorMode colorMode;
extern uint8_t paletteIndex;
extern bool sendLedsSocket;

extern uint8_t usedHue;

//</editor-fold>


#endif //DISCOLEDCONTROLLER_GLOBALS_H
