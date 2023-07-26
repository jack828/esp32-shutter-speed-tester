#define KEY_UP 8
#define KEY_RIGHT 9
#define KEY_DOWN 13
#define KEY_LEFT 5
#define KEY_CENTRE 4

#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  Serial.println("Hello! ST7735 TFT");

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  Serial.println("Initialised");

  int16_t gutterSize = tft.fontHeight() / 2;
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(TFT_RED);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("SHUTTER", tft.width() / 2, 0 + gutterSize);
  tft.drawString("SPEED", tft.width() / 2, tft.fontHeight() + gutterSize);
  tft.drawString("TESTER", tft.width() / 2, (tft.fontHeight() * 2) + gutterSize);

  delay(1000);
  tft.fillScreen(TFT_BLACK);
}

void loop() {
  delay(500);
}
