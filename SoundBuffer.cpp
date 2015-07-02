#include "SoundBuffer.h"
#include "GlobalResource.h"

SoundBuffer::SoundBuffer()
	:isRecording(false), playbackTheta(0), saveTheta(0)
	,readPointer(0), writePointer(0), maxSaveTheta(0)
	,saveSize(0), filter(0)
{
	for(int i = 0; i < readLength; i++)
	{
		circularBuffer[i] = 0;
	}
}

SoundBuffer::SoundBuffer(IFilter * filter_)
	:isRecording(false), playbackTheta(0), saveTheta(0)
	,readPointer(0), writePointer(0), maxSaveTheta(0)
	,saveSize(0), filter(filter_)
{
	for(int i = 0; i < readLength; i++)
	{
		circularBuffer[i] = 0;
	}
}

void SoundBuffer::reset_SoundBuffer()
{
	readPointer = 0;
	writePointer = 0;
	savePointer = 0;
	saveSize = 0;
	for(int i = 0; i < readLength; i++)
	{
		circularBuffer[i] = 0;
	}
}

void SoundBuffer::write_sound_input(int val)
{
	if(!isRecording) return;
	circularBuffer[writePointer++] = filter ? filter->push_filter_buffer(val) : val;
	writePointer &= bufferMask;	
	playbackTheta++;
	saveTheta++;
}

void SoundBuffer::write_save_header(FILE * fp)
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
	static const int writeChunckSize = 8;
	static short toWrite[writeChunckSize];
	write_save_header(fp);
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
	complete_save_header(fp);
}

void SoundBuffer::complete_save_header(FILE * fp)
{
	saveSize += 36;
    fseek(fp, 4, SEEK_SET);
    fwrite(&saveSize, sizeof(int), 1, fp);
    fseek(fp, 40, SEEK_SET);
    saveSize -= 36;
    fwrite(&saveSize, sizeof(int), 1, fp);
}

void SoundBuffer::start_SoundBuffer()
{
	isRecording = true;
}

void SoundBuffer::stop_SoundBuffer()
{
	isRecording = false;
}

int SoundBuffer::get_MaxSaveTheta()
{
	return maxSaveTheta;
}

int SoundBuffer::get_saveSize()
{
	return saveSize;
}

void SoundBuffer::set_filter(IFilter * newFilter)
{
	filter = newFilter;
}

SoundBuffer::~SoundBuffer()
{
}
