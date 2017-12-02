/*
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
*/

#include <xc.h>
#include <pic.h>
	#pragma config FOSC=HS, CP=OFF, DEBUG=OFF, BORV=20, BOREN=0, MCLRE=ON, PWRTE=ON, WDTE=OFF
	#pragma config BORSEN=OFF, IESO=OFF, FCMEN=0

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
	//init
	//PORTB = 0B00000000; // Clear Port B output latches
	TRISB = 0B00001111; // Configure Port B pin 0~3 as outputs,pin 4~7 as inputs
	
	TRISC = 0B11111111;	// Configure Port C as all input
	
	PORTD = 0B00000000; // Clear Port D output latches
	TRISD = 0B00000000; // Configure Port D as all output
	
	initAtoD;
	
	while(1 != 2) // Infinite loop
	{
		if(greenButton == 1) // If green press
		{
			while(greenButton == 1){} // Wait for release
			SwitchDelay(); // Let switch debounce
			
			while(GO == 1){} // Wait here until A/D conversion is done
			while(GO == 1){} // Make sure A/D has finished
			State = ADRESH; // 
			Swith(State)
			{
				case(0):
					Mode1();
					break;
				case(2)
					Mode2();
					break;
				case(2):
					Mode3();
					break;
				case(3):
					Mode4();
					break;
				default:
					Error();
				}
			
		}
			
	}
	
	
	
}

void 	
