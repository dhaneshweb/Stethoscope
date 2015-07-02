#ifndef __GLOBALRESOURCE_H__
#define __GLOBALRESOURCE_H__

#include "mbed.h"
#include "USBSerial.h"
#include "stdio.h"
#include "TLV320.h"
#include "SDFileSystem.h"
#include "USBMSD.h"
#include "SoundBuffer.h"
#include "Adafruit_SSD1306.h"


extern TLV320 audio; 
//USBSerial serial;
extern DigitalOut led1;
extern DigitalOut led2;
extern SDFileSystem sd;
extern Ticker ticker;
extern Adafruit_SSD1306_I2c oled;

extern int channels;
extern int wordWidth;
extern int sampleRate;

#endif
