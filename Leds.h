#ifndef LEDS_H
#define LEDS_h

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

//Constants
#define DS3231_I2C_ADDRESS 0x68
#define PIXEL_PIN 23
#define PIXEL_COUNT 50


#define VAGUE_LENGHT 7
const uint8_t vague_r[VAGUE_LENGHT] = { 250, 200, 150, 0, 150, 200, 250 };

//Leds function
void Strip_init();

boolean arrayIncludeElement(int array[], int element);

void setBrightness(uint8_t brightness);

uint32_t Wheel(byte WheelPos);

void setColor(int r, int g, int b);

void random_on(int delai);
void random_off(int delai);

void off_start(int delai);
void off_end(int delai);
void off_extremity(int delai);

void led_run(int r, int g, int b, int delai);
void fill_(int r, int g, int b, int delai);
void wave(int r, int g, int b, int delai);
void fill_middle(int r, int g, int b, int delai);
void fill_extremity(int r, int g, int b, int delai);
void circus(int r1, int g1, int b1, int r2, int g2, int b2, int delai);

void rainbow(int delai);

void setKelvin(int kelvin, int dispersion);
#endif