#include <Arduino.h>

#define DEBUG 1
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
#include "BeatMode.h"

#define NUM_LEDS    20
CRGB leds[NUM_LEDS];

RainMode rainMode(NUM_LEDS);
StrobeMode strobeMode(NUM_LEDS);
CylonMode cylonMode(NUM_LEDS);
SparkleMode sparkleMode(NUM_LEDS);
FlashMode flashMode(NUM_LEDS);
BeatMode beatMode(NUM_LEDS);


#include "ColorPalettes.h"

CRGBPalette16 currentPalette = Sunset_Real_gp;
TBlendType currentBlending = LINEARBLEND; // NOBLEND or LINEARBLEND


enum LedMode {
    Rain, Strobe, Cylon, Sparkle, Flash, Beat, Wash, Wave, AMOUNT_LEDMODE
};

enum ColorMode {
    Single, Complement, RainbowFade, RainbowSplash, Duo, Palette, Close, AMOUNT_COLORMODE
};

//<editor-fold desc="Declarations">
// WS2815 - 220ns, 360ns, 220ns https://github.com/FastLED/FastLED/issues/940
template<uint8_t DATA_PIN = D4, EOrder RGB_ORDER = RGB>
class WS2815Controller : public ClocklessController<DATA_PIN, C_NS(220), C_NS(360), C_NS(220), RGB_ORDER> {
};

template<uint8_t DATA_PIN, EOrder RGB_ORDER>
class WS2815 : public WS2815Controller<DATA_PIN, RGB_ORDER> {
};

void tickBeatClock();

void tickFastClock();

void serialPrintLeds();

void serialPrintResult();

void setupLeds();

void setupServer();

void changeMode(LedMode newMode);

void setBpm(int16_t bpm);

void sendLedsToSocket();

void updateLeds();

IPAddress ip(192, 168, 178, 200);
IPAddress gateway(192, 168, 178, 1);
IPAddress subnet(255, 255, 255, 0);
WebSocketsServer webSocket = WebSocketsServer(80);
//</editor-fold>



#define AMT_DATA_VALUES 8
// mode, modeOption, bpm, brightness, intensity, colorH1, colorH2, colorMode
uint8_t dataValues[AMT_DATA_VALUES] = {LedMode::Flash, 0, 120, 255, 0, 255, 255, ColorMode::Close};
uint8_t usedHue = dataValues[5];

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


//timing
byte ticks = 0;
#define TICKS_PER_BEAT 4
Ticker beatTicker(tickBeatClock, 1000, 0, MICROS);
Ticker fastTicker(tickFastClock, 10, 0, MILLIS);

uint8_t colorIndex = 0;

void updateColor() {

    colorIndex++;
    switch (static_cast<ColorMode>(getColorMode())) {
        case Complement: {
            int16_t h = getHue1();
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
                if (usedHue == getHue1()) {
                    usedHue = getHue2();
                } else {
                    usedHue = getHue1();
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
                if (usedHue == getHue1()) {
                    usedHue += 10;
                } else {
                    usedHue = getHue1();
                }
            }
            break;
        }
        default:
            break;
    }
}

boolean isLedModeBeatMatched(uint8_t mode) {
    return mode < LedMode::Wash;
}

void tickBeatClock() {
    ticks = addmod8(ticks, 1, TICKS_PER_BEAT);
    int16_t intensity = getIntensity();
    if ((intensity == 0 && ticks == 0) ||
        (intensity == 1 && ticks % 2 == 0) ||
        intensity >= 2) {
        updateColor();
        updateLeds();
    }
}

void tickFastClock() {
    updateLeds();
}

void setup() {
    Serial.begin(115200);

    setupLeds();
    pinMode(LED_BUILTIN, OUTPUT);

    setupServer();
//    setupOTA();

    changeMode(LedMode::Rain);
    setBpm(getBpm());
    beatTicker.start();
}


void changeMode(LedMode newMode) {
    int16_t intensity = getIntensity();
    auto oldMode = static_cast<LedMode>(dataValues[0]);
    dataValues[0] = newMode;
    uint8_t h = usedHue;
    uint8_t bpm = getBpm();
    uint8_t colorMode = getColorMode();
    switch (newMode) {
        case LedMode::Rain: {
            rainMode.init(leds, intensity, h, bpm, colorMode);
            break;
        }
        case LedMode::Strobe: {
            strobeMode.init(leds, intensity, h, bpm, colorMode);
            break;
        }
        case LedMode::Cylon: {
            cylonMode.init(leds, intensity, h, bpm, colorMode);
            break;
        }
        case LedMode::Sparkle: {
            sparkleMode.init(leds, intensity, h, bpm, colorMode);
            break;
        }
        case LedMode::Flash: {
            flashMode.init(leds, intensity, h, bpm, colorMode);
            break;
        }
        case LedMode::Beat: {
            beatMode.init(leds, intensity, h, bpm, colorMode);
            break;
        }
        default:
            DEBUG_PRINT("change into unkown mode option: ");
            DEBUG_PRINTLN(newMode);
            break;
    }
    if (isLedModeBeatMatched(newMode) && !isLedModeBeatMatched(oldMode)) {
        fastTicker.stop();
        beatTicker.start();
    } else if (!isLedModeBeatMatched(newMode) && isLedModeBeatMatched(oldMode)){
        beatTicker.stop();
        fastTicker.start();
    }
}

