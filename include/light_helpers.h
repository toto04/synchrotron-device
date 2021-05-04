#ifndef __LIGHT_HELP_H
#define __LIGHT_HELP_H
#include <Arduino.h>
#include <FastLED.h>

#include "config.h"

// defines the total number of pixels adding each strip
// i dont know c++, that's the best i can come up with :c
#if defined(STRIP4_LENGTH) && defined(STRIP4_PIN)
#define TOTAL_LENGTH STRIP1_LENGTH + STRIP2_LENGTH + STRIP3_LENGTH + STRIP4_LENGTH
#elif defined(STRIP3_LENGTH) && defined(STRIP3_PIN)
#define TOTAL_LENGTH STRIP1_LENGTH + STRIP2_LENGTH + STRIP3_LENGTH
#elif defined(STRIP2_LENGTH) && defined(STRIP2_PIN)
#define TOTAL_LENGTH STRIP1_LENGTH + STRIP2_LENGTH
#else
#define TOTAL_LENGTH STRIP1_LENGTH
#endif

extern CRGB leds[TOTAL_LENGTH];

void initializeLEDStrips();

void flash(uint8_t hue, uint8_t sat, uint8_t maxbri, uint16_t length);
void showCompletion(double percentage);
#endif