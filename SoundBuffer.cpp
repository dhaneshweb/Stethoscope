#include "SoundBuffer.h"

SoundBuffer::SoundBuffer()
	:isRecording(false), playbackTheta(0), saveTheta(0),
	readPointer(0), writePointer(0), savePointer(0)
{
	for(int i = 0; i < readLength; i++)
	{
		circularBuffer[i] = 0;
	}
}

void SoundBuffer::write_sound_input(int val)
{
	if(!isRecording) return;
	circularBuffer[writePointer++] = val;
	writePointer &= bufferMask;	
	playbackTheta++;
	saveTheta++;
}

void SoundBuffer::get_sound_output(int * outBuffer, int count)
{
	if(playbackTheta >= count)
	{
		for(int i = 0; i < count; i++)
		{
			outBuffer[i] = circularBuffer[readPointer++];
			readPointer &= bufferMask;
		}
		playbackTheta-=count;
	}
	else
	{
		for(int i = 0; i < count; i++)
		{
			outBuffer[i] = 0;
		}
	}
}

void SoundBuffer::start_save(FILE * fp)
{
	while(isRecording || saveTheta > 0)
	{
		if(saveTheta < 4 && isRecording) continue;
		fwrite(&circularBuffer[savePointer], sizeof(int), 4, fp);
		savePointer &= bufferMask;
		saveTheta -= 4;
	}	
}
