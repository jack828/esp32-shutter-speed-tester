#pragma once

#include <Arduino.h>
#include <SPI.h>

template <const uint8_t DC_PIN, const int8_t CS_PIN, const int8_t RST_PIN,
          SPIClass &_SPI = SPI>
class ST7735S {
public:
  static const uint16_t BLACK = 0x0000;
  static const uint16_t WHITE = 0xFFFF;
  static const uint16_t GRAY = 0x38E7;
  static const uint16_t RED = 0xF800;
  static const uint16_t GREEN = 0x07E0;
  static const uint16_t BLUE = 0x001F;
  static const uint16_t YELLOW = 0xFFE0;

  void begin();
  void reset();

  uint8_t width() const { return 160; }
  uint8_t height() const { return 80; }

  uint16_t rgb(uint8_t r, uint8_t g, uint8_t b) const;
  void fill(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t c);
  void clear() { fill(0, 0, width(), height(), 0); }
  void pixel(uint8_t x, uint8_t y, uint16_t c) { fill(x, y, 1, 1, c); }
  void draw(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint16_t *data);
  void draw(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bits,
            uint16_t cf, uint16_t cb = BLACK);

  uint8_t charWidth() const { return FONT_WIDTH + FONT_GAP; }
  uint8_t charHeight() const { return FONT_HEIGHT; }

  void print(uint8_t x, uint8_t y, char c, uint16_t cf, uint16_t cb = BLACK);
  void print(uint8_t x, uint8_t y, const char *str, uint16_t cf,
             uint16_t cb = BLACK);

  void flip(bool on);
  void inverse(bool on);
  void power(bool on);

protected:
  static const uint8_t FONT_WIDTH = 7;
  static const uint8_t FONT_HEIGHT = 16;
  static const uint8_t FONT_GAP = 1;

  void sendStart();
  void sendEnd();
  void sendCmd(uint8_t cmd);
  void sendCmd(uint8_t cmd, uint8_t arg);
  void sendCmd(uint8_t cmd, const uint8_t *args, uint16_t count);
  void sendData(const uint8_t *data, uint16_t count);
  void sendData(const uint16_t *data, uint16_t count);
  void sendData(uint8_t data, uint16_t count);
  void sendData(uint16_t data, uint16_t count);

