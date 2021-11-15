#include <Arduino.h>
#include <WiFi.h>
#include <SSD1306Wire.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "ClosedCube_HDC1080.h"
#include "perdefine.h"
#define OLED_HEIGHT 64
#define OLED_WIDTH 128
// #define OLED_FONT ArialMT_Plain_10
#define OLED_FONT ArialMT_Plain_16
// #define OLED_FONT ArialMT_Plain_24

#define OLED_FONT_SIZE 16

#define NTP_OFFSET 28800       // In seconds
#define NTP_INTERVAL 60 * 1000 // In miliseconds
#define NTP_ADDRESS "ntp.aliyun.com"
#define LOOP_DELAY 500

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

// for 128x64 displays:
SSD1306Wire display(0x3c, SDA, SCL);
ClosedCube_HDC1080 hdc1080;
char temp[32] = {};
char humi[32] = {};
char wifi_line[32] = {};
char time_line[32] = {};
void displayInit();
void hdc1080Init();
void printSerialNumber();
void ntpInit();
void drawUI();
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PWD);
  // while (WiFi.status() != WL_CONNECTED)
  // {
  //   Serial.print('.');
  //   delay(300);
  // }
  displayInit();
  hdc1080Init();
  Serial.println("Init Done!");
}

void loop()
{
  // put your main code here, to run repeatedly:
  Serial.printf("Hello world!\n");
  sprintf(temp, "Temp : %.2f", hdc1080.readTemperature());
  sprintf(humi, "Humi : %.2f", hdc1080.readHumidity());
  Serial.println(temp);
  Serial.println(humi);

  timeClient.update();
  snprintf(time_line, 32, "%s", timeClient.getFormattedTime().c_str());
  Serial.println(time_line);
  drawUI();
  delay(LOOP_DELAY);
}

void drawUI()
{
  display.clear();
  // show ip address
  display.setFont(ArialMT_Plain_10);
  String ip_str = "not connected";
  if (WiFi.status() == WL_CONNECTED)
  {

    auto ip = WiFi.localIP();
    ip_str = ip.toString();
  }

  snprintf(wifi_line, 31, "%s %s", WIFI_SSID, ip_str.c_str());
  display.drawString(0, 0, wifi_line);

  display.setFont(OLED_FONT);

  display.drawString(0, 10, temp);
  display.drawString(0, 10 + OLED_FONT_SIZE, humi);
  display.drawString(0, 10 + OLED_FONT_SIZE * 2, time_line);

  display.display();
}

void displayInit()
{
  display.init();
  display.flipScreenVertically();

  display.setFont(OLED_FONT);
}
void hdc1080Init()
{
  // Default settings:
  //  - Heater off
  //  - 14 bit Temperature and ,Humidity Measurement Resolutions
  // 0x40 = 0b01000000 hdc 1080 的 i2c 地址
  hdc1080.begin(0x40);

  Serial.print("Manufacturer ID=0x");
  Serial.println(hdc1080.readManufacturerId(), HEX); // 0x5449 ID of Texas Instruments
  Serial.print("Device ID=0x");
  Serial.println(hdc1080.readDeviceId(), HEX); // 0x1050 ID of the device
  printSerialNumber();
  Serial.println("HDC init done");
}
void printSerialNumber()
{
  Serial.print("Device Serial Number=");
  HDC1080_SerialNumber sernum = hdc1080.readSerialNumber();
  char format[15];
  sprintf(format, "%02X-%04X-%04X", sernum.serialFirst, sernum.serialMid, sernum.serialLast);
  Serial.println(format);
}

void ntpInit()
{

  timeClient.begin();
}