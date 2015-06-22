#ifndef __SOUNDBUFFER_H__
#define __SOUNDBUFFER_H__

#include "mbed.h"

void init_SoundBuffer();
void write_sound_input(int val);
void get_sound_output(int * outBuffer, int count);
void start_save(FILE * fp);

void start_SoundBuffer();
void stop_SoundBuffer();
	
void write_save_header(FILE * fp);
void complete_save_header(FILE * fp);

int get_MaxSaveTheta();
int get_saveSize();

#endif
