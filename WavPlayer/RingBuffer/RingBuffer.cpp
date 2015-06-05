

#include "RingBuffer.h"
//Serial debug(USBTX,USBRX);

RingBuffer::RingBuffer()
{
    Start = 0;
    Finish = 0;
    for(int i = 0; i < RBLENGTH; i++)
    {
        for(int j = 0; j<RBWIDTH; j++)
        {
            Buffer[i][j] = 0;
        }
    }
    //debug.baud(115200);
}

int RingBuffer::addToBuffer(int* input)
{
    if(Finish == Start && Samples != 0)
    {
        return -1;     
    }
    else
    {
        for(int i = 0; i < RBWIDTH; i++)
        {
            Buffer[Finish][i] = input[i];
        }
        sortFinishNS();
        return 0;
    }
    
}


int RingBuffer::addToBufferS(int input, int across)
{
    Buffer[Finish][across] = input;
    return 0;
}



int RingBuffer::nextBuffer()
{
    sortFinishNS();
    return 0;
}



void RingBuffer::readFirst(int* output)
{
    for(int i = 0; i < RBWIDTH; i++)
        {
            output[i] = Buffer[Start][i];
        }
}

int RingBuffer::readFirstS(int across)
{
    return Buffer[Start][across];
}



void RingBuffer::readAt(int* output, int at)
{
    for(int i = 0; i < RBWIDTH; i++)
        {
            output[i] = Buffer[at][i];
        }
}

int RingBuffer::readAtS(int at, int across)
{
    return Buffer[at][across];
}



void RingBuffer::readLast(int*output)
{
     for(int i = 0; i < RBWIDTH; i++)
        {
            output[i] = Buffer[Finish-1][i];
        }
}


int RingBuffer::readLastS(int across)
{
    return Buffer[Finish-1][across];
}

void RingBuffer::usedFirst()
{
    sortStartUS();
}

void RingBuffer::usedLast()
{
    Finish--;
    Samples--;
}

int RingBuffer::numberStored()
{
    if(Samples > RBLENGTH)
    {
        if(Finish < Start)
        {
            Samples = Finish + (RBLENGTH - Start);
        }
        else
        {
            Samples = Start - Finish;
        }
    }
    return Samples;
}

void RingBuffer::sortFinishNS()
{
    Finish++;
    Samples++;
    if(Finish == RBLENGTH)
    {
        Finish = 0;
        //debug.printf("\n\rBOO! %3i %3i\n\r",Start, Finish);
    }
    
}

void RingBuffer::sortStartUS()
{
    Start++;
    Samples--;
    if(Start == RBLENGTH)
    {
        Start = 0;
    }
}