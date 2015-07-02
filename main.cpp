#include "GlobalResource.h"
#include "AverageFilter.h"

void I2S_Handler(void);
void stop(void);

/* main */
int main()
{   
	oled.setRotation(0);
	oled.clearDisplay();
	oled.splash();
	oled.display();
	wait(10);
	oled.display();
	for(int x=0;x<128;x++)
	{
		for(int y=0;y<64;y++)
		{
			oled.drawPixel(x,y,WHITE);
		}
	}
	oled.display();
	oled.clearDisplay();
	for(int x=0;x<100;x++)
	{
		oled.setTextCursor(0,0);
		oled.printf("hello  %d \r\n",x);
		oled.display();
		wait(0.1);
	}

	
//oled.printf("%ux%u OLED Display\r\n", oled.width(), oled.height());
//	FILE * streamfp = fopen("/sd/read_log_2.wav", "w");
//	FILE * fp = fopen("/sd/log.txt", "w");
//    if(streamfp == NULL || fp == NULL)
//	{                   	//make sure it's been opened
//		led1 = 1;
//		led2 = 1;
//        perror("Error opening file!");
//        exit(1);
//    }
//	led1 = 1;
//	wait(1);
//	led1 = 0;
//	
//	audio.power(0x00);
//	audio.openMicInput(true);
//	audio.outputVolume(0.7, 0.7);
//    audio.frequency(sampleRate);            //set sample frequency
//    audio.format(wordWidth, (2-channels));  //set transfer protocol
//    audio.attach(&I2S_Handler);         

//	start_SoundBuffer();
//    audio.start(BOTH);                  //interrupt come from the I2STXFIFO only
//	ticker.attach(stop, 5);
//	write_save_header(streamfp);
//	led1 = 1;
//	start_save(streamfp);
//	fprintf(fp, "%d", get_saveSize());
////	fillBuffer();                           //continually fill circular buffer
////	myled = 1;
////	fprintf(logfp, "%ld %d %d \r\n", readCount, before, after);
//	complete_save_header(streamfp);
//	fclose(fp);
//	fclose(streamfp);
	while(1)
	{
		led2 = 1;
		led1 = 1;
	}
	return 0;
}

/* Function to read from circular buffer and send data to TLV320 */
//void I2S_Handler(void)
//{
//	static AverageFilter filter;
//	static int readCount = 0;
//	static int outputBuffer[4] = {0, 0, 0, 0};
//	
//	led2 = (readCount>>9) & 0x01;
//	if(audio.get_rxFIFO_level() > 4)
//	{
//		readCount++;
//		audio.read();
//		for(int i=0; i<4; i++)
//		{
//			write_sound_input(filter.push_filter_buffer(audio.rxBuffer[i]));
//		}
//	}
////	fprintf(logfp, "%d ", audio.get_txFIFO_level());
//	if(audio.get_txFIFO_level() <= 4)
//	{
//		get_sound_output(outputBuffer, 4);
//		audio.write(outputBuffer, 0 ,4);
//	}
////	fprintf(logfp, "%d ", audio.get_txFIFO_level());
//}

//void stop(void)
//{
////	led2 = 1;
//	audio.stop();
//	stop_SoundBuffer();
//}


