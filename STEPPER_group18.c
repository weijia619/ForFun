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
	
	char  i,Temp; // Variable for delay loop	
	State = 0B00000000; //State is a variable for holding the state of the program
	
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
	//init???
	//PORTB = 0B00000000; // Clear Port B output latches???
	TRISB = 0B00001111; // Configure Port B pin 0~3 as outputs,pin 4~7 as inputs
	
	TRISC = 0B11111111;	// Configure Port C as all input
	
	PORTD = 0B00000000; // Clear Port D output latches
	TRISD = 0B00000000; // Configure Port D as all output
	
	int Count,RealCount;
	
	initAtoD;// Initialize A/D
	
	while(1 != 2) // Infinite loop
	{
		if(greenButton == 1) // If green press
		{
			while(greenButton == 1){} // Wait for release
			SwitchDelay(); // Let switch debounce
			
			while(GO == 1){} // Wait here until A/D conversion is done
			while(GO == 1){} // Make sure A/D has finished
			State = ADRESH; // put the value of the octal switch in the variable "State"
			//State = A2D()
			Select(State);
			/*Swith(State)
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
				}*/
			
		}
			
	}
	
	
	
}

void Mode1(void)
{
	PORTB=000000001;//make the LEDs show mode 1???
	
	Synchronize()
	
	Count=0;
	
	while(1 != 2)
	{
		if (redButton == 1)
		{
			while(redButton == 1){} // Wait for release
			SwitchDelay(); // Let switch debounce
			
			Count++;
			RealCount = Count%4;//get the remainder of Count/4
			
			if(RealCount == 0)
			{
				while(PORTB,5 != 1)
				{
					UNI(0,1);	//the unipolar motor will move counterclockwise (the shortest path) to the vertical interrupter and stop
					//delay(30);	//need more than 30 msec between steps to keep synchronization.
				}
				//UNI(0,24);
			}
			
			if(RealCount == 1)
			{
				while(PORTB,6 != 1)
				{
					BI(1,1);	//the bipolar motor will move clockwise (the shortest path) to the vertical interrupter and stop
					//delay(30);	//need more than 30 msec between steps to keep synchronization.
				}
				//BI(1,25);
			}
			
			if(RealCount == 2)
			{
				while(PORTB,4 != 1)
				{
					UNI(1,1);	// the unipolar motor will move clockwise to the horizontal interrupter and stop
					//delay(30);	//need more than 30 msec between steps to keep synchronization.
				}
				//UNI(1,24);
			}
			
			if(RealCount == 3)
			{
				while(PORTB,7 != 1)
				{
					BI(0,1);	//the bipolar motor will move counterclockwise to the horizontal interrupter and stop
					//delay(30);	//need more than 30 msec between steps to keep synchronization.
				}
				//BI(0,25);
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

void Mode2(void)
{
	PORTB=000000010;

	Synchronize();
	
	while(1 != 2)
	{
		if(redButton == 1)
		{
			while(redButton == 1){} // Wait for release
			SwitchDelay(); // Let switch debounce
		
			BI(1,1);// make a step of the bipolar motor before moving the unipolar
		
			while(PORTB,4&&PORTB,7 == 0)
			{
				if(PORTB,4 == 0)
				{
					UNI(1,1);
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
					UNI(0,1);
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

void Mode3(void)
{
	PORTB=000000011;
	//Synchronize();
	
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
	Synchronize();
} 

void Synchronize(void)
{
	while (PORTB,4&&PORTB,7 == 0)
	{
		if(PORTB,4 == 0)
		{
			UNI(1,1);
		}
		if(PORTB,7 == 0)
		{
			BI(1,1);
		}
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


void UNI(UniDir,UniSteps)
{
	//leftshift
	//delay(30) every step
}
void BI(BiDir,BiSteps)
{
}

void UNIwave()
{
}
void BIwave()
{
}

int A2D(void)
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
