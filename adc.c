
Program for ADC:
#include<reg51.h>
#define input P0 //Input port to read the values of ADc
#define output P2 // Output port, connected to LED's.

sbit wr= P1^1; // Write pin. It is used to start the conversion.
sbit rd= P1^0; // Read pin. It is used to extract the data from internal register to the output pins of ADC.
sbit intr= P1^2; // Interrupt pin. This is used to indicate the end of conversion. It goes low when conversion is complete.

void delay(unsigned int msec ) // The delay function provides delay in msec.
{ int i,j ;
for(i=0;i<msec;i++) for(j=0; j<1275; j++);
}

void adc() // Function to read the values from ADC and display on the LED's.
{ rd=1;
wr=0;
delay(1);
wr=1;
while(intr==1);
rd=0;
output=input;
delay(1);
intr=1;
} void main()
{ input=0xff; // Declare port 0 as input port.
while(1)
{ adc(); }
}
