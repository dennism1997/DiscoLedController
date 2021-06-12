//
// Created by dennis on 21-05-21.
//

// use regex:
// rgb\((\s*\d{1,3}),(\s*\d{1,3}),(\s*\d{1,3})\)\s+(\d+)\.\d*%
// $4, $1, $2, $3

#ifndef DISCOLEDCONTROLLER_COLORPALETTES_H
#define DISCOLEDCONTROLLER_COLORPALETTES_H

#include "FastLed.h"

DEFINE_GRADIENT_PALETTE(Sunset_Real_gp) {
        0, 120, 0, 0,
        22, 179, 22, 0,
        51, 255, 104, 0,
        85, 167, 22, 18,
        135, 100, 0, 103,
        198, 16, 0, 130,
        255, 0, 0, 160
};

//http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_sunconure.png.index.html
DEFINE_GRADIENT_PALETTE(bhw1_sunconure_gp) {
        0, 97, 240, 78,
        160, 246, 137, 30,
        253, 246, 45, 30,
        255, 246, 45, 30
};

//http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_purplered.png.index.html
DEFINE_GRADIENT_PALETTE(bhw1_purplered_gp) {
        0, 255, 0, 0,
        255, 183, 26, 234
};

//http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_28.png.index.html
DEFINE_GRADIENT_PALETTE(bhw1_28_gp) {
        0, 160, 19, 241,
        31, 254, 145, 255,
        46, 218, 0, 250,
        120, 0, 200, 250,
        168, 129, 0, 250,
        207, 254, 145, 255,
        230, 162, 64, 238,
        255, 0, 200, 250
};

//http://soliton.vm.bytemark.co.uk/pub/cpt-city/ocal/tn/blues.png.index.html
DEFINE_GRADIENT_PALETTE(blues_gp) {
        0, 0, 0, 128,
        26, 0, 0, 192,
        51, 0, 0, 255,
        76, 64, 64, 255,
        127, 64, 127, 255,
        178, 0, 193, 255,
        242, 0, 255, 255,
        255, 255, 255, 255
};


#endif //DISCOLEDCONTROLLER_COLORPALETTES_H
