#include<8051.h>
#include<stdio.h>
#include<lcd.c>
void disp_digits(int n);
__sfr __at (0x80) ADC_PORT;
 
unsigned int temp;
int thous,hundreds,tens,ones;
unsigned int getadc;
char strbuff[12];
void main(){
while(1){
getadc=ADC_PORT;
MSDelay(100);
LCD_init();
LCD_command(0x86);
temp=getadc*6;
if(temp>=1000)
{
ones=temp%10;
temp=temp/10;
tens=temp%10;
temp=temp/10;
hundreds=temp%10;
thous=temp/10;
sprintf(strbuff,"%d%d.%d%d Volt",thous,hundreds,tens,ones);
LCD_sendstring(strbuff);
}
else
{
ones=temp%10;
temp=temp/10;
tens=temp%10;
hundreds=temp/10;
sprintf(strbuff,"%d.%d%dV",hundreds,tens,ones);
LCD_sendstring(strbuff);
}
MSDelay(100);
}
}
