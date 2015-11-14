//Development environment :IAR STM8 1.40.1 
//Microcontroller:STM8S003F3
//Funtion:display "R & D" in circularly.We use 2x6pcs 8x8 dot matrix and 32 bytes define a character,so it only display 3 characters each time.

#include<iostm8s003f3.h>

//PD4 Pin,connect to CLK of MAX7219.
#define CLK0 PD_ODR &= 0xef //PD4=0   
#define CLK1 PD_ODR |= 0x10 //PD4=1
//PD5 Pin,connect to CS of MAX7219. 
#define CS0  PD_ODR &= 0xdf //PD5=0
#define CS1  PD_ODR |= 0x20 //PD5=1
//PD6 Pin,connect to DIN of MAX7219.
#define DIN0 PD_ODR &= 0xbf //PD6=0
#define DIN1 PD_ODR |= 0x40 //PD6=1

#define LED_N 12                   //define the quanlity of 8x8 dot matrix MAX7219 module.It is 12 in here. 

void delay(unsigned int x) //ms
{
    unsigned int i,j;
    for(i=x;i>0;i--)
        for(j=250;j>0;j--);
}
void init(void)
{
    PD_DDR=0x70;
    PD_CR1=0x70;
    PD_CR2=0;
  
}
//Original character.
//We define 3 characters in here.There are consist of  16x16 bits (32 bytes) for each character.
//So,you can imagine cut an character to 4 parts: top-right-0,top-left-1,bottom-right-2,bottom-left-3.
//The character is "R & D".You can change character.

unsigned int   font[3][32] ={                                                                    
{0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xF8,0x18,0x1C,0x18,0x1C,0x18,0x1C,0x18,0x78,//R
0x1F,0xE0,0x18,0xE0,0x18,0x70,0x18,0x30,0x18,0x18,0x7E,0x1F,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x80,0x18,0xC0,0x18,0xC0,0x19,0x80,0x1F,0x3C,//&
0x3E,0x3C,0x66,0x10,0xE3,0xB0,0xE1,0xE0,0x70,0xF1,0x3F,0xBE,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xE0,0x38,0x38,0x38,0x1C,0x38,0x0E,0x38,0x0E,//D
0x38,0x0E,0x38,0x0E,0x38,0x0E,0x38,0x1C,0x38,0x78,0x7F,0xE0,0x00,0x00,0x00,0x00},
};

unsigned int  font_loop[12][8];    //Change font of original character in order to match dot matrix.
				   //It need 4 8x8 dot matrixs for each character, so we must ready	12 space to store data.
unsigned int  Change_bit( unsigned int num)//Change sequence of byte,make the bit0 to bit7 and so on.
{
	unsigned int i;
	unsigned int temp = 0;
	for( i = 0; i < 8 ; ++i)
	{
		temp <<= 1;
		if(num&0x01) temp |= 0x01;
		num >>= 1;
	}
	return temp;
}
void font_charge( unsigned int *num)//change font and make original character to font_loop[][].
{                            //num[]:the origrinal character need to change.
	unsigned int i,j;
	unsigned int *temp;
	for(j=0;j<3;j++)//it need 4 times to change each one characher.We must change all 3 character. 
	{
		temp = num + j*32; //keep address of top-right-0 for each character
		for(i=0;i<8;i++)
		{
			font_loop[j*2][i]= *temp;
			temp +=2;
		}

		temp = num + j*32;//keep address of top-left-1 for each character
		temp += 1;
		for(i = 0; i < 8 ; ++i)
		{
			font_loop[j*2+1][i] =  *temp;
			temp += 2;   
		}

		temp = num + j*32;//keep address of bottom-right-2 for each character
		temp += 30;
		for(i = 0; i < 8 ; ++i)
		{
			font_loop[11-j*2][i] =Change_bit(*temp) ;
			temp -= 2;                      
		}

		temp = num + j*32;//keep address of bottom-left-3 for each character
		temp += 31;
		for(i = 0; i < 8 ; ++i)
		{
			font_loop[11-j*2-1][i] =Change_bit(*temp) ;
			temp -= 2 ;                       
		}

	}
}

void Write_Max7219_byte(unsigned int temp) //write a byte to MAX7219        
{
	unsigned int i;    
	for(i=8;i >= 1;--i)
	{		  
		CLK0;       
		if (temp&0x80) DIN1;
		else  DIN0;
		temp <<= 1;
		CLK1;
	}                                 
}
void Write_Max7219(unsigned int address,unsigned int dat, unsigned int sel)//write data to MAX7219.
{
	unsigned int good;
	CS0;
	good = LED_N;
	for( ; good > 0; --good )                  //Determine which one equipment operation
	{
		Write_Max7219_byte(0x00);               //Write the address, that is, digital tube number
		Write_Max7219_byte(0x00);               //Write data, that is, digital tube display digital	  
	}
	Write_Max7219_byte(address);           //Write the address, that is, digital tube number
	Write_Max7219_byte(dat);               //Write data, that is, digital tube display digital
	good = sel ;
	for( ; good > 0; --good )                  //Determine which one equipment operation
	{
		Write_Max7219_byte(0x00);               //Write the address, that is, digital tube number
		Write_Max7219_byte(0x00);               //Write data, that is, digital tube display dig
	}
	CS1;  
}
void Initial_comm( unsigned int address,unsigned int dat)
{
	unsigned int i;
	CS0;
	//good = LED_N;
/*	for( ; good > 0; --good )                  //Determine which one equipment operation
	{
		Write_Max7219_byte(0x00);               //Write the address, that is, digital tube number
		Write_Max7219_byte(0x00);               //Write data, that is, digital tube display digital	  
	}*/

	for( i = 0; i < LED_N ; ++i)
	{
		Write_Max7219_byte(address);
		Write_Max7219_byte(dat);
	}
	CS1;
}
void Init_MAX7219(void)
{

	CLK0;
	CS0;
	Initial_comm( 0x09,0x00); //Decoding: BCD decoding
	Initial_comm( 0x0a, 0x0f);//light intensity
	Initial_comm( 0x0b, 0x07);//Scan line; Eight digital tube display
	Initial_comm( 0x0c, 0x01);//Power down mode: 0, normal mode: 1
	Initial_comm( 0x0f, 0x00);//Display test: 1; End of the test, normal display: 0

}
void write_word( void)//write 3 character to dot matrix.
{
	unsigned int i ,j;
	for(j = 0; j <= 11 ; ++j)
	{
		if(j<6)//write data to dot matrix in top.
		{
			for(i = 1;i <= 8;++i)
				Write_Max7219(i,font_loop[j][i-1],j);		
		}
		else//write data to matrix in bottom.
		{
			for(i = 1;i <= 8;++i)
				Write_Max7219(i,font_loop[6+j%6][i-1],j);			
		}

	}
}

int main()
{
        delay(100);
        init();
	 
	while(1)
	{
		Init_MAX7219();
                delay(50);
                font_charge(*font);//change character 
		write_word();
                delay(50);
                write_word();
                delay(50);
	}
}
