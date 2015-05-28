
#include "LCD.h"
#include "stdio.h"

Serial pc(USBTX,USBRX);
char msgInit[] = "Display ready";
////////////D0--------------------------D7, RS, Enable
LCD Display(p13,p14,p15,p16,p17,p18,p19,p20,p12,p10);

int main() {
	int count;
	Display.ClearDisplay();
	Display.Print_String(msgInit);
	wait(5);
	Display.ClearDisplay();
	sprintf(msgInit,"Line 1: Hello");	
	Display.Print_String(msgInit);
	sprintf(msgInit,"Line 2: Mom");
  Display.Print_String(msgInit,2);
	sprintf(msgInit,"Line 3: How are");
	Display.Print_String(msgInit,3);
	sprintf(msgInit,"Line 4: You");
	Display.Print_String(msgInit,4);
	wait(10);
  Display.ClearDisplay();
    while(1) //Do what ever is between these brackets forever or until the micro is reset.
			{
				for(count=0;count<5000;count++)
				{
					sprintf(msgInit,"I=%d",count);	
					Display.Print_String(msgInit,1,5);
					wait(.5);
				}
				
			}	
}


