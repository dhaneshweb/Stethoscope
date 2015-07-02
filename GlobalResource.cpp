#include "GlobalResource.h"

TLV320 audio(P0_0, P0_1, 52, P0_9, P0_8, P0_7, P0_6, P0_5); 
//USBSerial serial;
DigitalOut led1(P2_7);
DigitalOut led2(P2_8);
SDFileSystem sd(P0_18, P0_17, P0_15, P0_16, "sd");
Ticker ticker;

I2CPreInit i2c_oled(P0_27,P0_28);
Adafruit_SSD1306_I2c oled(i2c_oled,P0_2,0x78,64);

int channels = 1;
int wordWidth = 16;
int sampleRate = 32000;
