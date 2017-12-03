/**************************************************************
Case Study 5

1.PORTB
pin 0~pin 2:display the State(output)
pin 3: fault(output)
pin 4:-180° optical interrupter of the unipolar stepper(input)
pin 5:-270° optical interrupter of the unipolar stepper(input)
pin 6:-270° optical interrupter of the bipolar stepper(input)
pin 7:0° optical interrupter of the bipolar stepper(input)

2.PORTC
pin 0:red button(input)
pin 1:green button(input)

3.PORTD
pin 0~3:turn on the Port D pins in order(i.e. pin 0, pin 1, pin 2, pin 3), the unipolar stepper motor will rotate clockwise.(output)
pin 4:phase A(output)
pin 6:phase B(output)
turn on the Port D pin 4 hi followed by 6 hi, the stepper motor will rotate clockwise.
pin 5:Ia(output)
pin 7:Ib(output)

4.PORTE:
octal switch(input)
**************************************************************/
#include <xc.h>
#include <pic.h>
	#pragma config FOSC=HS, CP=OFF, DEBUG=OFF, BORV=20, BOREN=0, MCLRE=ON, PWRTE=ON, WDTE=OFF
	#pragma config BORSEN=OFF, IESO=OFF, FCMEN=0
/* Note: the format for the CONFIG directive starts with a double underscore.
The above directive sets the oscillator to an external high speed clock,
sets the watchdog timer off, sets the power up timer on, sets the system
clear on (which enables the reset pin) and turns code protect off. */
	static bit greenButton @ PORTBIT(PORTC,0);
	static bit redButton @ PORTBIT(PORTC,1);
	static bit UniH @ PORTBIT(PORTB,4); //the horizontal interrupter of the unipolar stepper
	static bit UniV @ PORTBIT(PORTB,5); //the vertical interrupter of the unipolar stepper
	static bit BiV 	@ PORTBIT(PORTB,6); //the vertical interrupter of the bipolar stepper
	static bit BiH	@ PORTBIT(PORTB,7); //the horizontal interrupter of the bipolar stepper
	
	
	char  i,Temp; // Variable for delay loop	
	char State = 0B00000000; //State is a variable for holding the state of the program
	
void SwitchDelay (void) 	// Waits for switch debounce
{
	for (i=200; i > 0; i--) {} 	// 1200 us delay
}


void SetupDelay(void) // Delay loop
{
	for (Temp=1; Temp > 0; Temp--) {} // 17 us delay
}

void initAtoD(void) // Initialize A/D
{
	ADCON1 = 0b00000100; // RA0,RA1,RA3 analog inputs, rest digital
	ADCON0 = 0b01000001; // Select 8* oscillator, analog input 0, turn on
	SetupDelay(); 		 // Delay a bit before starting A/D conversion
	GO = 1;				 // Start A/D
}

void main (void)
{	
	//init???about reset
	PORTB = 0B00000000; // Set PORTB low
	TRISB = 0B00001111; // Configure Port B pin 0~3 as outputs,pin 4~7 as inputs
	
	PORTC = 0B00000000; // Set PORTC low
	TRISC = 0B11111111;	// Configure Port C as all input???
	
	PORTD = 0B00000000; // Set PORTD low
	TRISD = 0B00000000; // Configure Port D as all output
	
	PORTE = 0B00000000; // Set PORTE low
	TRISE = 0B00001111; // Configure Port E as all inputs
	
	initAtoD;// Initialize A/D
	
	while(1) // Infinite loop
	{
		if(greenButton == 1) // If green press
		{
			while(greenButton == 1){} // Wait for release
			SwitchDelay(); // Let switch debounce
			State = A2D(); //put the mode we chose in the variable "State"
			Select(State);			
		}			
	}	
}

void Mode1(void)
{
	int Count,RealCount;
	PORTB=0B000000001;//make the LEDs show mode 1???
	
	Both2HorizontalUniCWBiCCW();//make the both motors to the horizontal position
	
	Count=0;
		
	while (GreenButton != 1)	//stay in mode 1 until the green button is pressed.
	{
		if(redButton == 1)
		{
			while(redButton == 1){} // Wait for release
			SwitchDelay(); // Let switch debounce
		
			Count++;
			RealCount = Count%4;//get the remainder of Count/4
		
			if(RealCount == 0)
			{
				while(UniV != 1){UniCCW();}
				//the unipolar motor will move counterclockwise (the shortest path) to the vertical interrupter and stop				
			}
		
			if(RealCount == 1)
			{
				while(BiV != 1){BiCW();}
				//the bipolar motor will move clockwise (the shortest path) to the vertical interrupter and stop
			}
		
			if(RealCount == 2)
			{
				while(UniH != 1){UniCW();}
				// the unipolar motor will move clockwise to the horizontal interrupter and stop
			}
	
			if(RealCount == 3)
			{
				while(BiH != 1){BiCCW();}
				//the bipolar motor will move counterclockwise to the horizontal interrupter and stop
			}
		}	
	}	
}	

void Mode2(void)
{
	PORTB=0B000000010;

	Both2HorizontalUniCWBiCCW();//make the both motors to the horizontal position
	
	while(GreenButton != 1)	//stay in mode 2 until the green button is pressed.
	{
		if(redButton == 1)
		{
			while(redButton == 1){} // Wait for release
			SwitchDelay(); // Let switch debounce
		
			// make a step of the bipolar motor before moving the unipolar
			PORTD=0B01000000;
			delay(30);
		
			Both2VerticalUniCCWBiCW();	//make the both motors to the vertical position
			Both2HorizontalUniCWBiCCW();//make the both motors to the horizontal position
			
			//if the red button is held down as the motors reach the horizontal interrupters,wait until it is released.
			if(redButton == 1)
			{
				while(redButton == 1){} // Wait for release
				SwitchDelay(); // Let switch debounce
			}				
	
		}				
	}	
} 

