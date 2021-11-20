#ifndef __PAGE_H__
#define __PAGE_H__
#define KEY_A 1
#define KEY_B 2

#include <WiFi.h>
#include <SSD1306Wire.h>
#include <ClosedCube_HDC1080.h>
#include <NTPClient.h>

#include "perdefine.h"
/**
 * @brief 页面接口
 * 
 */
struct Page
{
    virtual void draw(SSD1306Wire *display) = 0;
    virtual void loop() = 0;
    virtual void onPageEnter() = 0;
    virtual void onPageLeave() = 0;
    virtual void onKeyDown(int key) = 0;
    virtual void onKeyUp(int key) = 0;
};
struct HomePage : Page
{
    char temp[32] = {};
    char humi[32] = {};
    char wifi_line[32] = {};
    char time_line[32] = {};
    bool keydown = false;
    ClosedCube_HDC1080 *hdc1080;
    NTPClient *timeClient;
    HomePage(ClosedCube_HDC1080 *hdc1080, NTPClient *tc)
    {
        this->hdc1080 = hdc1080;
        this->timeClient = tc;
    }

    void draw(SSD1306Wire *display);
    void loop();
    void onPageEnter();
    void onPageLeave();
    void onKeyDown(int key);
    void onKeyUp(int key);
};
struct CounterPage : public Page
{
    int leave = 0;
    int counter = 0;
    int enter = 0;
    bool keydown = false;
    CounterPage() {}
    void draw(SSD1306Wire *display);
    void loop();
    void onPageEnter();
    void onPageLeave();
    void onKeyDown(int key);
    void onKeyUp(int key);
};
#endif // !__PAGE_H__