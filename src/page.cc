#include "page.h"

void HomePage::draw(SSD1306Wire *display)
{
    // show ip address
    display->setFont(ArialMT_Plain_10);
    String ip_str = "not connected";
    if (WiFi.status() == WL_CONNECTED)
    {
        auto ip = WiFi.localIP();
        ip_str = ip.toString();
    }

    snprintf(this->wifi_line, 31, "%s %s", WIFI_SSID, ip_str.c_str());
    display->drawString(0, 0, this->wifi_line);

    display->setFont(OLED_FONT);
    display->drawString(0, 10, this->temp);
    display->drawString(0, 10 + OLED_FONT_SIZE, humi);
    display->drawString(0, 10 + OLED_FONT_SIZE * 2, time_line);
    display->drawString(0, 10 + OLED_FONT_SIZE * 3, keydown ? "keydown" : "");
}
void HomePage::loop()
{
    if (xSemaphoreTake(mutex, 0) == pdTRUE)
    {
        sprintf(temp, "Temp : %.2f", this->hdc1080->readTemperature());
        sprintf(humi, "Humi : %.2f", this->hdc1080->readHumidity());
        Serial.println(temp);
        Serial.println(humi);

        this->timeClient->update();
        snprintf(time_line, 32, "%s", this->timeClient->getFormattedTime().c_str());
        Serial.println(time_line);
        xSemaphoreGive(mutex);
    }
}
void HomePage::onPageEnter()
{
}
void HomePage::onPageLeave()
{
    keydown = false;
}
void HomePage::onKeyDown(int key)
{
    keydown = true;
}
void HomePage::onKeyUp(int key)
{
    keydown = false;
}

void CounterPage::draw(SSD1306Wire *display)
{

    display->drawString(0, 0, "Counter");
    char buf[50] = {};
    display->drawStringf(0, 16, buf, "->: %d\n", enter);
    display->drawStringf(0, 16 + OLED_FONT_SIZE * 1, buf, "<-: %d\n", leave);
    display->drawStringf(0, 16 + OLED_FONT_SIZE * 2, buf, "pressed: %d\n", counter);
}
void CounterPage::loop()
{
}
void CounterPage::onPageEnter()
{
    ++enter;
}
void CounterPage::onPageLeave()
{
    ++leave;
    keydown = false;
}
void CounterPage::onKeyDown(int key)
{
    keydown = true;
}
void CounterPage::onKeyUp(int key)
{
    if (keydown)
    {
        keydown = false;
        ++counter;
    }
}
