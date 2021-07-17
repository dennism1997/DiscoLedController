#include <Arduino.h>

#define DEBUG 1
#define DEBUG_WEBSOCKET 1

#ifdef DEBUG
#define DEBUG_PRINT(x)     Serial.print (x)
#define DEBUG_PRINTDEC(x)     Serial.print (x, DEC)
#define DEBUG_PRINTLN(x)  Serial.println (x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTDEC(x)
#define DEBUG_PRINTLN(x)
#endif

#define SPI_DATA D6
#define SPI_CLOCK D5

#include "globals.h"
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <WebSocketsServer.h>

#define FASTLED_ALLOW_INTERRUPTS 0 // https://forum.makerforums.info/t/im-having-trouble-just-getting-fastled-to-work-properly-with-a-wemos-d1-mini/62964/5
//#define FASTLED_ESP8266_NODEMCU_PIN_ORDER

#include "FastLED.h"
#include "RainMode.h"
#include "StrobeMode.h"
#include "CylonMode.h"
#include "SparkleMode.h"
#include "FlashMode.h"
#include "WaveMode.h"
#include <ConstantMode.h>
#include "ColorPalettes.h"

#define NUM_LEDS 36
CRGB leds[NUM_LEDS];

RainMode rainMode(NUM_LEDS);
StrobeMode strobeMode(NUM_LEDS);
CylonMode cylonMode(NUM_LEDS);
SparkleMode sparkleMode(NUM_LEDS);
FlashMode flashMode(NUM_LEDS);
WaveMode waveMode(NUM_LEDS);
ConstantMode constantMode(NUM_LEDS);

LedModeController *currentLedMode;


CRGBPalette16 currentPalette = Sunset_Real_gp;
TBlendType currentBlending = LINEARBLEND; // NOBLEND or LINEARBLEND

//<editor-fold desc="Declarations">
// WS2815 - 220ns, 360ns, 220ns https://github.com/FastLED/FastLED/issues/940
template<uint8_t DATA_PIN = D4, EOrder RGB_ORDER = RGB>
class WS2815Controller : public ClocklessController<DATA_PIN, C_NS(220), C_NS(360), C_NS(220), RGB_ORDER> {
};

template<uint8_t DATA_PIN, EOrder RGB_ORDER>
class WS2815 : public WS2815Controller<DATA_PIN, RGB_ORDER> {
};

void tickBeatClock();

#ifdef DEBUG
void serialPrintLeds();

void serialPrintResult();
#endif
void setupLeds();

void setupServer();

void changeMode(LedMode newMode);

void setBpm(uint8_t bpm);
#ifdef DEBUG_WEBSOCKET
void sendLedsToSocket();
#endif
void updateLeds();

void changePalette(uint8_t newPaletteIndex);

void setupOTA();

IPAddress ip(192, 168, 178, 200);
IPAddress gateway(192, 168, 178, 1);
IPAddress subnet(255, 255, 255, 0);
WebSocketsServer webSocket = WebSocketsServer(80);

//</editor-fold>

uint8_t oldDataValues[AMT_DATA_VALUES];

LedMode ledMode = LedMode::Wave;
uint8_t modeOption = 11;
uint8_t bpm = 123;
uint8_t brightness = 60;
uint8_t intensity = 0;
uint8_t hue1 = 0;
uint8_t hue2 = 128;
ColorMode colorMode = ColorMode::Palette;
uint8_t paletteIndex = 0;
uint8_t usedHue = hue1;
bool sendLedsSocket = true;

//timing
uint8_t beatCounter = 0;
boolean updatedThisBeat = false;
#define TICKS_PER_BEAT 16
Ticker beatTicker(tickBeatClock, 1000, 0, MICROS);
uint32_t timebase = 0;

uint8_t colorIndex = 0;

void updateColor() {
    colorIndex++;
    switch (static_cast<ColorMode>(colorMode)) {
        case Single: {
            usedHue = hue1;
            break;
        }
        case Complement: {
            int16_t h = hue1;
            if (colorIndex % 2 == 0) {
                h += 128;
            }

            usedHue = h;
            break;
        }
        case RainbowFade: {
            if (colorIndex % 8 == 0) {
                usedHue = usedHue + 1;
            }
            break;
        }
        case RainbowSplash: {
            usedHue = usedHue + 39;
            break;
        }
        case Duo: {
            if (colorIndex % 4 == 0) {
                if (usedHue == hue1) {
                    usedHue = hue2;
                } else {
                    usedHue = hue1;
                }
            }
            break;
        }
        case Palette: {
            usedHue = rgb2hsv_approximate(ColorFromPalette(currentPalette, colorIndex, 255, currentBlending)).h;
            break;
        }
        case Close: {
            if (colorIndex % 8 == 0) {
                if (usedHue == hue1) {
                    usedHue += 10;
                } else {
                    usedHue = hue1;
                }
            }
            break;
        }
        default:
            break;
    }
}


