/**
* @author Daniel Worrall
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
*    I2S port abstraction library cpp file for NXP LPC1768
*
*/ 
#include "I2SSlave.h"
/*Global Functions*/
FunctionPointer akjnh3489v8ncv;

extern "C" void I2S_IRQHandler(void){                   //this is a very special function so can remain outside
    akjnh3489v8ncv.call();
}
/******************************************************
 * Function name:   I2SSlave
 *
 * Description:     class constructor
 *
 * Parameters:      PinName tx_sda, PinName tx_ws, PinName clk, PinName rx_sda, PinName rx_ws
 * Returns:         none
******************************************************/
//Constructor
I2SSlave::I2SSlave(PinName tx_sda, PinName tx_ws, PinName clk, PinName rx_sda, PinName rx_ws){
    storePins_(tx_sda, tx_ws, clk, rx_sda, rx_ws);
    format(16, STEREO);                                 //this also invokes initialize so no need to call it twice
}
//Public functions
/******************************************************
 * Function name:   format()
 *
 * Description:     sets the bit length for writing and stereo or mono mode
 *
 * Parameters:      int bit, bool mode
 * Returns:         none
******************************************************/
void I2SSlave::format(int bit, bool mode){
    bit_ = bit;
    mode_ = mode;
    initialize_(tx_sda_, tx_ws_, clk_, rx_sda_, rx_ws_);
}
/******************************************************
 * Function name:   write()
 *
 * Description:     load buffer to write to I2S port
 *
 * Parameters:      long *buffer
 * Returns:         none
******************************************************/
void I2SSlave::write(int* buffer, int from, int length){
    int to = from + length;
    for(int i = from; i < to; ++i){
        LPC_I2S->I2STXFIFO = buffer[i];
    }
}
/******************************************************
 * Function name:   start()
 *
 * Description:     attach streamOut_ function to ticker interrupt
 *
 * Parameters:      none
 * Returns:         none
******************************************************/
void I2SSlave::start(int mode){
    switch(mode){
        case(0):
             LPC_I2S->I2SIRQ |= (0 << 0);           //disable receive interrupt
             LPC_I2S->I2SIRQ |= (0 << 1);           //disable transmit interrupt
             break;
        case(1):
            LPC_I2S->I2SIRQ |= (0 << 0);            //disable receive interrupt
            LPC_I2S->I2SIRQ |= (1 << 1);            //enable transmit interrupt
            LPC_I2S->I2SIRQ |= (4 << 16);           //set I2STXFIFO depth to 0 words
            break;
        case(2):
            LPC_I2S->I2SIRQ |= (1 << 0);            //enable receive interrupt
            LPC_I2S->I2SIRQ |= (0 << 1);            //disable transmit interrupt
            LPC_I2S->I2SIRQ |= (4 << 8);            //set I2SRXFIFO depth to 4 words
            break;
        case(3):
            LPC_I2S->I2SIRQ |= (1 << 0);            //enable receive interrupt
            LPC_I2S->I2SIRQ |= (4 << 8);            //set I2SRXFIFO depth to 4 words
            LPC_I2S->I2SIRQ |= (1 << 1);            //enable transmit interrupt
            LPC_I2S->I2SIRQ |= (4 << 16);           //set I2STXFIFO depth to 4 words
            break;
        default:
            break;
    }
    NVIC_SetPriority(I2S_IRQn, 0);
    NVIC_EnableIRQ(I2S_IRQn);                       //enable I2S interrupt in the NVIC 
}
/******************************************************
 * Function name:   stop()
 *
 * Description:     detach streamOut_ from ticker
 *
 * Parameters:      none
 * Returns:         none
******************************************************/
void I2SSlave::stop(void){
    NVIC_DisableIRQ(I2S_IRQn);           
}
/******************************************************
 * Function name:   read()
 *
 * Description:     reads FIFORX buffer into [int32_t rxBuffer[8]]
 *
 * Parameters:      none
 * Returns:         none
******************************************************/
void I2SSlave::read(void){
    rxBuffer[0] = LPC_I2S->I2SRXFIFO;
    rxBuffer[1] = LPC_I2S->I2SRXFIFO;
    rxBuffer[2] = LPC_I2S->I2SRXFIFO;
    rxBuffer[3] = LPC_I2S->I2SRXFIFO;
}
/******************************************************
 * Function name:   status_()
 *
 * Description:     Read I2SSTATE register
 *
 * Parameters:      none
 * Returns:         int
******************************************************/
int I2SSlave::status(void){
    return LPC_I2S->I2SSTATE;
}
//Private functions
/******************************************************
 * Function name:   initialize()
 *
 * Description:     initialises I2S port 
 *
 * Parameters:      PinName tx_sda, PinName tx_ws, PinName clk, PinName rx_sda, PinName rx_ws
 * Returns:         0 =  successful initialisation
                   -1 = initialisation failure
******************************************************/
int I2SSlave::initialize_(PinName tx_sda, PinName tx_ws, PinName clk, PinName rx_sda, PinName rx_ws){
    setPins_(tx_sda, tx_ws, clk, rx_sda, rx_ws);        //designate pins
    LPC_SC->PCONP |= (1 << 27);
    //configure input/output register
    format_(bit_, mode_);           
    //set mbed as SLAVE  
    LPC_I2S->I2SDAO |= (1 << 5);
    LPC_I2S->I2SDAI |= (1 << 5);
    //clock mode
    setClocks_(4);
    //set slave mode
    modeConfig_();
    //set receiver mode
    LPC_I2S->I2SRXMODE |= (1 << 1);
    //slave mode
    LPC_I2S->I2STXRATE = 0;
    LPC_I2S->I2SRXRATE = 0;
    //Start
    LPC_I2S->I2SDAO |= (0 << 3);          
    LPC_I2S->I2SDAI |= (0 << 3);
    LPC_I2S->I2SDAO |= (0 << 4);          
    LPC_I2S->I2SDAI |= (0 << 4);
    LPC_I2S->I2SDAO |= (0 << 15);          
    return 0;
}
/******************************************************
 * Function name:   setClocks_()
 *
 * Description:     Set the division setting on the internal clocks
 *
 * Parameters:      int divideBy
 * Returns:         nothing
******************************************************/
void I2SSlave::setClocks_(int divideBy){
    switch(divideBy){
        case 1:
            LPC_SC->PCLKSEL1 |= (1 << 22); 
            LPC_SC->PCLKSEL1 |= (0 << 23);     
            break;
        case 2:
            LPC_SC->PCLKSEL1 |= (0 << 22);
            LPC_SC->PCLKSEL1 |= (1 << 23);
            break;
        case 4:
            LPC_SC->PCLKSEL1 |= (0 << 22);
            LPC_SC->PCLKSEL1 |= (0 << 23);
            break;
        case 8:
            LPC_SC->PCLKSEL1 |= (1 << 22);
            LPC_SC->PCLKSEL1 |= (1 << 23);
            break;
        default:
            break;
    }
}
/******************************************************
 * Function name:   setPins_()
 *
 * Description:     set external pin configuration
 *
 * Parameters:      PinName tx_sda, PinName tx_ws, PinName clk, PinName rx_sda, PinName rx_ws
 * Returns:         none
******************************************************/
void I2SSlave::setPins_(PinName tx_sda, PinName tx_ws, PinName clk, PinName rx_sda, PinName rx_ws){
    if(rx_ws == p29){
        LPC_PINCON->PINSEL0 |= (1 << 10);   //set p29 as receive word select line
    } else {
        LPC_PINCON->PINSEL1 |= (2 << 16);   //set p16 as receive word select line
    }
    if(rx_sda == p8){
        LPC_PINCON->PINSEL0 |= (1 << 12);   //set p8 as receive serial data line
    } else {
        LPC_PINCON->PINSEL1 |= (2 << 18);   //set p17 as receive serial data line
    }
    LPC_PINCON->PINSEL0 |= (1 << 14);       //set p7 as transmit clock line (only one of these)    
    LPC_PINCON->PINSEL0 |= (1 << 16);       //set p6 as word select line (only one of these)   
    LPC_PINCON->PINSEL0 |= (1 << 18);       //set p5 as transmit serial data line (only one of these)
    LPC_PINCON->PINSEL0 |= (0 << 8);        //clear rx_clk
}
/******************************************************
 * Function name:   format_()
 *
 * Description:     sets the bit length for writing and stereo or mono mode
 *
 * Parameters:      int bit, bool mode(STEREO 0, MONO 1)
 * Returns:         none
******************************************************/
void I2SSlave::format_(int bit, bool mode){
    uint32_t bps= ((bit+1)*8)-1;
    LPC_I2S->I2SDAO &= (0x00 << 6);
    LPC_I2S->I2SDAO |= (bps << 6);
    //set bit length
    switch(bit){
        case 8:
            LPC_I2S->I2SDAO &= 0xfffffffc;
            break;
        case 16:
            LPC_I2S->I2SDAO &= (0 << 1);
            LPC_I2S->I2SDAO |= (1 << 0);           
            break;
        case 32:
            LPC_I2S->I2SDAO &= (0 << 1);
            LPC_I2S->I2SDAO |= (3 << 0);
            break;
        default:
            break;
    }
    //set audio mode
    if(mode == STEREO){
        LPC_I2S->I2SDAO |= (0 << 2);
    } else {
        LPC_I2S->I2SDAO |= (1 << 2);
    }
    //set transmitter and receiver setting to be the same
    LPC_I2S->I2SDAI &= (0x00 << 6);
    LPC_I2S->I2SDAI |= (bps << 6);
    //set bit length
    switch(bit){
        case 8:
            LPC_I2S->I2SDAI &= 0xfffffffc;
            break;
        case 16:
            LPC_I2S->I2SDAI &= (0 << 1);
            LPC_I2S->I2SDAI |= (1 << 0);
            break;
        case 32:
            LPC_I2S->I2SDAI &= (0 << 1);
            LPC_I2S->I2SDAI |= (3 << 0);
            break;
        default:
            break;
    }
    //set audio mode
    if(mode == STEREO){
        LPC_I2S->I2SDAI |= (0 << 2);
    } else {
        LPC_I2S->I2SDAI |= (1 << 2);
    }
}
/******************************************************
 * Function name:   modeConfig_()
 *
 * Description:     Set slave mode
 *
 * Parameters:      none
 * Returns:         none
******************************************************/
void I2SSlave::modeConfig_(void){
    LPC_I2S->I2STXMODE |= (0x0 << 0);
    LPC_I2S->I2SRXMODE |= (0x0 << 0);
}
/******************************************************
 * Function name:   storePins_()
 *
 * Description:     set external pin configuration
 *
 * Parameters:      PinName tx_sda, PinName tx_ws, PinName clk, PinName rx_sda, PinName rx_ws
 * Returns:         none
******************************************************/
void I2SSlave::storePins_(PinName tx_sda, PinName tx_ws, PinName clk, PinName rx_sda, PinName rx_ws){
    tx_sda_ = tx_sda;
    tx_ws_ = tx_ws;
    clk_ = clk;
    rx_sda_ = rx_sda;
    rx_ws_ = rx_ws;
}
