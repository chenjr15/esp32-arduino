#include "SSD1306Wire.h"
#include "images.h"
#define DEMO_DURATION 3000
typedef void (*Demo)(void);
int demoMode = 0;
int counter = 1;
// for 128x64 displays:
SSD1306Wire display(0x3c, SDA, SCL);  // ADDRESS, SDA, SCL
void setup_display();
void drawFontFaceDemo();
void drawTextFlowDemo();
void drawTextAlignmentDemo();
void drawRectDemo() ;
void drawCircleDemo();
void drawProgressBarDemo();
void drawImageDemo();
Demo demos[] = {drawFontFaceDemo, drawTextFlowDemo, drawTextAlignmentDemo, drawRectDemo, drawCircleDemo, drawProgressBarDemo, drawImageDemo};
int demoLength = (sizeof(demos) / sizeof(Demo));
long timeSinceLastModeSwitch = 0;