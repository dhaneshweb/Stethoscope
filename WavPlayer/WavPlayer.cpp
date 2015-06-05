// TODO: Add licence and stop start remembering and fill buffer functions



#include "WavPlayer.h"

#define DEBUG 1
#define debug_printf(args ...) if (DEBUG) printf(args)

#define WAVPLAYER_PLAYER_BUF 1000

#define WAVPLAYER_OUTPUT_RATE 48000

#define WAVPLAYER_READ_MARGIN 0.9

#define WAVLPAYER_SLOW_MARGIN 5

#define WAVPLAYER_GEN_TEST 0

#define WAVPLAYER_GEN_FREQ 440

#if WAVPLAYER_GEN_TEST == 1
#include "sinelookup.h"
#else
#define SINE16LENGTH 2
#define SINE32LENGTH 2
const int16_t sine16lookup[] = {0,10000};
#endif

WavPlayer* WavPlayer::instance;
//void WavPlayer::i2sisr();

WavPlayer::WavPlayer() :
        flag_play(true), i2s(I2S_TRANSMIT, p5, p6, p7), codec(p9, p10)//, ext_flag(p22), //run_flag(p23)
{
    instance = this;
    codec.power(true);                  //power up TLV apart from analogue input
    codec.stop();
    codec.frequency(WAVPLAYER_OUTPUT_RATE);            //set sample frequency
    codec.wordsize(16);  //set transfer protocol
    codec.master(false);
    codec.headphone_volume(0.6);
    //interrupt come from the I2STXFIFO only

    i2s.masterslave(I2S_MASTER);
    i2s.attach(&WavPlayer::i2sisr);

    i2s.wordsize(16);
    i2s.stereomono(I2S_MONO);

    i2s.frequency(WAVPLAYER_OUTPUT_RATE);

    //ext_flag = false;

    flag_vol = false;
    volume = 1;
}

WavPlayer::WavPlayer(FILE **fpp):flag_play(true), i2s(I2S_TRANSMIT, p5, p6, p7), codec(p9, p10)//, ext_flag(p22), //run_flag(p23)
{
    instance = this;
    codec.power(true);                  //power up TLV apart from analogue input
    codec.stop();
    codec.frequency(WAVPLAYER_OUTPUT_RATE);            //set sample frequency
    codec.wordsize(16);  //set transfer protocol
    codec.master(false);
    codec.headphone_volume(0.6);
    //interrupt come from the I2STXFIFO only

    i2s.masterslave(I2S_MASTER);
    i2s.attach(&WavPlayer::i2sisr);

    i2s.wordsize(16);
    i2s.stereomono(I2S_MONO);

    i2s.frequency(WAVPLAYER_OUTPUT_RATE);

    //ext_flag = false;

    flag_vol = false;
    volume = 1;
    open(fpp);
}

void WavPlayer::i2sisr()
{
    instance->i2sisr_();
}

void WavPlayer::i2sisr_()
{
    int to_write = i2s.max_fifo_points() - i2s.fifo_points();
    int temp[32];
    /*for(int i = 0; i < 32; i++)
     {
     temp[i] = 0;
     }*/
    int destore[2];
    if (rbuf.numberStored() > to_write + 8 && flag_play)
    {
        for (int i = 0; i < to_write; i += 2)
        {
            rbuf.readFirst(destore);
            rbuf.usedFirst();
            temp[i]   = destore[0];
            temp[i+1] = destore[1];
        }

        i2s.write(temp, to_write);
    }
    else
    {
        for (int i = 0; i < to_write; i++)
        {
            temp[i] = 0;
        }
        i2s.write(temp, to_write);
        isr_underrun++;
        
    }

}

void WavPlayer::open(FILE **fpp)
{
    filepp = fpp;
    if (*filepp == NULL)
        printf(
                "Please open a file before passing the file pointer pointer...\n\r");
    char temp[4];
    fgets_m(temp, 4, *filepp);
    debug_printf("%s\n\r", temp);
    if (strcmp(temp, "RIFF") != 0)
        printf("This is not a RIFF file\n\r");
    fseekread(*filepp, temp, 8, 4);
    if (strcmp(temp, "WAVE") != 0)
        printf("This is not a WAVE file\n\r");

    getConfig();
}

