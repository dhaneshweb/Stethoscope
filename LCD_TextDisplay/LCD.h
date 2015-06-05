#include "mbed.h"

class LCD {
public:					//Strobe			//CLK					//Data
 
 LCD(PinName pin1, PinName pin2, PinName pin3, PinName pin4, PinName pin5, PinName pin6, PinName pin7, PinName pin8, PinName pin9, PinName pin10); 
		void ClearDisplay();
		void Print_String(char* Pbuffer,char line_num=1, char offset=0);



private:

	BusInOut _DataBus;
  DigitalOut 		_RS;
	DigitalOut 		_Enable;

		void Initialize();
		void WriteLCDData(char c, char Pos);
		void WriteLCDCmd(char c);


};
