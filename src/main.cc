

#include <Arduino.h>
#include <vector>
#include <WiFi.h>
#include <SSD1306Wire.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "ClosedCube_HDC1080.h"
#include "page.h"
#include "perdefine.h"

using namespace std;
Page **pages;
uint32_t pagesNum;
uint32_t curPageIdx = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

// for 128x64 displays:
SSD1306Wire display(0x3c, SDA, SCL);
ClosedCube_HDC1080 hdc1080;
SemaphoreHandle_t mutex;
void onKeyADown()
{
}
void onKeyAUp()
{
    // 切换页面 -> 下一页
    int now = curPageIdx;
    pages[now]->onPageLeave();
    curPageIdx = (curPageIdx + 1) % pagesNum;
    now = curPageIdx;
    Serial.printf("Turn to page:%d\n", curPageIdx);
    pages[now]->onPageEnter();
}
void onKeyBDown()
{
    // 用户页面事件
    int now = curPageIdx;
    pages[now]->onKeyDown(KEY_B);
}
void onKeyBUp()
{
    int now = curPageIdx;
    pages[now]->onKeyUp(KEY_B);
}

void UIDrawThread(void *parms)
{
    while (1)
    {

        if (xSemaphoreTake(mutex, 0) == pdFALSE)
        {
            vTaskDelay(10);
            continue;
        }
        Serial.println("Draw!");
        // 绘制ui
        int now = curPageIdx;
        display.clear();
        pages[now]->draw(&display);
        display.display();
        xSemaphoreGive(mutex);
        vTaskDelay(500);
    }
}

void LoopThread(void *parms)
{
    while (1)
    {

        // 循环页面
        for (size_t i = 0; i < pagesNum; ++i)
        {

            pages[i]->loop();

            vTaskDelay(50);
        }
        vTaskDelay(1000);
        /* code */
    }
}
void setup()
{
    Serial.begin(115200);

    display.init();
    display.flipScreenVertically();

    display.setFont(OLED_FONT);
    display.drawString(0, 0, "init...");
    display.display();
    hdc1080.begin(0x40);
    WiFi.begin(WIFI_SSID, WIFI_PWD);
    pagesNum = 2;
    pages = new Page *[pagesNum]
    {
        new HomePage(&hdc1080, &timeClient),
            new CounterPage(),
    };
    // 加入任务中

    pinMode(KEY_BUILTIN, INPUT_PULLUP);
    pinMode(GPIO_NUM_23, INPUT_PULLUP);
    // 注册下降沿回调
    attachInterrupt(KEY_BUILTIN, onKeyADown, FALLING);
    attachInterrupt(KEY_BUILTIN, onKeyAUp, RISING);
    attachInterrupt(GPIO_NUM_23, onKeyBDown, FALLING);
    attachInterrupt(GPIO_NUM_23, onKeyBUp, RISING);
    // Create mutex before starting tasks
    mutex = xSemaphoreCreateMutex();
    pages[curPageIdx]->onPageEnter();
    // xTaskCreatePinnedToCore(UIDrawThread, "UI Thread", 10240, NULL, 10, NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(LoopThread, "Loop Thread", 4096, NULL, 5, NULL, tskNO_AFFINITY);
}

void loop()
{
    UIDrawThread(NULL);
}