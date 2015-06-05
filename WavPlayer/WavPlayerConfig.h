/**
* @author Giles Barton-Owen
*
* @section LICENSE
*
* Copyright (c) 2012 mbed
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
* @section DESCRIPTION
*    A WAV player configuration class for WavPlayer
*
*/ 


#ifndef WAVPLAYERCONFIG_H
#define WAVPLAYERCONFIG_H

#include "mbed.h"

class WavPlayerConfig
{
public:
    /*format_tag(uint16_t ft);
     channels(uint16_t n);
     samples_per_second(uint32_t sps);
     avg_bytes_per_second(uint32_t abps);
     block_align(uint16_t ba);
     bits_per_sample(uint16_t bps);
     data_length(uint32_t len);*/

    WavPlayerConfig();

    uint16_t format_tag;
    uint16_t channels;
    uint32_t samples_per_sec;
    uint32_t avg_bytes_per_sec;
    uint16_t block_align;
    uint16_t bits_per_sample;
    uint32_t data_length;
    uint32_t file_size;

private:

};

#endif
