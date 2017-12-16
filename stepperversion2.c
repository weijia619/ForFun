#include <xc.h>
#include <pic.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>
 #pragma config FOSC=HS, CP=OFF, DEBUG=OFF, BORV=20, BOREN=0, MCLRE=ON, PWRTE=ON, WDTE=OFF
 #pragma config BORSEN=OFF, IESO=OFF, FCMEN=0
/* Note: the format for the CONFIG directive starts with a double underscore.
The above directive sets the oscillator to an external high speed clock,
sets the watchdog timer off, sets the power up timer on, sets the system
clear on (which enables the reset pin) and turns code protect off. */
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
/* Variable declarations */
#define PORTBIT(adr,bit) ((unsigned)(&adr)*8+(bit))
// The function PORTBIT is used to give a name to a bit on a port
// The variable RC0 could have equally been used
 static bit greenButton @ PORTBIT(PORTC,0);
 static bit redButton @ PORTBIT(PORTC,1);
 static bit UniH @ PORTBIT(PORTC,4); //the horizontal interrupter of the unipolar stepper
 static bit UniV @ PORTBIT(PORTC,5); //the vertical interrupter of the unipolar stepper
 static bit BiV  @ PORTBIT(PORTC,6); //the vertical interrupter of the bipolar stepper
 static bit BiH @ PORTBIT(PORTC,7); //the horizontal interrupter of the bipolar stepper
 
 
 
 unsigned char  Temp; // Variable for delay loop 
 int i;
 unsigned char State; //State is a variable for holding the state of the program
/*********mode*******/ 
 void Select(char);
 void Mode1(void);
 void Mode2(void);
 void Mode3(void);
 void Mode4(void);
 
/********A2D*********/
 /*void initAtoD(void);
 char A2D(void);
 void SetupDelay(void);*/
 
/*******functions about the motors********/
 void UniCW(void);
 void UniCCW(void);
 void BiCW(void);
 void BiCCW(void);
 
 void UniCWBiCW(void);
 void UniCCWBiCCW(void);
 void UniCWBiCCW(void);
 void UniCCWBiCW(void);
 
 void Both2H_UniCWBiCCW(void);
 void Both2V_UniCCWBiCW(void);
 void Uni2VBi2H_UniCWBiCW(void);
 void Uni2HBi2V_UniCCWBiCCW(void);
 
 void UniCW_wave(void);
 void BiCW_wave(void);
 void UniCCW_wave(void);
 void BiCCW_wave(void);
 
 void Both2H_UniCWBiCCW_wave(void);
 void Both2V_UniCWBiCCW_wave(void);
 
/*******************************************/
 void SwitchDelay(void);
 
 void Error(char);
 void delay(int);
 
