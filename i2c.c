Program for AT24C04:
#include <REG52.h>

unsigned char data DEVICEADD;

sbit SDA = P1^0;
sbit SCL = P1^1;

int data c;
char data wrom;
char data rrom;


void outchar(char data chr)
{
while(!TI)
{
}
TI = 0;
SBUF = chr;
}

void outstr(char* str)
{
while(*str != 0)
{
outchar(*str++);
}
}
char i2c_start(void)
{
/*
if(!SDA || !SCL)
return 0; // i2c_start error: bus not free
*/
// Both bus lines are high,
SDA = 0; // so set data line low to generate a start condition.
SCL = 0;
/*
SDA = 1;
SCL = 1;
SDA = 0;
SCL = 0;
*/
return 1;
}

void i2c_stop(void)
{
/*
SDA = 0;
SCL = 1;
SDA = 1;
*/
unsigned char input_var;
SCL = 1;
SDA = 0;
SCL = 1;
SDA = 1;
input_var = SDA;
}

// return ack value: 0 for fale; 1 for success
char i2c_write(char data byte)
{
int data i;
for(i = 0; i < 8 ; i++)
{
SDA = ((byte & 0x80) ? 1 : 0);
SCL = 1; // Ensure that the data is stable during the high period.
byte <<= 1;
SCL = 0;
}
// Get ack
SDA = 1;
SCL = 1;
i = SDA;
//i = 500;
SCL = 0;
/*
while(i-- && SDA)
{
}
*/
return !i; // Get data at end of cycle.
}

char i2c_writebyte(int data address, char data byte)
{
if(!i2c_start())
{
outstr("i2c_start error: bus not free.\n");
return 0;
}
if(!i2c_write(DEVICEADD))
{
i2c_stop();
i2c_start();
if(!i2c_write(DEVICEADD)) // Interface failed to acknowledge device address
{
i2c_stop();
outstr("Interface failed to acknowledge device address.\n");
return 0;
}
}

if(!i2c_write(address >>8)) // Interface failed to acknowledge byteH address
{
i2c_stop();
outstr("Interface failed to acknowledge byteH address.\n");
return 0;
}
if(!i2c_write(address)) // Interface failed to acknowledge byteL address
{
i2c_stop();
outstr("Interface failed to acknowledge byteL address.\n");
return 0;
}
if(!i2c_write(byte)) // Interface failed to acknowledge data
{
i2c_stop();
outstr("Interface failed to acknowledge data.\n");
return 0;
}
i2c_stop();
return 1; // success
}

char i2c_read(char data ACK)
{
char index, byte;
index = SDA; // Put data pin into read mode
byte = 0x00;
for(index = 0; index < 8; index++) // Send 8 bits to the I2C Bus
{
byte <<= 1;
SCL = 1; // Clock the data into the I2C Bus
byte |= SDA; // Input the data from the I2C Bus
SCL = 0;
}

// Write ack bit
SDA = ACK;
SCL = 1; // Ensure that the data is stable during the high period.
SCL = 0;
return byte;
}

char i2c_readbyte(int data address)
{
char data byte;
if(!i2c_start())
{
outstr("i2c_start error: bus not free.\n");
return 0;
}
if(!i2c_write(DEVICEADD))
{
i2c_stop();
i2c_start();
if(!i2c_write(DEVICEADD)) // Interface failed to acknowledge device address
{
i2c_stop();
outstr("Interface failed to acknowledge device address.\n");
return 0;
}
}

// Transmit address to read from.
if(!i2c_write(address >> 8)) // Interface failed to acknowledge byteH address
{
i2c_stop();
outstr("Interface failed to acknowledge byteH address. \n");
return 0;
}
if(!i2c_write(address)) // Interface failed to acknowledge byteL address
{
i2c_stop();
outstr("Interface failed to acknowledge byteL address. \n");
return 0;
}

// Now terminate write operation and do a read.
if(!i2c_start())
{
outstr("i2c_start error: bus not free.\n");
return 0;
}
// Transmit address to read from.
if(!i2c_write(DEVICEADD | 1)) // Slave failed to ack read access
{
i2c_stop();
outstr("Slave failed to ack read access.\n");
return 0;
}
// Read from address and ack = 1.
byte = i2c_read(1);
i2c_stop();
return byte;
}

char get_char()
{
while(!RI)
{
}
RI = 0;
return SBUF;
}


void WriteData()
{
char data ch;
ch = get_char();
while(ch != 0x0D)
{
i2c_writebyte(c++,ch);
outchar(ch);
ch = get_char();
}
wrom = 0;
ES = 1;
}

void ReadData()
{
char data index;
char data input;
for(index = 0; index < c; index++)
{
input = i2c_readbyte(index);
if(input != 0)
outchar(input);
}
c = 0;
rrom = 0;
ES = 1;
}

void main(void)
{
// Init serial
SCON = 0x52;
// Set timer1 mode and reload value
TMOD &= ~0xF0; // clear timer 1 mode bits
TMOD |= 0x20; // put timer 1 into MODE 2 8 bit auto reload
TH1 = 0xFD;
TR1 = 1; // start timer 1
TI = 0;
ES = 1; // Enable Serial interrupt
EA = 1; // Enable all interrupts

c = 0;
rrom = 0;
wrom = 0;
DEVICEADD = 0xA0; // 10100000B
while(1)
{
if(wrom == 1)
{
outchar('W');
WriteData();
}
if(rrom == 1)
{
outchar('R');
ReadData();
}
/*
count = get_char();
outchar(count);
*/
}
}

void serial (void) interrupt 4 using 3
{
char data inp;
if(RI) // Received data interrupt
{
RI = 0;
inp = (char)SBUF;
if((inp == 'w') || (inp == 'W'))
{
ES = 0;
rrom = 0;
c = 0;
TI = 1;
wrom = 1;
}
else if((inp == 'r') || (inp == 'R'))
{
ES = 0;
wrom = 0;
TI = 1;
rrom = 1;
}
}
else if(TI) // Transmitted data interrupt
{
TI = 0;
}
}