int WavPlayer::getConfig()
{
    char temp[4];
    clear(temp, 4);
    fseekread(*filepp, temp, 4, 4);
    config.file_size = getu32(temp, 4);
    debug_printf("FileSize = %i\n\r", config.file_size);

    int chunkstart = findChunk(*filepp, "fmt ", 4, config.file_size, 12);

    if (chunkstart != -1)
    {

        config.format_tag = fsru16(*filepp, chunkstart + 8);
        debug_printf("Format_tag:            %i\n\r", config.format_tag);

        config.channels = fsru16(*filepp, chunkstart + 10);
        debug_printf("nChannels:            %i\n\r", config.channels);

        config.samples_per_sec = fsru32(*filepp, chunkstart + 12);
        debug_printf("nSamplesPerSec:        %i\n\r", config.samples_per_sec);

        config.avg_bytes_per_sec = fsru32(*filepp, chunkstart + 16);
        debug_printf("avg_bytes_per_sec:    %i\n\r", config.avg_bytes_per_sec);

        config.block_align = fsru16(*filepp, chunkstart + 20);
        debug_printf("block_align:            %i\n\r", config.block_align);

        config.bits_per_sample = fsru16(*filepp, chunkstart + 22);
        debug_printf(
                "bits per sample:            %i\n\r", config.bits_per_sample);

    }

    chunkstart = findChunk(*filepp, "data", 4, config.file_size, 12);

    if (chunkstart != -1)
    {
        config.data_length = fsru32(*filepp, chunkstart + 4);
        debug_printf("Data Length:            %i\n\r", config.data_length);
    }
    return 0;

}

float WavPlayer::play()
{
    return play(float(config.data_length) / float(config.samples_per_sec));

}

float WavPlayer::play(float time)
{
    return play(current_time, time);
}

float WavPlayer::play(float start, float timefor)
{

    #if WAVPLAYER_GEN_TEST == 1
    config.channels = 1;    
    config.samples_per_sec = 48000;
    #endif
    
    float timeElapsed = 0;
    int endPoint = int((start + timefor) * float(config.samples_per_sec));
    if (endPoint * config.block_align >= config.data_length)
        endPoint = config.data_length / config.block_align;
    //printf("Will end after: %i samples \n\r", endPoint);
    int point = int(start * float(config.samples_per_sec));
    bool valid = true;
    flag_play = false;
    int dataOffset = findChunk(*filepp, "data", 4, config.file_size, 12) + 8 + (point * config.block_align);

    double timeComp = 0;

    double out_rate_t_p = 1 / double(WAVPLAYER_OUTPUT_RATE);
    //printf("out_rate_t_p = %f\n\r", out_rate_t_p);
    double out_rate_t_n = -1 / double(WAVPLAYER_OUTPUT_RATE);
    double in_rate = 1 / double(config.samples_per_sec); //0.0000125
    //printf("in_rate = %f\n\r", in_rate);
    
    float phase = 0;

    if (dataOffset != -1)
    {
        fseek(*filepp, dataOffset, SEEK_SET);

        int16_t buffer[WAVPLAYER_PLAYER_BUF];

        codec.start();
        //debug_printf("Expecting isr\n\r");

        i2s.frequency(WAVPLAYER_OUTPUT_RATE);


        if (config.channels == 1)
        {
            stereo = false;
            i2s.stereomono(I2S_MONO);
        }
        else
        {
            stereo =true;
            i2s.stereomono(I2S_STEREO);
        }

        i2s.attach(&WavPlayer::i2sisr);
        i2s.start();
        flag_play = true;

        while (valid && point < endPoint)
        {
            if (rbuf.numberStored() < RBLENGTH - 10)
            {

                //timer.reset();
                //timer.start();
                int numToread = WAVPLAYER_PLAYER_BUF;
                if (numToread > (RBLENGTH - 10) - rbuf.numberStored())
                {
                    numToread = (RBLENGTH - 10) - rbuf.numberStored();
                }
                if (numToread + (point * (config.block_align/config.channels)) > config.data_length)
                {
                    numToread = config.data_length - point * config.block_align;
                    valid = false;
                    debug_printf("EOF detected\n\r");
                }
                if(numToread % 2 == 1) numToread--;
                //printf("Generating\n\r");
                //run_flag = true;
                //phase = sine_gen(buffer,numToread,float(config.samples_per_sec)/120,phase);
                timer.reset();
                timer.start();
                //fread(buffer, config.block_align/config.channels, numToread, *filepp);
                 #if WAVPLAYER_GEN_TEST == 1
                    phase = sine_gen(buffer,numToread,float(config.samples_per_sec)/WAVPLAYER_GEN_FREQ,phase);  
                #else
                    fread(buffer, config.block_align/config.channels, numToread, *filepp);
                #endif
                timer.stop();
                //printf("numToread:%f\n\r",float(numToread));
                if(numToread>=10)
                {
                    read_time += (timer.read()/float(numToread));
                    read_time_div += 1;
                }
                
                
                if(read_time/float(read_time_div) >= WAVPLAYER_READ_MARGIN / float(config.samples_per_sec*config.channels))
                {
                    slow_count++;
                    if(slow_count > WAVLPAYER_SLOW_MARGIN)
                    {
                        printf("Data rates not high enough to sustain read...%f seconds per read\n\r", read_time/float(read_time_div));
                        valid = false;
                        break;
                    }

                }
                
                //run_flag = false;
                int storer[2];
                for (int i = 0; i < numToread; i += 2)
                {

                    //run_flag = true;

                    if (timeComp < out_rate_t_p && timeComp > out_rate_t_n)
                    {
                        storer[0] = (int) buffer[i];
                        storer[1] = (int) buffer[i + 1];

                        //storer[0] = blip;
                        //storer[1] = blip;
                        timeComp += in_rate;
                        while (rbuf.numberStored() > RBLENGTH - 10)
                        {
                            //run_flag = true;
                            //run_flag = false;
                        }
                        rbuf.addToBuffer(storer);
                        timeComp -= out_rate_t_p;
                        //printf(",",timeComp);
                        point++;
                        //printf("straight %f\n\r",timeComp);
                    }
                    else if (timeComp >= out_rate_t_p)
                    {

                        storer[0] = (int) buffer[i];
                        storer[1] = (int) buffer[i + 1];

                        //storer[0] = blip;
                        //storer[1] = blip;

                        timeComp += in_rate;
                        /*const float ct = 5;
                         float c = 0;
                         float bufi = float(buffer[i]);
                         float bufip = float((i+1 >= numToread) ? buffer[i+1] : bufi);*/

                        while (timeComp >= out_rate_t_p)
                        {
                            //storer[0] = int((((ct-c)/ct)*bufi)+((c/ct)*bufip));
                            while (rbuf.numberStored() > RBLENGTH - 10)
                            {
                                //run_flag = true;
                                //run_flag = false;
                            }
                            rbuf.addToBuffer(storer);
                            timeComp -= out_rate_t_p;
                            //printf(";");
                            //if(c < ct) c++;
                        }
                        point++;

                    }
                    else if (timeComp <= out_rate_t_n)
                    {
                        timeComp += in_rate;
                        //printf("-");
                    }

                    //storer[1] = 0;
                    //if(i%1 == 0) printf("%i\n",int(storer[0]),int(storer[1]));
                    //run_flag = false;

                }
                flag_play = true;
                //timer.stop();
                //printf("point: %i\n\r",isr_underrun);
                //read_time += timer;
                //read_time_div =numToread;

            }
        }
    }
    flag_play = false;
    current_time = float(point)/float(config.samples_per_sec);
    return current_time;
}