void loop() {
//    ArduinoOTA.handle();
    webSocket.loop();
    beatTicker.update();
    fastTicker.update();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_TEXT: {
            Serial.printf("[%u] get Text: %s\n", num, payload);
            size_t index = 0;
            int16_t newDataValues[AMT_DATA_VALUES];

            char *ptr = strtok((char *) payload, ",");

            while (ptr != nullptr && index < AMT_DATA_VALUES) {
                newDataValues[index++] = atoi(ptr);
                ptr = strtok(nullptr, ",");
            }

            int16_t newMode = newDataValues[0];
            if (newMode != getLedMode() && newMode >= 0 && newMode < LedMode::AMOUNT_LEDMODE) {
                changeMode(static_cast<LedMode>(newMode));
            }
            dataValues[1] = newDataValues[1];

            int16_t newBpm = newDataValues[2];
            if (newBpm != getBpm() && newBpm >= 60 && newBpm <= 180) {
                setBpm(newBpm);
            }

            //brightness
            dataValues[3] = newDataValues[3];
            FastLED.setBrightness(dataValues[3]);

            //intensity
            dataValues[4] = newDataValues[4];

            //h1
            dataValues[5] = newDataValues[5];
            //h2
            dataValues[6] = newDataValues[6];
            //colorMode
            dataValues[7] = newDataValues[7];

            serialPrintResult();
            break;
        }
        case WStype_CONNECTED : {
            DEBUG_PRINTLN("client connected");
            char message[sizeof(dataValues) * 4];
            uint8_t message_length = 0;
            for (unsigned char dataValue : dataValues) {
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
    FastLED.addLeds<WS2815, D4, RGB>(leds, NUM_LEDS);
    FastLED.setBrightness(dataValues[3]);
}

void setupOTA() {
    ArduinoOTA.setHostname("discoled");
    ArduinoOTA.setPassword("123");
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
    ArduinoOTA.begin();
    DEBUG_PRINTLN("OTA ready\r\n");
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
    digitalWrite(LED_BUILTIN, HIGH);
    DEBUG_PRINTLN(WiFi.localIP());


    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

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


void buildLedString(char *out) {
    uint16_t bufSize = 0;
    for (auto &led : leds) {
        sprintf(out + bufSize, "#%02X%02X%02X", led.r, led.g, led.b);
        bufSize += 7;
    }
}

void sendLedsToSocket() {
    char s[(NUM_LEDS * 7) + 1];
    buildLedString(s);
    webSocket.broadcastTXT(s);
}


void setBpm(int16_t newBpm) {
    dataValues[2] = static_cast<int16_t>(newBpm);
    beatTicker.interval(15000 / newBpm);
}

void updateLeds() {
    int16_t modeOption = getModeOption();
    int16_t intensity = getIntensity();
    uint8_t h = usedHue;
    uint8_t bpm = getBpm();
    uint8_t colorMode = getColorMode();
    auto mode = static_cast<LedMode>(dataValues[0]);
    switch (mode) {
        case LedMode::Rain: {
            rainMode.update(leds, intensity, modeOption, h, bpm, colorMode);
            break;
        }
        case LedMode::Strobe: {
            strobeMode.update(leds, intensity, modeOption, h, bpm, colorMode);
            break;
        }
        case LedMode::Sparkle: {
            sparkleMode.update(leds, intensity, modeOption, h, bpm, colorMode);
            break;
        }
        case LedMode::Cylon: {
            cylonMode.update(leds, intensity, modeOption, h, bpm, colorMode);
            break;
        }
        case LedMode::Flash: {
            flashMode.update(leds, intensity, modeOption, h, bpm, colorMode);
            break;
        }
        case LedMode::Beat: {
            beatMode.update(leds, intensity, modeOption, h, bpm, colorMode);
        }
        default:
            DEBUG_PRINT("unkown mode option: ");
            DEBUG_PRINTLN(mode);
            break;
    }
    serialPrintLeds();
    sendLedsToSocket();
    FastLED.show();
}

void serialPrintResult() {
    if (!Serial) {
        return;
    }
    DEBUG_PRINT("mode: ");
    DEBUG_PRINTLN(getModeOption());
    DEBUG_PRINT("modeOption: ");
    DEBUG_PRINTLN(getModeOption());
    DEBUG_PRINT("bpm: ");
    DEBUG_PRINTLN(getBpm());
    DEBUG_PRINT("brightness: ");
    DEBUG_PRINTLN(getBrightness());
    DEBUG_PRINT("intensity: ");
    DEBUG_PRINTLN(getIntensity());

    DEBUG_PRINT("h1: ");
    DEBUG_PRINTLN(getHue1());
    DEBUG_PRINT("h2: ");
    DEBUG_PRINTLN(getHue2());
    DEBUG_PRINT("colorMode:");
    DEBUG_PRINTLN(getColorMode());

}