void tickBeatClock() {
    beatCounter = addmod8(beatCounter, 1, TICKS_PER_BEAT);
    updatedThisBeat = false;
}

__attribute__((unused)) void setup() {
    Serial.begin(115200);


    setupLeds();
    pinMode(LED_BUILTIN, OUTPUT);

    setupServer();
    setupOTA();

    changeMode(static_cast<LedMode>(ledMode));
    setBpm(bpm);
    beatTicker.start();
}


__attribute__((unused)) void loop() {
    ArduinoOTA.handle();
    webSocket.loop();
    beatTicker.update();
    updateLeds();
}

void updateLeds() {
    uint8_t beatBrightness = beatsin8(bpm, 0, 255, timebase);
    bool beatUpdated = false;
    if (!updatedThisBeat) {
        updatedThisBeat = true;
        if ((intensity == 0 && beatCounter % 4 == 0) ||
            (intensity == 1 && beatCounter % 2 == 0) ||
            intensity >= 2) {
            updateColor();
            beatUpdated = true;
#ifdef DEBUG_WEBSOCKET
            sendLedsToSocket();
#endif
#ifdef DEBUG
            serialPrintLeds();
#endif
        }

//        if (beatCounter % 4 == 0) {
//            sendLedsToSocket();
//            serialPrintLeds();
//        }
    }
    currentLedMode->updateLeds(leds, beatCounter, beatUpdated, beatBrightness);

    FastLED.show();
}

void changeMode(LedMode newMode) {
//    auto oldMode = static_cast<LedMode>(dataValues[0]);
    ledMode = newMode;
    DEBUG_PRINTLN(newMode);
    switch (newMode) {
        case LedMode::Rain: {
            currentLedMode = &rainMode;
            break;
        }
        case LedMode::Strobe: {
            currentLedMode = &strobeMode;
            break;
        }
        case LedMode::Cylon: {
            currentLedMode = &cylonMode;
            break;
        }
        case LedMode::Sparkle: {
            currentLedMode = &sparkleMode;
            break;
        }
        case LedMode::Flash: {
            currentLedMode = &flashMode;
            break;
        }
        case LedMode::Wave: {
            currentLedMode = &waveMode;
            break;
        }
        case LedMode::Constant: {
            currentLedMode = &constantMode;
            break;
        }
        default:
            DEBUG_PRINT("change into unkown mode option: ");
            DEBUG_PRINTLN(newMode);
            break;
    }
    currentLedMode->init(leds);

}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, __attribute__((unused)) size_t length) {
    switch (type) {
        case WStype_TEXT: {
            Serial.printf("[%u] get Text: %s\n", num, payload);
            size_t index = 0;
            uint8_t newDataValues[AMT_DATA_VALUES];

            char *ptr = strtok((char *) payload, ",");

            while (ptr != nullptr && index < AMT_DATA_VALUES) {
                newDataValues[index++] = atoi(ptr); // NOLINT(cert-err34-c)
                ptr = strtok(nullptr, ",");
            }

            uint8_t newMode = newDataValues[0];
            if (newMode != ledMode && newMode >= 0 && newMode < LedMode::AMOUNT_LEDMODE) {
                changeMode(static_cast<LedMode>(newMode));
            }
            modeOption = newDataValues[1];

            uint8_t newBpm = newDataValues[2];
            if (newBpm != bpm && newBpm >= 60 && newBpm <= 180) {
                setBpm(newBpm);
            }

            //brightness
            brightness = newDataValues[3];
            FastLED.setBrightness(brightness);

            //intensity
            intensity = newDataValues[4];

            //h1
            hue1 = newDataValues[5];
            //h2
            hue2 = newDataValues[6];
            //colorMode
            colorMode = static_cast<ColorMode>(newDataValues[7]);

            uint8_t newPaletteIndex = newDataValues[8];
            if (newPaletteIndex != paletteIndex) {
                changePalette(newPaletteIndex);
            }

            if (newDataValues[9] > 0) {
                sendLedsSocket = true;
            } else {
                sendLedsSocket = false;
            }

            memcpy(oldDataValues, newDataValues, AMT_DATA_VALUES);
#ifdef DEBUG
            serialPrintResult();
#endif
            break;
        }
        case WStype_CONNECTED : {
            DEBUG_PRINTLN("client connected");
            char message[AMT_DATA_VALUES * 4];
            uint8_t message_length = 0;
            for (unsigned char dataValue : oldDataValues) {
                sprintf(message + message_length, "%03d,", dataValue);
                message_length += 4;
            }
            webSocket.sendTXT(num, message, message_length);
            break;
        }
        default:
            break;
    }
}