  void select(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
};

template <const uint8_t DC_PIN, const int8_t CS_PIN, const int8_t RST_PIN,
          SPIClass &_SPI>
void ST7735S<DC_PIN, CS_PIN, RST_PIN, _SPI>::begin() {
  if (CS_PIN >= 0) {
    pinMode(CS_PIN, OUTPUT);
    digitalWrite(CS_PIN, HIGH);
  }
  pinMode(DC_PIN, OUTPUT);
  if (RST_PIN >= 0) {
    pinMode(RST_PIN, OUTPUT);
  }

  _SPI.begin();

  reset();
}

template <const uint8_t DC_PIN, const int8_t CS_PIN, const int8_t RST_PIN,
          SPIClass &_SPI>
void ST7735S<DC_PIN, CS_PIN, RST_PIN, _SPI>::reset() {
  if (RST_PIN >= 0) {
    digitalWrite(RST_PIN, LOW);
    delay(100);
    digitalWrite(RST_PIN, HIGH);
    delay(120);
  } else {
    sendCmd(0x01); // SWRST
    delay(120);
  }

  sendCmd(0x11); // SLPOUT
  delay(120);

  sendCmd(0x36, 0x68); // MADCTL
  sendCmd(0x3A, 5);    // COLMOD (16 bit)
  sendCmd(0x13);       // NORON

  clear();

  sendCmd(0x29); // DISPON
}

template <const uint8_t DC_PIN, const int8_t CS_PIN, const int8_t RST_PIN,
          SPIClass &_SPI>
uint16_t ST7735S<DC_PIN, CS_PIN, RST_PIN, _SPI>::rgb(uint8_t r, uint8_t g,
                                                     uint8_t b) const {
  uint16_t result;

  result = (r >> 3) << 11;
  result |= (g >> 2) << 5;
  result |= (b >> 3);
  return result;
}

template <const uint8_t DC_PIN, const int8_t CS_PIN, const int8_t RST_PIN,
          SPIClass &_SPI>
void ST7735S<DC_PIN, CS_PIN, RST_PIN, _SPI>::fill(uint8_t x, uint8_t y,
                                                  uint8_t w, uint8_t h,
                                                  uint16_t c) {
  if ((x < width()) && (y < height())) {
    if (x + w > width())
      w = width() - x;
    if (y + h > height())
      h = height() - y;
    select(x, y, w, h);
    sendData(c, w * h);
  }
}

template <const uint8_t DC_PIN, const int8_t CS_PIN, const int8_t RST_PIN,
          SPIClass &_SPI>
void ST7735S<DC_PIN, CS_PIN, RST_PIN, _SPI>::draw(uint8_t x, uint8_t y,
                                                  uint8_t w, uint8_t h,
                                                  const uint16_t *data) {
  if ((x < width()) && (y < height())) {
    uint8_t _w, _h;

    if (x + w > width())
      _w = width() - x;
    else
      _w = w;
    if (y + h > height())
      _h = height() - y;
    else
      _h = h;
    select(x, y, _w, _h);
    while (_h--) {
      sendData(data, _w);
      data += w;
    }
  }
}

template <const uint8_t DC_PIN, const int8_t CS_PIN, const int8_t RST_PIN,
          SPIClass &_SPI>
void ST7735S<DC_PIN, CS_PIN, RST_PIN, _SPI>::draw(uint8_t x, uint8_t y,
                                                  uint8_t w, uint8_t h,
                                                  const uint8_t *bits,
                                                  uint16_t cf, uint16_t cb) {
  if ((x < width()) && (y < height())) {
    uint16_t line[w];
    uint8_t _w, _h;

    if (x + w > width())
      _w = width() - x;
    else
      _w = w;
    if (y + h > height())
      _h = height() - y;
    else
      _h = h;
    for (uint8_t _y = 0; _y < _h; ++_y) {
      for (uint8_t _x = 0; _x < _w; ++_x) {
        line[_x] = (bits[(_y / 8) * w + _x] & (1 << (_y % 8))) ? cf : cb;
      }
      select(x, y + _y, _w, 1);
      sendData(line, _w);
    }
  }
}

template <const uint8_t DC_PIN, const int8_t CS_PIN, const int8_t RST_PIN,
          SPIClass &_SPI>
void ST7735S<DC_PIN, CS_PIN, RST_PIN, _SPI>::print(uint8_t x, uint8_t y, char c,
                                                   uint16_t cf, uint16_t cb) {
  static const uint8_t FONT[] = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x67, 0x67, 0x00, 0x00, 0x00, 0x3E, 0x0E, 0x00, 0x3E, 0x0E, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x20, 0xFE, 0x20, 0x20, 0xFE,
      0x20, 0x04, 0x7F, 0x04, 0x04, 0x7F, 0x04, 0x04, 0x02, 0x0C, 0x30, 0xC0,
      0x30, 0x0C, 0x02, 0x02, 0x02, 0x02, 0x7F, 0x02, 0x02, 0x02, 0x3C, 0x42,
      0x42, 0xBC, 0x60, 0x18, 0x06, 0x60, 0x18, 0x06, 0x3D, 0x42, 0x42, 0x3C,
      0x00, 0x3C, 0xC2, 0x22, 0x1C, 0x00, 0x00, 0x1C, 0x23, 0x40, 0x47, 0x28,
      0x30, 0x4E, 0x00, 0x00, 0x1C, 0xFE, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x18, 0x04, 0x02, 0x00, 0x00,
      0x00, 0x07, 0x18, 0x20, 0x40, 0x00, 0x00, 0x02, 0x04, 0x18, 0xE0, 0x00,
      0x00, 0x00, 0x40, 0x20, 0x18, 0x07, 0x00, 0x00, 0x40, 0x80, 0x00, 0xF8,
      0x00, 0x80, 0x40, 0x10, 0x0C, 0x03, 0x01, 0x03, 0x0C, 0x10, 0x80, 0x80,
      0x80, 0xF8, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x58, 0x38, 0x00,
      0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x60, 0x18,
      0x06, 0x60, 0x18, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x02, 0x01,
      0x01, 0x02, 0xFC, 0x00, 0x03, 0x04, 0x08, 0x08, 0x04, 0x03, 0x00, 0x00,
      0x04, 0x02, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
      0x04, 0x02, 0x01, 0x81, 0x41, 0x22, 0x1C, 0x0C, 0x0A, 0x09, 0x08, 0x08,
      0x08, 0x08, 0x01, 0x11, 0x19, 0x25, 0x23, 0xC1, 0x00, 0x04, 0x08, 0x08,
      0x08, 0x04, 0x03, 0x00, 0x80, 0x60, 0x18, 0x06, 0xF1, 0x00, 0x00, 0x01,
      0x01, 0x01, 0x01, 0x0F, 0x01, 0x01, 0x00, 0x1F, 0x11, 0x11, 0x11, 0x21,
      0xC1, 0x00, 0x04, 0x08, 0x08, 0x08, 0x04, 0x03, 0xE0, 0x58, 0x24, 0x22,
      0x21, 0x40, 0x80, 0x03, 0x04, 0x08, 0x08, 0x08, 0x04, 0x03, 0x01, 0x01,
      0x01, 0x81, 0x71, 0x0D, 0x03, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00,
      0x8C, 0x52, 0x21, 0x21, 0x21, 0x52, 0x8C, 0x03, 0x04, 0x08, 0x08, 0x08,
      0x04, 0x03, 0x1C, 0x22, 0x41, 0x41, 0x41, 0xA2, 0x7C, 0x00, 0x00, 0x08,
      0x04, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x33, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0xB3, 0x73, 0x00, 0x00, 0x00, 0x80, 0x80, 0x40, 0x40,
      0x20, 0x20, 0x10, 0x00, 0x00, 0x01, 0x01, 0x02, 0x02, 0x04, 0x00, 0x40,
      0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
      0x10, 0x20, 0x20, 0x40, 0x40, 0x80, 0x80, 0x04, 0x02, 0x02, 0x01, 0x01,
      0x00, 0x00, 0x18, 0x04, 0x02, 0x82, 0x82, 0x44, 0x38, 0x00, 0x00, 0x67,
      0x67, 0x00, 0x00, 0x00, 0xC4, 0x22, 0x22, 0xC2, 0x02, 0x04, 0xF8, 0x1F,
      0x20, 0x20, 0x1F, 0x20, 0x20, 0x1F, 0x00, 0xE0, 0x9C, 0x82, 0x9C, 0xE0,
      0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0xFE, 0x42, 0x42,
      0x42, 0xA2, 0x1C, 0x00, 0x0F, 0x08, 0x08, 0x08, 0x08, 0x07, 0xF8, 0x04,
      0x02, 0x02, 0x02, 0x04, 0x08, 0x03, 0x04, 0x08, 0x08, 0x08, 0x04, 0x02,
      0x00, 0xFE, 0x02, 0x02, 0x02, 0x04, 0xF8, 0x00, 0x0F, 0x08, 0x08, 0x08,
      0x04, 0x03, 0x00, 0xFE, 0x42, 0x42, 0x42, 0x42, 0x02, 0x00, 0x0F, 0x08,
      0x08, 0x08, 0x08, 0x08, 0x00, 0xFE, 0x22, 0x22, 0x22, 0x22, 0x02, 0x00,
      0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x04, 0x02, 0x02, 0x42, 0x44,
      0xC8, 0x03, 0x04, 0x08, 0x08, 0x08, 0x04, 0x0F, 0x00, 0xFE, 0x40, 0x40,
      0x40, 0x40, 0xFE, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x02,
      0x02, 0xFE, 0x02, 0x02, 0x00, 0x00, 0x08, 0x08, 0x0F, 0x08, 0x08, 0x00,
      0x00, 0x00, 0x02, 0x02, 0xFE, 0x02, 0x02, 0x06, 0x08, 0x08, 0x08, 0x07,
      0x00, 0x00, 0x00, 0xFE, 0x40, 0xB0, 0x0C, 0x02, 0x00, 0x00, 0x0F, 0x00,
      0x01, 0x06, 0x08, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
      0x0F, 0x08, 0x08, 0x08, 0x0E, 0x00, 0xFE, 0x0C, 0x70, 0x80, 0x70, 0x0C,
      0xFE, 0x0F, 0x00, 0x00, 0x01, 0x00, 0x00, 0x0F, 0x00, 0xFE, 0x0C, 0x30,
      0xC0, 0x00, 0xFE, 0x00, 0x0F, 0x00, 0x00, 0x01, 0x06, 0x0F, 0xF8, 0x04,
      0x02, 0x02, 0x02, 0x04, 0xF8, 0x03, 0x04, 0x08, 0x08, 0x08, 0x04, 0x03,
      0x00, 0xFE, 0x42, 0x42, 0x42, 0x24, 0x18, 0x00, 0x0F, 0x00, 0x00, 0x00,
      0x00, 0x00, 0xF8, 0x04, 0x02, 0x02, 0x02, 0x04, 0xF8, 0x03, 0x04, 0x08,
      0x09, 0x0A, 0x04, 0x0B, 0x00, 0xFE, 0x42, 0xC2, 0x42, 0x24, 0x18, 0x00,
      0x0F, 0x00, 0x00, 0x03, 0x04, 0x08, 0x18, 0x24, 0x42, 0x42, 0x82, 0x84,
      0x08, 0x02, 0x04, 0x08, 0x08, 0x08, 0x04, 0x03, 0x02, 0x02, 0x02, 0xFE,
      0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0xFE, 0x00,
      0x00, 0x00, 0x00, 0x00, 0xFE, 0x03, 0x04, 0x08, 0x08, 0x08, 0x04, 0x03,
      0x0E, 0x70, 0x80, 0x00, 0x80, 0x70, 0x0E, 0x00, 0x00, 0x03, 0x0C, 0x03,
      0x00, 0x00, 0xFE, 0x00, 0xC0, 0x3C, 0xC0, 0x00, 0xFE, 0x0F, 0x06, 0x01,
      0x00, 0x01, 0x06, 0x0F, 0x02, 0x0C, 0xB0, 0x40, 0xB0, 0x0C, 0x02, 0x08,
      0x06, 0x01, 0x00, 0x01, 0x06, 0x08, 0x06, 0x18, 0x60, 0x80, 0x60, 0x18,
      0x06, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0xC2,
      0x22, 0x1A, 0x06, 0x08, 0x0C, 0x0B, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,
      0x00, 0xFE, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x40, 0x40, 0x00,
      0x06, 0x18, 0x60, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x06,
      0x18, 0x60, 0x00, 0x02, 0x02, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x40, 0x40,
      0x7F, 0x00, 0x00, 0x00, 0x10, 0x08, 0x04, 0x02, 0x04, 0x08, 0x10, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x0E,
      0x30, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x90,
      0x90, 0x90, 0x90, 0x10, 0xE0, 0x07, 0x08, 0x08, 0x08, 0x08, 0x05, 0x0F,
      0x00, 0xFF, 0x20, 0x10, 0x10, 0x20, 0xC0, 0x00, 0x0F, 0x04, 0x08, 0x08,
      0x04, 0x03, 0xC0, 0x20, 0x10, 0x10, 0x10, 0x10, 0x20, 0x03, 0x04, 0x08,
      0x08, 0x08, 0x08, 0x04, 0xC0, 0x20, 0x10, 0x10, 0x20, 0xFF, 0x00, 0x03,
      0x04, 0x08, 0x08, 0x04, 0x0F, 0x00, 0xC0, 0xA0, 0x90, 0x90, 0x90, 0xA0,
      0xC0, 0x03, 0x04, 0x08, 0x08, 0x08, 0x08, 0x04, 0x00, 0x40, 0x40, 0xFC,
      0x42, 0x42, 0x0C, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00, 0xE0,
      0x10, 0x10, 0x20, 0xF0, 0x00, 0x00, 0x23, 0x44, 0x44, 0x42, 0x3F, 0x00,
      0x00, 0xFF, 0x20, 0x10, 0x10, 0x10, 0xE0, 0x00, 0x0F, 0x00, 0x00, 0x00,
      0x00, 0x0F, 0x00, 0x00, 0x00, 0xF3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF3, 0x00, 0x00, 0x00,
      0x20, 0x40, 0x40, 0x3F, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x80, 0x40, 0x20,
      0x00, 0x00, 0x0F, 0x01, 0x02, 0x04, 0x08, 0x00, 0x00, 0x00, 0xFF, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x08, 0x08, 0x04, 0x00, 0xF0, 0x20,
      0x10, 0xE0, 0x10, 0x10, 0xE0, 0x0F, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x0F,
      0x00, 0xF0, 0x20, 0x10, 0x10, 0x10, 0xE0, 0x00, 0x0F, 0x00, 0x00, 0x00,
      0x00, 0x0F, 0xC0, 0x20, 0x10, 0x10, 0x10, 0x20, 0xC0, 0x03, 0x04, 0x08,
      0x08, 0x08, 0x04, 0x03, 0x00, 0xF0, 0x20, 0x10, 0x10, 0x20, 0xC0, 0x00,
      0x7F, 0x04, 0x08, 0x08, 0x04, 0x03, 0xC0, 0x20, 0x10, 0x10, 0x20, 0xF0,
      0x00, 0x03, 0x04, 0x08, 0x08, 0x04, 0x7F, 0x00, 0x00, 0x00, 0x10, 0xE0,
      0x10, 0x10, 0x20, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x60,
      0x90, 0x90, 0x10, 0x10, 0x20, 0x00, 0x04, 0x08, 0x08, 0x09, 0x09, 0x06,
      0x00, 0x10, 0x10, 0xFE, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x07, 0x08,
      0x08, 0x04, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x07, 0x08,
      0x08, 0x08, 0x04, 0x0F, 0x30, 0xC0, 0x00, 0x00, 0x00, 0xC0, 0x30, 0x00,
      0x00, 0x03, 0x0C, 0x03, 0x00, 0x00, 0xF0, 0x00, 0x00, 0xE0, 0x00, 0x00,
      0xF0, 0x03, 0x0C, 0x03, 0x00, 0x03, 0x0C, 0x03, 0x10, 0x20, 0x40, 0x80,
      0x40, 0x20, 0x10, 0x04, 0x02, 0x01, 0x00, 0x01, 0x02, 0x04, 0x30, 0xC0,
      0x00, 0x00, 0x00, 0xC0, 0x30, 0x20, 0x10, 0x0B, 0x04, 0x03, 0x00, 0x00,
      0x10, 0x10, 0x10, 0x10, 0x90, 0x50, 0x30, 0x08, 0x0C, 0x0A, 0x09, 0x08,
      0x08, 0x08, 0x00, 0x00, 0x80, 0x40, 0x3E, 0x01, 0x00, 0x00, 0x00, 0x00,
      0x01, 0x3E, 0x40, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3E, 0x40, 0x80, 0x00,
      0x00, 0x00, 0x40, 0x3E, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
      0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  draw(x, y, FONT_WIDTH, FONT_HEIGHT,
       &FONT[(c - ' ') * FONT_WIDTH * ((FONT_HEIGHT + 7) / 8)], cf, cb);
  fill(x + FONT_WIDTH, y, FONT_GAP, FONT_HEIGHT, cb);
}

template <const uint8_t DC_PIN, const int8_t CS_PIN, const int8_t RST_PIN,
          SPIClass &_SPI>
void ST7735S<DC_PIN, CS_PIN, RST_PIN, _SPI>::print(uint8_t x, uint8_t y,
                                                   const char *str, uint16_t cf,
                                                   uint16_t cb) {
  while (*str && (x < width())) {
    print(x, y, *str++, cf, cb);
    x += FONT_WIDTH + FONT_GAP;
  }
}

template <const uint8_t DC_PIN, const int8_t CS_PIN, const int8_t RST_PIN,
          SPIClass &_SPI>
inline void ST7735S<DC_PIN, CS_PIN, RST_PIN, _SPI>::flip(bool on) {
  sendCmd(0x36, on ? 0xA8 : 0x68); // MADCTL
}

template <const uint8_t DC_PIN, const int8_t CS_PIN, const int8_t RST_PIN,
          SPIClass &_SPI>
inline void ST7735S<DC_PIN, CS_PIN, RST_PIN, _SPI>::inverse(bool on) {
  sendCmd(0x20 | on); // INVOFF/INVON
}

template <const uint8_t DC_PIN, const int8_t CS_PIN, const int8_t RST_PIN,
          SPIClass &_SPI>
void ST7735S<DC_PIN, CS_PIN, RST_PIN, _SPI>::power(bool on) {
  if (on) {
    sendCmd(0x11); // SLPOUT
    delay(120);
    sendCmd(0x29); // DISPON
  } else {
    sendCmd(0x28); // DISPOFF
    sendCmd(0x10); // SLPIN
    delay(120);
  }
}

template <const uint8_t DC_PIN, const int8_t CS_PIN, const int8_t RST_PIN,
          SPIClass &_SPI>
void ST7735S<DC_PIN, CS_PIN, RST_PIN, _SPI>::sendStart() {
  if (CS_PIN >= 0)
    digitalWrite(CS_PIN, LOW);
  _SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
}

template <const uint8_t DC_PIN, const int8_t CS_PIN, const int8_t RST_PIN,
          SPIClass &_SPI>
void ST7735S<DC_PIN, CS_PIN, RST_PIN, _SPI>::sendEnd() {
  _SPI.endTransaction();
  if (CS_PIN >= 0)
    digitalWrite(CS_PIN, HIGH);
}

template <const uint8_t DC_PIN, const int8_t CS_PIN, const int8_t RST_PIN,
          SPIClass &_SPI>
void ST7735S<DC_PIN, CS_PIN, RST_PIN, _SPI>::sendCmd(uint8_t cmd) {
  sendStart();
  digitalWrite(DC_PIN, LOW);
  _SPI.transfer(cmd);
  sendEnd();
}

template <const uint8_t DC_PIN, const int8_t CS_PIN, const int8_t RST_PIN,
          SPIClass &_SPI>
void ST7735S<DC_PIN, CS_PIN, RST_PIN, _SPI>::sendCmd(uint8_t cmd, uint8_t arg) {
  sendStart();
  digitalWrite(DC_PIN, LOW);
  _SPI.transfer(cmd);
  digitalWrite(DC_PIN, HIGH);
  _SPI.transfer(arg);
  sendEnd();
}

template <const uint8_t DC_PIN, const int8_t CS_PIN, const int8_t RST_PIN,
          SPIClass &_SPI>
void ST7735S<DC_PIN, CS_PIN, RST_PIN, _SPI>::sendCmd(uint8_t cmd,
                                                     const uint8_t *args,
                                                     uint16_t count) {
  sendStart();
  digitalWrite(DC_PIN, LOW);
  _SPI.transfer(cmd);
  digitalWrite(DC_PIN, HIGH);
  while (count--) {
    _SPI.transfer(*args++);
  }
  sendEnd();
}

template <const uint8_t DC_PIN, const int8_t CS_PIN, const int8_t RST_PIN,
          SPIClass &_SPI>
inline void
ST7735S<DC_PIN, CS_PIN, RST_PIN, _SPI>::sendData(const uint8_t *data,
                                                 uint16_t count) {
  sendCmd(0x2C, data, count); // RAMWR
}

template <const uint8_t DC_PIN, const int8_t CS_PIN, const int8_t RST_PIN,
          SPIClass &_SPI>
void ST7735S<DC_PIN, CS_PIN, RST_PIN, _SPI>::sendData(const uint16_t *data,
                                                      uint16_t count) {
  sendStart();
  digitalWrite(DC_PIN, LOW);
  _SPI.transfer(0x2C); // RAMWR
  digitalWrite(DC_PIN, HIGH);
  while (count--) {
    _SPI.transfer16(*data++);
  }
  sendEnd();
}

template <const uint8_t DC_PIN, const int8_t CS_PIN, const int8_t RST_PIN,
          SPIClass &_SPI>
void ST7735S<DC_PIN, CS_PIN, RST_PIN, _SPI>::sendData(uint8_t data,
                                                      uint16_t count) {
  sendStart();
  digitalWrite(DC_PIN, LOW);
  _SPI.transfer(0x2C); // RAMWR
  digitalWrite(DC_PIN, HIGH);
  while (count--) {
    _SPI.transfer(data);
  }
  sendEnd();
}

template <const uint8_t DC_PIN, const int8_t CS_PIN, const int8_t RST_PIN,
          SPIClass &_SPI>
void ST7735S<DC_PIN, CS_PIN, RST_PIN, _SPI>::sendData(uint16_t data,
                                                      uint16_t count) {
  sendStart();
  digitalWrite(DC_PIN, LOW);
  _SPI.transfer(0x2C); // RAMWR
  digitalWrite(DC_PIN, HIGH);
  while (count--) {
    _SPI.transfer16(data);
  }
  sendEnd();
}

template <const uint8_t DC_PIN, const int8_t CS_PIN, const int8_t RST_PIN,
          SPIClass &_SPI>
void ST7735S<DC_PIN, CS_PIN, RST_PIN, _SPI>::select(uint8_t x, uint8_t y,
                                                    uint8_t w, uint8_t h) {
  constexpr uint8_t COL_START = 0;
  constexpr uint8_t ROW_START = 24;

  uint8_t args[4];

  args[0] = 0;
  args[1] = COL_START + x;
  args[2] = 0;
  args[3] = COL_START + x + w - 1;
  sendCmd(0x2A, args, sizeof(args) / sizeof(args[0])); // CASET
  args[1] = ROW_START + y;
  args[3] = ROW_START + y + h - 1;
  sendCmd(0x2B, args, sizeof(args) / sizeof(args[0])); // RASET
}
