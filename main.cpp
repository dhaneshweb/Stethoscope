#include "GlobalResource.h"
#include "AverageFilter.h"
#include "FIRFilter.h"

void I2S_Handler(void);
void stop(void);

AverageFilter averageFilter;
SoundBuffer soundBuffer(&averageFilter);

/* main */
int main()
{   
	FILE * streamfp = fopen("/sd/beat.wav", "w");
    if(streamfp == NULL)
	{                   	//make sure it's been opened
		led1 = 1;
		led2 = 1;
        perror("Error opening file!");
        exit(1);
    }
	led1 = 1;
	wait(1);
	led1 = 0;

	
	
	audio.power(0x00);
	audio.openMicInput(true);
	audio.outputVolume(0.7, 0.7);
    audio.frequency(sampleRate);            //set sample frequency
    audio.format(wordWidth, (2-channels));  //set transfer protocol
    audio.attach(&I2S_Handler);         

	soundBuffer.start_SoundBuffer();
    audio.start(BOTH);                  //interrupt come from the I2STXFIFO only
	ticker.attach(stop, 5);
	led1 = 1;
	soundBuffer.start_save(streamfp);
//	fillBuffer();                           //continually fill circular buffer
//	myled = 1;
//	fprintf(logfp, "%ld %d %d \r\n", readCount, before, after);
	fclose(streamfp);
	led2 = 1;
	led1 = 1;
	wait(1);
	
		
	FIRFilter filter(50);
	soundBuffer.set_filter(&filter);
	
	streamfp = fopen("/sd/mix.wav", "w");
	soundBuffer.reset_SoundBuffer();
	soundBuffer.start_SoundBuffer();
	audio.start(BOTH);
	ticker.attach(stop, 5);
	soundBuffer.start_save(streamfp);
	fclose(streamfp);
	
	while(1)
	{
		led2 = 1;
		led1 = 1;
	}
	return 0;
}

/* Function to read from circular buffer and send data to TLV320 */
void I2S_Handler(void)
{
	static AverageFilter filter;
	static int readCount = 0;
	static int outputBuffer[4] = {0, 0, 0, 0};
	
	led2 = (readCount>>10) & 0x01;
	if(audio.get_rxFIFO_level() > 4)
	{
		readCount++;
		audio.read();
		for(int i=0; i<4; i++)
		{
			soundBuffer.write_sound_input(filter.push_filter_buffer(audio.rxBuffer[i]));
		}
	}
//	fprintf(logfp, "%d ", audio.get_txFIFO_level());
	if(audio.get_txFIFO_level() <= 4)
	{
		soundBuffer.get_sound_output(outputBuffer, 4);
		audio.write(outputBuffer, 0 ,4);
	}
//	fprintf(logfp, "%d ", audio.get_txFIFO_level());
}

void stop(void)
{
	led2 = 0;
	led1 = 0;
	wait(1);
	audio.stop();
	soundBuffer.stop_SoundBuffer();
}


