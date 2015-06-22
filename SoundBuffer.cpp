#include "SoundBuffer.h"
#include "GlobalResource.h"

volatile bool isRecording = false;
volatile int playbackTheta = 0;
volatile int saveTheta = 0;

int readPointer = 0;
int writePointer = 0;
int savePointer = 0;
int maxSaveTheta = 0;
int saveSize = 0;

const int readLength = 2048;
const int bufferMask = readLength - 1;

int circularBuffer[readLength];

void init_SoundBuffer()
{
	for(int i = 0; i < readLength; i++)
	{
		circularBuffer[i] = 0;
	}
}

void write_sound_input(int val)
{
	if(!isRecording) return;
	circularBuffer[writePointer++] = val;
	writePointer &= bufferMask;	
	playbackTheta++;
	saveTheta++;
}

void write_save_header(FILE * fp)
{
	/* RIFF WAV header
 * ---------------
 * This is composed of several sub chunks. All strings
 * are in big endian form. All numeric data is in little endian form.
 * All data to be completed after recording (i.e. data chunk size) is default blank.
 */
    char blockAlign = (char)channels * (wordWidth/8);
	int saveRate = sampleRate / 4;
    int bps = (int)saveRate * blockAlign;
    char header[44] = { 0x52, 0x49, 0x46, 0x46,     //'RIFF'
                        0x00, 0x00, 0x00, 0x00,     //file size
                        0x57, 0x41, 0x56, 0x45,     //'WAVE'
       /*sub chunk 1*/  0x66, 0x6d, 0x74, 0x20,     //'fmt '
                        0x10, 0x00, 0x00, 0x00,     //subchunk size = 16
                        0x01, 0x00, ((char)(channels & 0xff)), 0x00,     //PCM compression code | number of channels - I assume no more than 2 channels
                        ((char)((saveRate & 0xff)>>0)), ((char)((saveRate & 0xff00)>>8)), ((char)((saveRate & 0xff0000)>>16)), ((char)((saveRate & 0xff000000)>>24)),   //sample rate
                        ((char)((bps & 0xff)>>0)), ((char)((bps & 0xff00)>>8)), ((char)((bps & 0xff0000)>>16)), ((char)((bps & 0xff000000)>>24)),     //bit rate
                        blockAlign, 0x00, ((char)((wordWidth & 0xff)>>0)), ((char)((wordWidth & 0xff00)>>8)),   //bloack align | wordwidth
       /*sub chunk 2*/  0x64, 0x61, 0x74, 0x61,     //'data'
                        0x00, 0x00, 0x00, 0x00};    //size of data chunk in bytes
     fwrite(header, 1, 44, fp); 
}

void get_sound_output(int * outBuffer, int count)
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

void start_save(FILE * fp)
{
	static const int writeChunckSize = 8;
	static short toWrite[writeChunckSize];
	while(isRecording || saveTheta > 0)
	{
		if(saveTheta < writeChunckSize*2 && isRecording) continue;
		for(int i = 0; i < writeChunckSize; i++)
		{
			toWrite[i] = circularBuffer[savePointer] & 0xffff;
			savePointer += 2;
			savePointer &= bufferMask;
		}
		fwrite(toWrite, sizeof(short), writeChunckSize, fp);
		saveTheta -= writeChunckSize*2;
		maxSaveTheta = saveTheta > maxSaveTheta ? saveTheta : maxSaveTheta;
		saveSize += sizeof(short) * writeChunckSize;
	}	
}

void complete_save_header(FILE * fp)
{
	saveSize += 36;
    fseek(fp, 4, SEEK_SET);
    fwrite(&saveSize, sizeof(int), 1, fp);
    fseek(fp, 40, SEEK_SET);
    saveSize -= 36;
    fwrite(&saveSize, sizeof(int), 1, fp);
}

void start_SoundBuffer()
{
	isRecording = true;
}

void stop_SoundBuffer()
{
	isRecording = false;
}

int get_MaxSaveTheta()
{
	return maxSaveTheta;
}

int get_saveSize()
{
	return saveSize;
}