void Mode3(void)
{
	PORTB=0B000000011;//make the LEDs show mode 3
	Uni2VerticalBi2Horizontal();//make the unipolar stepper to the vertical position and the bipolar stepper to the horizontal position.
	
	while(PORTB,5 != 1)
	{
		UNI(1,1);
	}//The unipolar motor starts at the vertical interrupter position
	while(PORTB,6 != 1)
	{
		BI(1,1);
	}//The bipolar motor starts at the horizontal interrupter position. 
	
	while(1 != 2)
	{
		if(redButton == 1)
		{
			while(redButton == 1){} // Wait for release
			SwitchDelay(); // Let switch debounce
		
			BI(0,3);// make 3 steps of the bipolar motor before moving the unipolar
		
			while(PORTB,4&&PORTB,7 == 0)
			{
				if(PORTB,4 == 0)
				{
					UNI(0,1);
				}
				if(PORTB,7 == 0)
				{
					BI(0,1);
				}
			}
			
			
			while(PORTB,5&&PORTB,6 == 0)
			{
				if(PORTB,5 == 0)
				{
					UNI(1,1);
				}
				if(PORTB,6 == 0)
				{
					BI(1,1);
				}
			}	
		}
		
		if(greenButton == 1)
		{
			while(greenButton == 1){} // Wait for release
			SwitchDelay(); // Let switch debounce
				
			State = A2D();
			Select(State);
			
		}	

	}	
} 

void Mode4(void)
{
	PORTB=000000100;
	Both2HorizontalUniCWBiCCW();//make the both motors to the horizontal position
} 

void Both2HorizontalUniCWBiCCW(void)
{
	while (UniH&&BiH == 0)
	{
		if(UniH == 0){UniCW();}		
		if(BiH == 0){BiCCW();}
	}
}

void Both2VerticalUniCCWBiCW(void)
{
	while (UniV&&BiV == 0)
	{
		if(UniV == 0){UniCCW();}		
		if(BiV == 0){BiCW();}
	}
}

void Uni2VerticalBi2Horizontal(void)
{
	while(UniV&&BiH == 0)
	{
		if(UniV == 0){UniCW();}		
		if(BiH == 0){BiCW();}
	}
}

void Select(void)
{
	Swith(State)
			{
				case(0):
					Mode1();
					break;
				case(1)
					Mode2();
					break;
				case(2):
					Mode3();
					break;
				case(3):
					Mode4();
					break;
				default:
					Error(State);
	
}

void UniCW(void)
{
	PORTD=0B00000011;
	delay(30);
	PORTD=0B00000110;
	delay(30);
	PORTD=0B00001100;
	delay(30);
	PORTD=0B00001001;
	delay(30);
	return;
}

void UniCCW(void)
{
	PORTD=0B00001001;
	delay(30);
	PORTD=0B00001100;
	delay(30);
	PORTD=0B00000110;
	delay(30);
	PORTD=0B00000011;
	delay(30);
	return;
}

void BiCW(void)
{
	PORTD=0B00000000;
	delay(30);
	PORTD=0B00010000;
	delay(30);
	PORTD=0B01010000;
	delay(30);
	PORTD=0B01000000;
	delay(30);
	return;
}

void BiCCW(void)
{
	PORTD=0B01000000;
	delay(30);
	PORTD=0B01010000;
	delay(30);
	PORTD=0B00010000;
	delay(30);
	PORTD=0B00000000;
	delay(30);
	return;
}

void UniCWBiCW(void)
{
	PORTD=0B00000011;
	delay(30);
	PORTD=0B00010110;
	delay(30);
	PORTD=0B01011100;
	delay(30);
	PORTD=0B01001001;
	delay(30);
	return;
}

void UniCCWBiCCW(void)
{
	PORTD=0B01001001;
	delay(30);
	PORTD=0B01011100;
	delay(30);
	PORTD=0B00010110;
	delay(30);
	PORTD=0B00000011;
	delay(30);
	return;	
}

void UniCWBiCCW(void)
{
	PORTD=0B01000011;
	delay(30);
	PORTD=0B01010110;
	delay(30);
	PORTD=0B00011100;
	delay(30);
	PORTD=0B00001001;
	delay(30);
	return;
}

void UniCCWBiCW(void)
{
	PORTD=0B00001001;
	delay(30);
	PORTD=0B00011100;
	delay(30);
	PORTD=0B01010110;
	delay(30);
	PORTD=0B01000011;
	delay(30);
	return;
}

void UNIwave()
{
	
}
void BIwave()
{
	
}

char A2D(void)
{
	while(GO == 1){} // Wait here until A/D conversion is done
	while(GO == 1){} // Make sure A/D has finished
	return ADRESH;
}

void Error(State)
{
	PORTB,0 = State,0; 
	PORTB,1 = State,1;  
	PORTB,2 = State,2;  //nake the last 3 three pins of PORTB show the mode we chose
	PORTB,2 = 1;		//light the third LED for the fault
	while(1 != 2){};	//wait for the reset button pressed
}
