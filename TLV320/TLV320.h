/**
* @author Ioannis Kedros, Daniel Worrall
*
* @section LICENSE
*
* Copyright (c) 2011 mbed
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
*    Library for Texas instruments TLV320AIC23B library NXP LPC1768
*
*/

#ifndef MBED_TLV320_H
#define MBED_TLV320_H

#include "mbed.h"
#include "I2SSlave.h"

/** TLV320 class, defined on the I2C master bus
*
*/

class TLV320
{        
    public:
        //constructor
        /** Create a TLV320 object defined on the I2C port
        *  
        * @param sda Serial data pin (p9 or p28)
        * @param scl Serial clock pin (p10 or p27)
        * @param addr Object address
        */
        TLV320(PinName sda, PinName scl, int addr, PinName tx_sda, PinName tx_ws, PinName clk, PinName rx_sda, PinName rx_ws);
        /** Power up/down
        *
        * @param powerUp 0 = power down, 1 = power up
        */
        void power(bool powerUp);
        /** Overloaded power() function default = 0x07, record requires 0x02
        *
        * @param device Call individual devices to power up/down
        * Device power      0x00 = On 0x80 = Off
        * Clock             0x00 = On 0x40 = Off
        * Oscillator        0x00 = On 0x20 = Off
        * Outputs           0x00 = On 0x10 = Off
        * DAC               0x00 = On 0x08 = Off
        * ADC               0x00 = On 0x04 = Off
        * Microphone input  0x00 = On 0x02 = Off
        * Line input        0x00 = On 0x01 = Off
        */
        void power(int device);
        /** Set I2S interface bit length and mode
        *
        * @param length Set bit length to 16, 20, 24 or 32 bits
        * @param mode Set STEREO (0), MONO (1)
        */
        void format(char length, bool mode);
        /** Set sample frequency
         *
         * @param frequency Sample frequency of data in Hz
         * @return Returns an integer 0 = success, -1 = unrecognnised frequency
         * 
         * The TLV320 supports the following frequencies: 8kHz, 8.021kHz, 32kHz, 44.1kHz, 48kHz, 88.2kHz, 96kHz
         * Default is 44.1kHz
         */
        int frequency(int hz);
        /** Reset TLV320
        *
        */
        void reset(void);
        /** Start streaming i.e. enable interrupts
         *
         * @param mode Enable interrupts for NONE, TRANSMIT only, RECEIVE only, BOTH
         */
        void start(int mode);
        /** Stop streaming i.e. disable all interrupts
         *
         */
        void stop(void);
		
		int get_txFIFO_level();
		int get_rxFIFO_level();
		
        /** Write [length] 32 bit words in buffer to I2S port
         *
         * @param *buffer Address of buffer to be written
         * @param from Start position in buffer to read from
         * @param length Number of words to be written (MUST not exceed 4)
         */
        void write(int *buffer, int from, int length);
        /** Read 4 x (32bit) words into rxBuffer
         *
         */
        void read(void);
        /** Attach a void/void function or void/void static member function to an interrupt generated by the I2SxxFIFOs
         *
         * @param function Function to attach
         *
         * e.g. <code> myTlv320Object.attach(&myfunction);</code>
         * OR  <code> myTlv320Object.attach(&myClass::myStaticMemberFunction);</code>
         */
        void attach(void(*fptr)(void));
        /** Attach a nonstatic void/void member function to an interrupt generated by the I2SxxFIFOs
         *
         * @param tptr Object pointer
         * @param mptr Member function pointer
         *
         * e.g. <code>myTlv320Object.attach(&myObject, &myClass::myNonstaticMemberFunction);</code> where myObject is an object of myClass
         */
        template<typename T>
        void attach(T *tptr, void(T::*mptr)(void)){
            mI2s_.attach(tptr, mptr);
        }
                /** Line in volume control i.e. record volume
        *
        * @param leftVolumeIn Left line-in volume 
        * @param rightVolumeIn Right line-in volume
        * @return Returns 0 for success, -1 if parameters are out of range
        * Parameters accept a value, where 0.0 < parameter < 1.0 and where 0.0 maps to -34.5dB 
        * and 1.0 maps to +12dB (0.74 = 0 dB default).
        */
        int lineInputVolume(float leftVolumeIn, float rightVolumeIn);
        /** Headphone out volume control
        *
        * @param leftVolumeOut Left line-out volume
        * @param rightVolumeOut Right line-out volume
        * @return Returns 0 for success, -1 if parameters are out of range
        * Parameters accept a value, where 0.0 < parameter < 1.0 and where 0.0 maps to -73dB (mute) 
        * and 1.0 maps to +6dB (0.5 = default)
        */
        int outputVolume(float leftVolumeOut, float rightVolumeOut);
       /** Analog audio path control
        *
        * @param bypassVar Route analogue audio direct from line in to headphone out
        */
        void bypass(bool bypassVar);
		
		void openMicInput(bool sidetoneOn);
		
        /**Digital audio path control
        *
        * @param softMute Mute output
        */
        void mute(bool softMute);
        //Receive buffer
        
        int *rxBuffer;
        
    protected:
        char cmd[2];    //the address and command for TLV320 internal registers
        int mAddr;      //register write address
    private:
        I2C mI2c_;      //MUST use the I2C port
        I2SSlave mI2s_;
        Ticker I2sTick;
        void io(void);   
       /** Sample rate control
        *
        * @param rate Set the sampling rate as per datasheet section 3.3.2
        * @param clockIn Set the clock in divider MCLK, MCLK_DIV2
        * @param clockMode Set clock mode CLOCK_NORMAL, CLOCK_USB
        */
        void setSampleRate_(char rate, bool clockIn, bool mode, bool bOSR); 
       /** Digital interface activation
        *
        */
        void activateDigitalInterface_(void);
        /** Digital interface deactivation
        *
        */
        void deactivateDigitalInterface_(void);

        //TLV320AIC23B register addresses as defined in the TLV320AIC23B datasheet
        #define LEFT_LINE_INPUT_CHANNEL_VOLUME_CONTROL  (0x00 << 1)
        #define RIGHT_LINE_INPUT_CHANNEL_VOLUME_CONTROL (0x01 << 1)
        #define LEFT_CHANNEL_HEADPHONE_VOLUME_CONTROL   (0x02 << 1)
        #define RIGHT_CHANNEL_HEADPHONE_VOLUME_CONTROL  (0x03 << 1)
        #define ANALOG_AUDIO_PATH_CONTROL               (0x04 << 1)
        #define DIGITAL_AUDIO_PATH_CONTROL              (0x05 << 1)
        #define POWER_DOWN_CONTROL                      (0x06 << 1)
        #define DIGITAL_AUDIO_INTERFACE_FORMAT          (0x07 << 1)
        #define SAMPLE_RATE_CONTROL                     (0x08 << 1)
        #define DIGITAL_INTERFACE_ACTIVATION            (0x09 << 1)
        #define RESET_REGISTER                          (0x0F << 1)
        
        #define CLOCK_NORMAL        0
        #define CLOCK_USB           1
        #define MCLK                0
        #define MCLK_DIV2           1
		
		#define POWER_BASE			(0x0F)
		#define DAC_ON				0x07
		#define ADC_ON				0x0B
		#define MIC_ON				0x0D
		#define LIN_ON				0x0E
};

#endif