void setupLeds() {
    FastLED.addLeds<WS2815, D4, RGB>(leds, NUM_LEDS); // NOLINT(readability-static-accessed-through-instance)
    FastLED.setBrightness(brightness);
}

void setupServer() {
    WiFi.hostname("discoled");
    WiFi.mode(WIFI_STA);
    WiFi.config(ip, gateway, subnet);
    WiFi.begin("gratis wifi", "Fissakom.2020");

    while (!WiFi.isConnected()) {
        DEBUG_PRINTLN("not connected to wifi");
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
    }
    DEBUG_PRINTLN("connected");
    digitalWrite(LED_BUILTIN, LOW);
    DEBUG_PRINTLN(WiFi.localIP());


    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

#ifdef DEBUG
void serialPrintLeds() {
    for (auto &led : leds) {
        uint8_t v = rgb2hsv_approximate(led).v;
        if (v > 200U) {
            DEBUG_PRINT("O");
        } else if (v > 10U) {
            DEBUG_PRINT("-");
        } else {
            DEBUG_PRINT("_");
        }
    }
    DEBUG_PRINTLN();
}
#endif


void buildLedString(char *out) {
    uint16_t bufSize = 0;
    for (auto &led : leds) {
        sprintf(out + bufSize, "#%02X%02X%02X", led.r, led.g, led.b);
        bufSize += 7;
    }
}

#ifdef DEBUG_WEBSOCKET
void sendLedsToSocket() {
    if (sendLedsSocket) {
        char s[(NUM_LEDS * 7) + 1];
        buildLedString(s);
        webSocket.broadcastTXT(s);
    }
}
#endif


void setBpm(uint8_t newBpm) {
    bpm = newBpm;
    beatTicker.interval(15000 / newBpm);
//    timebase = beatTicker.elapsed();
    timebase = millis();
    colorIndex = 0;
}

void changePalette(uint8_t newPaletteIndex) {
    colorIndex = 0;
    paletteIndex = newPaletteIndex;
    switch (paletteIndex) {
        case 0: {
            currentPalette = Sunset_Real_gp;
            break;
        }
        case 1: {
            currentPalette = bhw1_sunconure_gp;
            break;
        }
        case 2:
            currentPalette = bhw1_purplered_gp;
            break;
        case 3:
            currentPalette = bhw1_28_gp;
            break;
        case 4:
            currentPalette = blues_gp;
            break;
        default:
            break;
    }
}

void setupOTA() {
    ArduinoOTA.setHostname("wemosDiscoled");
    ArduinoOTA.setPassword((const char *)"fissakom");
    ArduinoOTA.setPort(8266);
    ArduinoOTA.onStart([]() {
        DEBUG_PRINTLN("Start OTA");
    });
    ArduinoOTA.onEnd([]() {
        DEBUG_PRINTLN("\r\nEnd OTA");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) DEBUG_PRINTLN("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) DEBUG_PRINTLN("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) DEBUG_PRINTLN("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) DEBUG_PRINTLN("Receive Failed");
        else if (error == OTA_END_ERROR) DEBUG_PRINTLN("End Failed");
    });
    ArduinoOTA.begin(false);
    DEBUG_PRINTLN("OTA ready\r\n");
}
#ifdef DEBUG
void serialPrintResult() {
    if (!Serial) {
        return;
    }
    DEBUG_PRINT("mode: ");
    DEBUG_PRINTLN(ledMode);
    DEBUG_PRINT("modeOption: ");
    DEBUG_PRINTLN(modeOption);
    DEBUG_PRINT("bpm: ");
    DEBUG_PRINTLN(bpm);
    DEBUG_PRINT("brightness: ");
    DEBUG_PRINTLN(brightness);
    DEBUG_PRINT("intensity: ");
    DEBUG_PRINTLN(intensity);

    DEBUG_PRINT("h1: ");
    DEBUG_PRINTLN(hue1);
    DEBUG_PRINT("h2: ");
    DEBUG_PRINTLN(hue2);
    DEBUG_PRINT("colorMode:");
    DEBUG_PRINTLN(colorMode);

}
#endif