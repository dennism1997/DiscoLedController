//
// Created by dennis on 29-06-21.
//

#ifndef DISCOLEDCONTROLLER_GLOBALS_H
#define DISCOLEDCONTROLLER_GLOBALS_H


enum LedMode {
    Rain, Strobe, Cylon, Sparkle, Flash, Wave, AMOUNT_LEDMODE
};

enum ColorMode {
    Single, Complement, RainbowFade, RainbowSplash, Duo, Palette, Close, AMOUNT_COLORMODE
};

#define AMT_DATA_VALUES 9
// mode, modeOption, bpm, brightness, intensity, colorH1, colorH2, colorMode, paletteIndex
extern uint8_t dataValues[AMT_DATA_VALUES] ;
extern uint8_t usedHue;
extern LedMode ledMode;
extern uint8_t modeOption;
extern uint8_t bpm;
extern uint8_t brightness;
extern uint8_t intensity;
extern uint8_t hue1;
extern uint8_t hue2;
extern ColorMode colorMode;
extern uint8_t paletteIndex;

//</editor-fold>


#endif //DISCOLEDCONTROLLER_GLOBALS_H