int WavPlayer::fseekread(FILE *fp, char* str, int offset, int len)
{
    fseek(fp, offset, SEEK_SET);
    fgets_m(str, len, fp);
    return len;
}

uint32_t WavPlayer::getu32(char str[], int len)
{
    uint32_t temp = 0;
    for (int i = 0; i < len; i++)
    {
        temp += str[i] << (i * 8);

    }
    return temp;
}

uint16_t WavPlayer::getu16(char str[], int len)
{
    uint16_t temp = 0;
    for (int i = 0; i < len; i++)
    {
        temp += str[i] << (i * 8);
    }
    return temp;
}

int32_t WavPlayer::get32(char str[], int len)
{
    uint32_t temp = 0;
    for (int i = 0; i < len; i++)
    {
        temp += str[i] << (i * 8);

    }
    return temp;
}

int16_t WavPlayer::get16(char str[], int len)
{
    uint16_t temp = 0;
    for (int i = 0; i < len; i++)
    {
        temp += str[i] << (i * 8);
    }
    return temp;
}

uint32_t WavPlayer::fsru32(FILE *fp, int offset, int len)
{
    char temp[4];
    fseekread(fp, temp, offset, len);
    return getu32(temp, len);
}

uint16_t WavPlayer::fsru16(FILE *fp, int offset, int len)
{
    char temp[2];
    fseekread(fp, temp, offset, len);
    return getu16(temp, len);
}

void WavPlayer::clear(char* str, int len)
{
    for (int i = 0; i < len; i++)
    {
        str[i] = 0;
    }
}

int WavPlayer::findChunk(FILE *fp, char* match, int len, int fileSize,
        int startOffset)
{
    char temp[5];
    int count = startOffset;
    while ((count + 8) < fileSize)
    {
        clear(temp, 5);
        fseekread(fp, temp, count, 4);
        int chunksize = fsru32(fp, count + 4) + 8;
        //debug_printf("@ %i Chunk Name: %s Size: %i\n\r", count, temp, chunksize);

        if (strcmp(temp, match) == 0)
        {
            return count;
        }
        count += chunksize;
    }
    return -1;
}

int WavPlayer::findChunk(FILE *fp, char* match, int len, int startOffset)
{
    return findChunk(fp, match, len, config.file_size, startOffset);
}

void WavPlayer::fgets_m(char* str, int num, FILE* fp)
{
    for (int i = 0; i < num; i++)
    {
        str[i] = fgetc(fp);
        //printf("%c",str[i]);
    }
}

float WavPlayer::sine_gen(int16_t* buf, int len, float div, float phase)
{
    #if WAVPLAYER_GEN_TEST == 1
    float t = SINE16LENGTH / div;
    for (int i = 0; i < len; i++)
    {
        buf[i] = sine16lookup[int(phase)];
        //printf("%i\n\r",buf[i]);
        phase += t;
        while (phase >= SINE16LENGTH)
            phase -= SINE16LENGTH;
    }
    #endif
    return phase;
}
