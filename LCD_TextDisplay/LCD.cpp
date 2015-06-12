#include "LCD.h"

//LCD Class Constructor for polling A2D using int ADC0848::Poll_A2D(unsigned char Channel)
LCD::LCD(PinName pin1, PinName pin2, PinName pin3, PinName pin4, PinName pin5, PinName pin6, PinName pin7, PinName pin8, PinName pin9, PinName pin10) 
								:_DataBus(pin1,pin2,pin3,pin4,pin5,pin6,pin7,pin8), _RS(pin9), _Enable(pin10)
{
	Initialize();
}

void LCD::Initialize()
 {
  wait(.01); 
  _RS = 0;
	_Enable = 0;	 //EN and RS = 0
  wait(.001);
  WriteLCDCmd(0x38);       //Required for initialization    
  wait(.001);
  WriteLCDCmd(0x38);       //Required for initialization    
  wait(.001);
  WriteLCDCmd(0x38);       //Required for initialization    
  wait(.001);
  WriteLCDCmd(0x38);       //2 rows x 16 cols 5x7 dot char    
  wait(.001);
  WriteLCDCmd(0x0C);       //Display on, cursor off
  wait(.001);
  WriteLCDCmd(0x01);       //Cursor home
  wait(.001);
  WriteLCDCmd(0x06);       //incr cursor and shift on
  wait(.001);
	wait(.1);	 
  return;
 }

void LCD::WriteLCDData(char c, char Pos)
 {
  WriteLCDCmd(Pos);
	_DataBus.output();
  wait(.001);
  _DataBus  = c;                    //Data to LCD Data
  _RS = 1;
	_Enable = 1;//RS = EN = 1
  wait(.001);
  _Enable = 0;            //EN = 0
  return;
 }

void LCD::WriteLCDCmd(char c)
 {
	_DataBus.output();
  _DataBus = c;              //Data to LCD data
  _RS = 0;      //RS = 0
  _Enable = 1;      //En = 1
  wait(.010);
  _Enable = 0;      //En = 0
  return;
 }
 
 void LCD::ClearDisplay()
  {
   WriteLCDCmd(0x01);         // clear display
  }

	
void LCD::Print_String(char* Pbuffer,char line_num, char offset)
	{
		unsigned int i=0;
		char pos = 0;
		switch (line_num)
		{
			case(1):
				pos = 0x80+offset;
				break;
			case(2):
				pos = 0xC0+offset;
				break;
			case(3):
				pos = 0x94+offset;
				break;
			case(4):
				pos = 0xD4+offset;
				break;
			default:
				break;			
		}
		while (Pbuffer[i] !=  0)
    {
        WriteLCDData(Pbuffer[i], pos+i);  
        i++;    
    }
}
