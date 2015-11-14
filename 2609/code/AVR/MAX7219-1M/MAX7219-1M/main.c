#include <avr/io.h>
#include  <avr\pgmspace.h>
#define uchar unsigned char
#define uint  unsigned int
//define Max7219 port
#define NOP()   asm("nop")

#define BIT0 1
#define BIT1 2
#define BIT2 4

#define LED_N 4                   //define devies ,four devies in here




#define Max7219_pinCLK0  PORTB   &= ~BIT0  // ~(1 << PB0)
#define Max7219_pinCLK1  PORTB   |= BIT0   // (1 << PB0)

#define Max7219_pinCS0   PORTB   &= ~BIT1  //~(1 << PB1)
#define Max7219_pinCS1   PORTB   |= BIT1 // (1 << PB1)

#define Max7219_pinDIN0  PORTB   &=  ~BIT2//~(1 << PB2)
#define Max7219_pinDIN1  PORTB   |=  BIT2//(1 << PB2)









uchar font[1][32] ={                                                                    //Font mode

      {0x04,0x10,0x0E,0x10,0xF8,0x90,0x08,0x50,0x08,0x10,0xFE,0x90,0x08,0x50,0x1C,0x14,
      0x1A,0x1E,0x29,0xF0,0x28,0x10,0x48,0x10,0x88,0x10,0x08,0x10,0x08,0x10,0x08,0x10},


};




uchar font_loop[4][8];    //Display Buffer;





uchar  Change_bit( uchar num)
{
   

   uchar i;
   uchar temp = 0;
   for( i = 0; i < 8 ; ++i)
   {
    temp <<= 1;
    if(num&0x01) temp |= 0x01;
	num >>= 1;
   }
   
return temp;



}


//change font and put in font[] 
void font_charge( uchar *num )
{
   uchar i;
   uchar *temp;

   temp = num;
   //change one
   for(i = 0; i < 8 ; ++i)
   {
     font_loop[0][i] =  *temp;
	 temp += 2;   //jump next
   }


   //change two
   temp = num;
   temp += 1;
   for(i = 0; i < 8 ; ++i)
   {
     font_loop[1][i] =  *temp;
	  temp += 2;   //jump next
   }

                                          //change  third

   temp = num;
   temp += 31;
  
   for(i = 0; i < 8 ; ++i)
   {
     font_loop[2][i] =Change_bit(*temp) ;

	  temp -= 2 ;                       //jump next
   }

   temp = num;                         //change four
   temp += 30;
   for(i = 0; i < 8 ; ++i)
   {
     font_loop[3][i] =Change_bit(*temp) ;
	  temp -= 2;                       //jump next
   }
}




void Delay_xms(uint x)
{
 uint i,j;
 for(i=0;i<x;i++)
  for(j=0;j<112;j++);
}

//------------------------------------------
//function£ºwrite MAX7219(U3) a byte
//input parameter£ºDATA 
//output £ºnothing
void Write_Max7219_byte(uchar temp)         
{
    	uchar i;    
		
	    for(i=8;i >= 1;--i)
          {		  
           	Max7219_pinCLK0;        // give a rising edge
           if (temp&0x80) Max7219_pinDIN1;
           else  Max7219_pinDIN0;
            temp <<= 1;

		
           // NOP();
            Max7219_pinCLK1;
			//NOP();

           }                                 
}






//-------------------------------------------
//fubnction£º put the data to MAX7219
//input£ºaddress¡¢dat
//output£ºnoting
void Write_Max7219(uchar address,uchar dat, uchar sel)
{ 


   
     uchar good;
     Max7219_pinCS0;
	 


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
	 Max7219_pinCS1;  
	 
	 
	 
	 
	                       
}







void Initial_comm( uchar address,uchar dat)
{
  uchar i,good;
   Max7219_pinCS0;


   good = LED_N;
   for( ; good > 0; --good )                  //Determine which one equipment operation
   {
	 Write_Max7219_byte(0x00);               //Write the address, that is, digital tube number
     Write_Max7219_byte(0x00);               //Write data, that is, digital tube display digital	  
   }


   for( i = 0; i < LED_N ; ++i)
   {
    Write_Max7219_byte(address);
    Write_Max7219_byte(dat);
   }
   Max7219_pinCS1;
}


void Init_MAX7219(void)


   {
      
       DDRB |= ( BIT0+BIT1+BIT2);
       Max7219_pinCLK0;
       Max7219_pinCS0;


      char num = 0;
	  Initial_comm( 0x09,0x00); //Decoding: BCD decoding
      Initial_comm( 0x0a, 0x03);//light intensity


	  Initial_comm( 0x0b, 0x07);//Scan line; Eight digital tube display
	  Initial_comm( 0x0c, 0x01);//Power down mode: 0, normal mode: 1
	  Initial_comm( 0x0f, 0x00);//Display test: 1; End of the test, normal display: 0

  }





void write_word( void)
{

  uchar i ,j;
    for(j = 0; j <= 3 ; ++j)
      {
      for(i = 1;i <= 8;++i)
      Write_Max7219(i,font_loop[j][i-1],j);
    
      }

}



void Mov_bity( uchar *num)
{
    uchar temp1,temp2;
	temp1 = num[0];
	temp2 = num[1];

    num[0] <<= 1;
	num[1] <<= 1;
	if(temp1&0x80) num[1] |= 0x01;
	if(temp2&0x80) num[0] |= 0x01;
}






void LED_mov()
{
 uchar i;
 uchar *temp = font;

 for( i =0 ;i < 16 ;++i)
 {
  Mov_bity(temp);
  temp += 2;
 }

  




}




 int main()
 {

 Delay_xms(50);
  Init_MAX7219(); 
 while (1)
 {

     
     font_charge(font[0]);
     write_word();

	LED_mov();
	Delay_xms(300);
     
}
     loop: goto loop;




 }










