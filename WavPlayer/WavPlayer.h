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
*    A WAV player library for the TLV320 and the LPC1768's built in I2S peripheral
*
*/ 

#ifndef WAVPLAYER_H
#define WAVPLAYER_H

#include "mbed.h"
#include "WavPlayerConfig.h"
#include "RingBuffer.h"
#include "TLV320.h"
#include "I2S.h"

/** A class to play WAV files from a file system, tested with USB
 *
 * Example (note, this has requires the USB MSC library to be imported):
 * @code
 *
 * #include "mbed.h"
 * #include "WavPlayer.h"
 * #include "MSCFileSystem.h"
 * 
 * MSCFileSystem msc("msc"); // Mount flash drive under the name "msc"
 * WavPlayer player;
 * 
 * int main() {
 *    FILE *fp = fopen("/msc/test.wav", "r");  // Open "out.txt" on the local file system for writing
 *     player.open(&fp);
 *     player.play();
 *     fclose(fp);
 * }
 * @endcode
 */


class WavPlayer
{
public:
      /** Create a WavPlayer instance
     * 
     */
    WavPlayer();
    
    /** Create a WavPlayer instance
     * 
     * @param fpp A pointer to a file pointer to read out of
     */
    WavPlayer(FILE **fpp);
    
    /** Set the file to read out of
     * 
     * @param fpp A pointer to a file pointer to read out of
     */
    void open(FILE **fpp);
    
    /** Extract the header infomation, automatically called by open
     */
    int getConfig();
    
    /** Play the entire file. Blocking
     */
    float play();
    
    /** Play the file for a certain number of seconds. Blocking
     *
     * @param time The number of seconds to play the file for.
     */
    float play(float time);
    
    /** Play the file for a certain number of seconds, from a certain start point. Blocking
     *
     * @param start        The start time
     * @param timefor     The number of seconds to play the file for.
     */
    float play(float start, float timefor);



private:
    WavPlayerConfig config;
    FILE ** filepp;
    RingBuffer rbuf;

    I2S i2s;
    TLV320 codec;
    static void i2sisr();
    void i2sisr_();
    static WavPlayer* instance;

    int fseekread(FILE *fp, char* str, int offset, int len);
    uint32_t getu32(char str[], int len);
    uint16_t getu16(char str[], int len);
    int32_t get32(char str[], int len);
    int16_t get16(char str[], int len);
    uint32_t fsru32(FILE *fp, int offset, int len = 4);
    uint16_t fsru16(FILE *fp, int offset, int len = 2);
    void clear(char* str, int len);

    int findChunk(FILE *fp, char* match, int len, int fileSize, int startOffset = 12);
    int findChunk(FILE *fp, char* match, int len, int startOffset = 12);
    void fgets_m(char* str, int num, FILE* fp);

    float sine_gen(int16_t* buf, int len, float div, float phase);

    bool flag_vol;
    double volume;

    bool stereo;

    bool flag_play;

    Timer timer;
    
    int isr_underrun;

    float read_time;
    int read_time_div;

    float isr_time;
    int isr_time_div;
    
    float current_time;
    
    int slow_count;

    //Timer isrtimer;

    //DigitalOut ext_flag;
    //DigitalOut run_flag;

};

#endif
