#include "Leds.h"

//Declare the leds
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_RGB + NEO_KHZ800);


boolean arrayIncludeElement(int array[], int element) {
  if (array[element] == 1) {
    return true;
  }
  return false;
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


void off_start(int delai) {
  for (int i = 0; i <= PIXEL_COUNT - 1; i++) {
    strip.setPixelColor(i, 0, 0, 0);
    strip.show();
    delay(delai);
  }
}

void off_end(int delai) {
  for (int i = PIXEL_COUNT - 1; i >= 0; i--) {
    strip.setPixelColor(i, 0, 0, 0);
    strip.show();
    delay(delai);
  }
}

void off_extremity(int delai) {
  int k = PIXEL_COUNT;
  for (int i = 0; i <= PIXEL_COUNT / 2; i++) {
    Serial.println(k);
    strip.setPixelColor(i, 0);
    strip.setPixelColor(k, 0);
    strip.show();
    k--;
    delay(delai);
  }
}

int k = 0;
void led_run(int r, int g, int b, int delai) {
  int r2, g2, b2;

  if (k <= 0) {
    k = PIXEL_COUNT - 1;
  }

  if (r == 0 && g == 0 && b == 0) {
    r2 = random(0, 255);
    g2 = random(0, 255);
    b2 = random(0, 255);
  } else {
    r2 = r;
    g2 = g;
    b2 = b;
  }

  Serial.println(k);
  for (int i = 0; i <= k; i++) {
    strip.setPixelColor(i, r2, g2, b2);
    strip.setPixelColor(i - 1, 0, 0, 0);
    strip.show();
    delay(delai);
  }
  k--;
}

void fill_(int r, int g, int b, int delai) {
  int r2, g2, b2;

  for (int i = 0; i <= PIXEL_COUNT - 1; i++) {
    if (r == 0 & g == 0 & b == 0) {
      r2 = random(0, 255);
      g2 = random(0, 255);
      b2 = random(0, 255);
    } else {
      r2 = r;
      g2 = g;
      b2 = b;
    }
    strip.setPixelColor(i, r2, g2, b2);
    strip.show();
    delay(delai);
  }
}

void wave(int r, int g, int b, int delai) {
  for (int i = 0; i <= PIXEL_COUNT; i++) {
    for (int k = 0; k < VAGUE_LENGHT; k++) {
      strip.setPixelColor(i - k, r != 0 ? r - vague_r[k] : 0, g != 0 ? r - vague_r[k] : 0, b != 0 ? r - vague_r[k] : 0);
      strip.setPixelColor(i - (VAGUE_LENGHT + 1), 0, 0, 0);
    }
    strip.show();
    delay(delai);
  }
}

void fill_middle(int r, int g, int b, int delai) {
  int k = PIXEL_COUNT / 2;
  for (int i = PIXEL_COUNT / 2; i >= 0; i--) {
    Serial.println(k);
    strip.setPixelColor(i, r, g, b);
    strip.setPixelColor(k, r, g, b);
    strip.show();
    k++;
    delay(delai);
  }
}

void fill_extremity(int r, int g, int b, int delai) {
  int k = 0;
  for (int i = PIXEL_COUNT; i >= PIXEL_COUNT / 2; i--) {
    Serial.println(k);
    strip.setPixelColor(i, r, g, b);
    strip.setPixelColor(k, r, g, b);
    strip.show();
    k++;
    delay(delai);
  }
}

void circus(int r1, int g1, int b1, int r2, int g2, int b2, int delai) {
  for (int k = 0; k <= 3; k++) {
    for (int i = 0; i <= PIXEL_COUNT - 1; i++) {
      if (i & 1 == 1) {
        strip.setPixelColor(i, k & 1 == 1 ? r1 : r2, k & 1 == 1 ? g1 : g2, k & 1 == 1 ? b1 : b2);
      } else {
        strip.setPixelColor(i, k & 1 == 1 ? r2 : r1, k & 1 == 1 ? g2 : g1, k & 1 == 1 ? b2 : b1);
      }
      strip.show();
    }
    delay(delai);
  }
}

void random_on(int delai) {
  int rand_[PIXEL_COUNT] = {};
  int randnum = random(0, PIXEL_COUNT);
  for (int i = 0; i < PIXEL_COUNT; i++) {
    while (arrayIncludeElement(rand_, randnum)) randnum = random(0, PIXEL_COUNT);
    rand_[randnum] = 1;
    Serial.println(i);
    strip.setPixelColor(randnum, random(0, 255), random(0, 255), random(0, 255));
    strip.show();
    delay(delai);
  }
  Serial.println("exit");
}

void random_off(int delai) {
  int rand2_[PIXEL_COUNT] = {};
  int randnum = random(0, PIXEL_COUNT);
  Serial.println("test");
  for (int i = 0; i < PIXEL_COUNT; i++) {
    while (arrayIncludeElement(rand2_, randnum)) randnum = random(0, PIXEL_COUNT);

    rand2_[randnum] = 1;
    strip.setPixelColor(randnum, 0, 0, 0);
    strip.show();
    delay(delai);
  }
}

int w;
void rainbow() {
  if (w == 255) {
    w = 0;
  }
  for (int l = 0; l < PIXEL_COUNT; l++) {
    strip.setPixelColor(l, Wheel((w + l * 5) % 255));
  }
  strip.show();
  w++;
  delay(2);
}

void setColor(int r, int g, int b) {
  for (int o = 0; o < PIXEL_COUNT; o++) {
    strip.setPixelColor(o, r, g, b);
  }
  strip.show();
}