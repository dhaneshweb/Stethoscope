#include "GlobalResource.h"
#include "AverageFilter.h"

void I2S_Handler(void);
void stop(void);

/* main */
int main()
{   
	int channels = 1;
	int sampleRate = 32000;
	int wordWidth = 16;
	FILE * streamfp = fopen("/sd/read_log_2.dat", "w");
	FILE * fp = fopen("/sd/log.txt", "w");
    if(streamfp == NULL || fp == NULL)
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
	audio.outputVolume(0.9, 0.9);
    audio.frequency(sampleRate);            //set sample frequency
    audio.format(wordWidth, (2-channels));  //set transfer protocol
    audio.attach(&I2S_Handler);         

	soundBuffer.start();
    audio.start(BOTH);                  //interrupt come from the I2STXFIFO only
	ticker.attach(stop, 20);
	soundBuffer.start_save(streamfp);
//	fillBuffer();                           //continually fill circular buffer
//	myled = 1;
//	fprintf(logfp, "%ld %d %d \r\n", readCount, before, after);
	fclose(fp);
	fclose(streamfp);
	return 0;
}

/* Function to read from circular buffer and send data to TLV320 */
void I2S_Handler(void)
{
	static AverageFilter filter;
	static int readCount = 0;
	static int outputBuffer[4] = {0, 0, 0, 0};
	
	led2 = (readCount>>9) & 0x01;
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
//	led2 = 1;
	audio.stop();
	soundBuffer.stop();
}


