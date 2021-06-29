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

#define AMT_DATA_VALUES 8
// mode, modeOption, bpm, brightness, intensity, colorH1, colorH2, colorMode
extern uint8_t dataValues[AMT_DATA_VALUES] ;
extern uint8_t usedHue;


//<editor-fold desc="Getters">
uint8_t getLedMode() {
    return dataValues[0];
}

uint8_t getModeOption() {
    return dataValues[1];
}

uint8_t getBpm() {
    return dataValues[2];
}

uint8_t getBrightness() {
    return dataValues[3];
}

uint8_t getIntensity() {
    return dataValues[4];
}

uint8_t getHue1() {
    return dataValues[5];
}

uint8_t getHue2() {
    return dataValues[6];
}

uint8_t getColorMode() {
    return dataValues[7];
}

//</editor-fold>


#endif //DISCOLEDCONTROLLER_GLOBALS_H