/*************************************************/
/********************** MAIN *********************/
/*************************************************/
void main (void)
{ 
 //init???about reset
 PORTB = 0B00000000; // Set PORTB low
 TRISB = 0B11110000; // Configure Port B pin 0~3 as outputs,pin 4~7 as inputs
 
 PORTC = 0B00000000; // Set PORTC low
 TRISC = 0B11110011; // Configure Port C as all input???
 
 PORTD = 0B00000000; // Set PORTD low
 TRISD = 0B00000000; // Configure Port D as all output
 
 PORTE = 0B00000000; // Set PORTE low
 TRISE = 0B00000111; // Configure Port E as all inputs
 
 ADCON1= 0B00001010;
 
 //Both2H_UniCWBiCCW();
 PORTB = 0b11111111;     // TEST TEST TEST TES TES TES TES TEST 
 while(1) // Infinite loop
 {
  if(greenButton == 1) // If green press
  {
   while(greenButton == 1){} // Wait for release
   SwitchDelay(); // Let switch debounce
   State = ~PORTE;
 State=State&0B00000111; //put the mode we chose in the variable "State"
   Select(State);   
  }   
 } 
}
/*************************************************/
/********************  MODE 1  *******************/
/*************************************************/
void Mode1(void)
{
 int Count,RealCount;
 PORTB=0B00000001;//make the LEDs show mode 1
 
 Both2H_UniCWBiCCW();//make the both motors to the horizontal position
 
 Count=0;
  
 while (greenButton != 1) //stay in mode 1 until the green button is pressed.
 {
  if(redButton == 1)
  {
   while(redButton == 1){} // Wait for release
   SwitchDelay(); // Let switch debounce
  
   Count++;
   RealCount = Count%4;//get the remainder of Count/4
  
   if(RealCount == 1)
   {
    while(UniV != 1){UniCCW();}
    //the unipolar motor will move counterclockwise (the shortest path) to the vertical interrupter and stop    
   }
  
   if(RealCount == 2)
   {
    while(BiV != 1){BiCW();}
    //the bipolar motor will move clockwise (the shortest path) to the vertical interrupter and stop
   }
  
   if(RealCount == 3)
   {
    while(UniH != 1){UniCW();}
    // the unipolar motor will move clockwise to the horizontal interrupter and stop
   }
 
   if(RealCount == 0)
   {
    while(BiH != 1){BiCCW();}
    //the bipolar motor will move counterclockwise to the horizontal interrupter and stop
   }
  } 
 } 
} 
/*************************************************/
/********************  MODE 2  *******************/
/*************************************************/
void Mode2(void)
{
 PORTB=0B00000010;
 Both2H_UniCWBiCCW();//make the both motors to the horizontal position
 
 while(greenButton != 1) //stay in mode 2 until the green button is pressed.
 {
  if(redButton == 1)
  {
   while(redButton == 1){} // Wait for release
   SwitchDelay(); // Let switch debounce
  
   // make a step of the bipolar motor before moving the unipolar
   //PORTD=0B01000000;
   //delay(30);
   
   Both2V_UniCCWBiCW(); //make the both motors to the vertical position
 
   Both2H_UniCWBiCCW(); //make the both motors to the horizontal position
   
   //if the red button is held down as the motors reach the horizontal interrupters,wait until it is released.
   if(redButton == 1)
   {
    while(redButton == 1){} // Wait for release
    SwitchDelay(); // Let switch debounce
   }    
  }    
 } 
}
/*************************************************/
/********************  MODE 3  *******************/
/*************************************************/
void Mode3(void)
{
 PORTB=0B00000011;//make the LEDs show mode 3
 Uni2VBi2H_UniCWBiCW();//make the unipolar stepper to the vertical position and the bipolar stepper to the horizontal position. 
 
 while(greenButton != 1)
 {
  if(redButton == 1)
  {
   while(redButton == 1){} // Wait for release
   SwitchDelay(); // Let switch debounce
     
   Uni2HBi2V_UniCCWBiCCW();
   
   Uni2VBi2H_UniCWBiCW();
   
   //if the red button is held down as the motors reach the horizontal interrupters,wait until it is released.
   if(redButton == 1)
   {
    while(redButton == 1){} // Wait for release
    SwitchDelay(); // Let switch debounce
   }  
  }
 } 
}
/*************************************************/
/********************  MODE 4  *******************/
/*************************************************/
void Mode4(void)
{
 PORTB=0B00000100;
 Both2H_UniCWBiCCW();//make the both motors to the horizontal position
 
 while(greenButton != 1) //stay in mode 2 until the green button is pressed.
 {
  if(redButton == 1)
  {
   while(redButton == 1){} // Wait for release
   SwitchDelay(); // Let switch debounce
   Both2V_UniCWBiCCW_wave(); //make the both motors to the vertical position by wave drive
 
   Both2H_UniCWBiCCW_wave(); //make the both motors to the horizontal position by wave drive
   
   //if the red button is held down as the motors reach the horizontal interrupters,wait until it is released.
   if(redButton == 1)
   {
    while(redButton == 1){} // Wait for release
    SwitchDelay(); // Let switch debounce
   }    
  }    
 } 
}
void Select(char)
{
 switch(State)
   {
    case(1):
     Mode1();
     break;
    case(2):
     Mode2();
     break;
    case(3):
     Mode3();
     break;
    case(4):
     Mode4();
     break;
    default:
    Error(State);
 }
}
void UniCW(void)
{
 Temp = PORTD&0B11110000;
 PORTD = Temp|0B00000011;
 delay(30);
 Temp = PORTD&0B11110000;
 PORTD=Temp|0B00000110;
 delay(30);
 Temp = PORTD&0B11110000;
 PORTD=Temp|0B00001100;
 delay(30);
 Temp = PORTD&0B11110000;
 PORTD=Temp|0B00001001;
 delay(30);
}
void UniCCW(void)
{
 Temp = PORTD&0B11110000;
 PORTD=Temp|0B00001001;
 delay(30);
 Temp = PORTD&0B11110000;
 PORTD=Temp|0B00001100;
 delay(30);
 Temp = PORTD&0B11110000;
 PORTD=Temp|0B00000110;
 delay(30);
 Temp = PORTD&0B11110000;
 PORTD=Temp|0B00000011;
 delay(30);
}
void BiCW(void)
{
 Temp = PORTD&0B00001111;
 PORTD=Temp|0B00000000;
 delay(30);
 Temp = PORTD&0B00001111;
 PORTD=Temp|0B00010000;
 delay(30);
 Temp = PORTD&0B00001111;
 PORTD=Temp|0B01010000;
 delay(30);
 Temp = PORTD&0B00001111;
 PORTD=Temp|0B01000000;
 delay(30);
}
void BiCCW(void)
{
 Temp = PORTD&0B00001111;
 PORTD=Temp|0B01000000;
 delay(30);
 Temp = PORTD&0B00001111;
 PORTD=Temp|0B01010000;
 delay(30);
 Temp = PORTD&0B00001111;
 PORTD=Temp|0B00010000;
 delay(30);
 Temp = PORTD&0B00001111;
 PORTD=Temp|0B00000000;
 delay(30);
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
}
 void Both2H_UniCWBiCCW(void)
{
 while ((UniH!=1 )||(BiH != 1))
 {
  if(UniH != 1){UniCW();}  
  if(BiH != 1){BiCCW();}
 }
}
void Both2V_UniCCWBiCW(void)
{
 while ((UniV!=1 )||(BiV != 1))
 {
  if(UniV != 1){UniCCW();}  
  if(BiV != 1){BiCW();}
 }
}
void Uni2VBi2H_UniCWBiCW(void)
{
 while((UniV!=1 )||(BiH != 1))
 {
  if(UniV != 1){UniCW();}  
  if(BiH != 1){BiCW();}
 }
}
void Uni2HBi2V_UniCCWBiCCW(void)
{
 while((UniH!=1 )||(BiV != 1))
 {
  if(UniH != 1){UniCCW();}  
  if(BiV != 1){BiCCW();}
 }
}
void UniCW_wave(void)
{
 PORTD=0B00000001;
 delay(30);
 PORTD=0B00000010;
 delay(30);
 PORTD=0B00000100;
 delay(30);
 PORTD=0B00001000;
 delay(30);
}
void BiCW_wave(void)
{
 PORTD=0B11000000;
 delay(30);
 PORTD=0B00110000;
 delay(30);
 PORTD=0B10010000;
 delay(30);
 PORTD=0B01100000;
 delay(30);
}
void UniCCW_wave(void)
{
 PORTD=0B00001000;
 delay(30);
 PORTD=0B00000100;
 delay(30);
 PORTD=0B00000010;
 delay(30);
 PORTD=0B00000001;
 delay(30);
}
void BiCCW_wave(void)
{
 PORTD=0B01100000;
 delay(30);
 PORTD=0B10010000;
 delay(30);
 PORTD=0B00110000;
 delay(30);
 PORTD=0B11000000;
 delay(30);
}
void Both2H_UniCWBiCCW_wave(void)
{
 while (UniH&&BiH == 0)
 {
  if(UniH == 0){UniCW_wave();}  
  if(BiH == 0){BiCCW_wave();}
 }
}
void Both2V_UniCWBiCCW_wave(void)
{
 while (UniV&&BiV == 0)
 {
  if(UniV == 0){UniCCW_wave();}  
  if(BiV == 0){BiCW_wave();}
 }
}
void SwitchDelay (void)  // Waits for switch debounce
{
 for (i=200; i > 0; i--) {}  // 1200 us delay
}
void SetupDelay(void) // Delay loop
{
 for (Temp=1; Temp > 0; Temp--) {} // 17 us delay
}
void Error(char State)
{
 PORTB=State+0B00001000;
 while(1 != 2){} //wait for the reset button pressed
}
void delay(int n)
{
    int j;
 for(j=1;j<2000;j++)
 {}
}
