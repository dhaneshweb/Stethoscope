#ifndef __SOUNDBUFFER_H__
#define __SOUNDBUFFER_H__

#include "mbed.h"
#include "IFilter.h"

class SoundBuffer
{
void init_SoundBuffer();
public:
	SoundBuffer();
	SoundBuffer(IFilter * filter);
	
	void reset_SoundBuffer();

	void write_sound_input(int val);
	void get_sound_output(int * outBuffer, int count);
	void start_save(FILE * fp);

	void start_SoundBuffer();
	void stop_SoundBuffer();

	int get_MaxSaveTheta();
	int get_saveSize();

	void set_filter(IFilter * newFilter);

	~SoundBuffer();
private:
	void write_save_header(FILE * fp);
	void complete_save_header(FILE * fp);

	volatile bool isRecording;
	volatile int playbackTheta;
	volatile int saveTheta;

	int readPointer;
	int writePointer;
	int savePointer;
	int maxSaveTheta;
	int saveSize;

	static const int readLength = 2048;
	static const int bufferMask = readLength - 1;

	IFilter * filter;

	int circularBuffer[readLength];
};

#endif
