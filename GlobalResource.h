#ifndef __GLOBALRESOURCE_H__
#define __GLOBALRESOURCE_H__

#include "mbed.h"
#include "USBSerial.h"
#include "LCD.h"
#include "stdio.h"
#include "TLV320.h"
#include "SDFileSystem.h"
#include "USBMSD.h"
#include "SoundBuffer.h"

extern TLV320 audio; 
//USBSerial serial;
extern DigitalOut led1;
extern DigitalOut led2;
extern SDFileSystem sd;
extern Ticker ticker;
extern SoundBuffer soundBuffer;

#endif
