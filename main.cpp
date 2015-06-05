#include "mbed.h"
#include "USBSerial.h"
#include "LCD.h"
#include "stdio.h"
#include "TLV320.h"
#include "SDFileSystem.h"

#define READ_LENGTH 2048
#define BUFFER_MASK (READ_LENGTH-1)


char msgInit[] = "Display ready";
////////////D0--------------------------D7, RS, Enable
//TLV320(PinName sda, PinName scl, int addr, PinName tx_sda, PinName tx_ws, PinName clk, PinName rx_sda, PinName rx_ws)
TLV320 audio(P0_0, P0_1, 52, P0_9, P0_8, P0_7, P0_6, P0_5); 
//USBSerial serial;
DigitalOut myled(P2_7);
DigitalOut led2(P2_8);
SDFileSystem sd(P0_18, P0_17, P0_15, P0_16, "sd");
FILE *infp;                                     //File pointer object
FILE * logfp;

/* Buffers */
int circularBuffer[READ_LENGTH];
int emptyOut[8] = {200, 0, 0, 0, 0, 0, 0, 0};
int outputBuffer[4] = {5000, 0, 5000, 0};
volatile int readPointer = 0;
volatile int writePointer = 0;
volatile int theta = 0;
/* Wav file header data, for setting up the transfer protocol */
int channels;
int sampleRate;
int wordWidth;

int before;
int after;

long readCount = 0;
long writeCount = 0;

/* Function to read from circular buffer and send data to TLV320 */
void play(void){
	led2 = (readCount>>9) & 0x01;
	if(audio.get_rxFIFO_level() > 4)
	{
		readCount++;
		audio.read();
		for(int i=0; i<4; i++)
		{
			circularBuffer[writePointer++] = audio.rxBuffer[i];
			writePointer &= BUFFER_MASK;
		}
//		if(audio.get_rxFIFO_level() > 4)
//		{
//			led2 = 1;
//		}
		theta+=4;
	}
//	fprintf(logfp, "%d ", audio.get_txFIFO_level());
	if(audio.get_txFIFO_level() <= 4)
	{
//		audio.write(emptyOut, 0, 4);
		if(theta >= 4)
		{
			writeCount++;
//			myled = writeCount > 20000;
			//led2 = !led2;
			for(int i=0; i<4; i++)
			{
				outputBuffer[i] = circularBuffer[readPointer++];
				readPointer &= BUFFER_MASK;
			}
//			if (outputBuffer[0] != 0)
//			{
//				myled = 1;
//			}
			audio.write(outputBuffer, 0, 4);      
			theta -= 4;
		}
		else 
		{
			audio.write(emptyOut, 0, 4);
	//		theta -= 8;
		}
	}
//	fprintf(logfp, "%d ", audio.get_txFIFO_level());
}

void chargeOneBuffer()
{
	fread(&circularBuffer[writePointer], sizeof(int), 4, infp);   //read 128 integers into the circular buffer at a time
	theta+=4;
	//More pointer fun :D
	writePointer+=4;
	writePointer = writePointer & 0xFFF;
}

/* Function to load circular buffer from SD Card */
void fillBuffer(void){
    while(!feof(infp)){                                         //fill the circular buffer until the end of the file
//		if (theta >= 3900)
//			led2 = 1;
//		if (theta <= 100)
//			led2 = 0;
        if(theta < 4096){
			chargeOneBuffer();
        }
    }
}

void chargeBuffer(void)
{
	while(theta < 128 && !feof(infp))
	{
		chargeOneBuffer();
	}
}

/* main */
int main(){   
//	myled = 1;
//    infp = fopen("/sd/piano2.wav", "r");      //open file
	logfp = fopen("/sd/read_log.txt", "w");
    if(logfp == NULL){                   	//make sure it's been opened
		myled = 1;
		led2 = 1;
        perror("Error opening file!");
        exit(1);
    }
	myled = 1;
	wait(1);
	myled = 0;
	

    /* Parse wav file header */
//    fseek(infp, 22, SEEK_SET);                      
//    fread(&channels, 2, 1, infp);
//    fseek(infp, 24, SEEK_SET);
//    fread(&sampleRate, 4, 1, infp);
//    fseek(infp, 34, SEEK_SET);
//    fread(&wordWidth, 2, 1, infp);
	
	channels = 1;
	sampleRate = 32000;
	wordWidth = 16;
//	audio.power(0x00);                      //power up TLV apart from analogue input
//	fprintf(logfp, "%d %d %d\r\n",channels, wordWidth, sampleRate);

	audio.power(0x00);
	audio.openMicInput(true);
	audio.outputVolume(0.9, 0.9);
    audio.frequency(sampleRate);            //set sample frequency
    audio.format(wordWidth, (2-channels));  //set transfer protocol
    audio.attach(&play);                    //attach interrupt handler to send data to TLV320
//	audio.bypass(true);
//	wait(20);

    for(int j = 0; j < READ_LENGTH; ++j){          //upon interrupt generation
        circularBuffer[j] = 0;              //clear circular buffer
    }
//	myled = 1;
//	chargeBuffer();
//	myled = 0;
    audio.start(BOTH);                  //interrupt come from the I2STXFIFO only
//	fillBuffer();                           //continually fill circular buffer
//	myled = 1;
	wait(10);
	audio.stop();
	myled = 0;
	led2 = 0;
	fprintf(logfp, "%ld %ld \n", readCount, writeCount);
	for(int i=0; i < READ_LENGTH; i++)
	{
		fprintf(logfp, "%d %d ", circularBuffer[i] & 0xFFFF, (circularBuffer[i] >> 16) & 0xFFFF);
	}
	fclose(logfp);
	
//	fprintf(logfp, "%ld %d %d \r\n", readCount, before, after);
//	fclose(logfp);
}

