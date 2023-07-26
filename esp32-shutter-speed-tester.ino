#define KEY_UP 8
#define KEY_RIGHT 9
#define KEY_DOWN 13
#define KEY_LEFT 5
#define KEY_CENTRE 4

#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup(void) {
  Serial.begin(115200);
  Serial.print("Hello! ST7735 TFT Test\n");

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  Serial.println("Initialized");

}

void loop () {
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(TFT_RED);
  tft.printf("SHUTTER\n");
  tft.printf("SPEED\n");
  tft.printf("TESTER");
  delay(500);
}
