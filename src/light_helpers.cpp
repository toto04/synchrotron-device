#include "light_helpers.h"

CRGB leds[TOTAL_LENGTH];

/** 
 * Initializes the FastLED istance, adding all the strips with correct offsets
*/
void initializeLEDStrips() {
    FastLED.addLeds<NEOPIXEL, STRIP1_PIN>(leds, 0, STRIP1_LENGTH)
        .setCorrection(TypicalLEDStrip);
    // i dont know c++, that's the best i can come up with :c
#if defined(STRIP2_LENGTH) && defined(STRIP2_PIN)
    FastLED.addLeds<NEOPIXEL, STRIP2_PIN>(leds, STRIP1_LENGTH, STRIP2_LENGTH)
        .setCorrection(TypicalLEDStrip);
#if defined(STRIP3_LENGTH) && defined(STRIP3_PIN)
    FastLED.addLeds<NEOPIXEL, STRIP3_PIN>(leds, STRIP2_LENGTH, STRIP3_LENGTH)
        .setCorrection(TypicalLEDStrip);
#if defined(STRIP4_LENGTH) && defined(STRIP4_PIN)
    FastLED.addLeds<NEOPIXEL, STRIP4_PIN>(leds, STRIP3_LENGTH, STRIP4_LENGTH)
        .setCorrection(TypicalLEDStrip);
#endif
#endif
#endif
    FastLED.clear();
}

void flash(uint8_t hue, uint8_t sat, uint8_t maxbri, uint16_t length) {
    uint8_t i = 0;
    while (i < maxbri) {
        for (uint16_t j = 0; j < length; j++) leds[j] = CHSV(hue, sat, i);
        FastLED.show();
        delay(2);
        i++;
    }
    while (i > 0) {
        for (uint16_t j = 0; j < length; j++) leds[j] = CHSV(hue, sat, i);
        FastLED.show();
        delay(2);
        i--;
    }
}

void showCompletion(double percentage) {
    double complete, last;
    last = modf(percentage * TOTAL_LENGTH, &complete);
    int i = 0;
    while (i < complete) {
        leds[i] = CHSV(139, 205, 100);
        i++;
    }
    leds[i] = CHSV(139, 205, int(last * 100));
    FastLED.show();
}