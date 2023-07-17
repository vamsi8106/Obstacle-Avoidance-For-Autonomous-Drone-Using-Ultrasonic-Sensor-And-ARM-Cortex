//K VAMSI(M22RM002), SOHAM PADHYE(M22RM007)
#include "stm32f412Zx.h"
void delay(int dd);  // Functions declaration
void Timer_initialize();
void TIM2_us_Delay(uint32_t delay);                                                                                             

uint32_t static data;      //variable declaration
double static time,dist;   //variable for collecting the distance, time etc.

int main(){

		 int speed;
		 RCC->AHB1ENR |= 0x04;				//Enable GPIOC clock
		 RCC->AHB1ENR |= 0x2;					//Enable GPIOB clock	
		 RCC->APB1ENR |=0x1; 				 // This line enables Timer2
		 
		 Timer_initialize();

		 GPIOB->MODER = 0x4000;		   //PB7 pin as Trigger Pin as Trigger pin
		 GPIOC->MODER = 0x0000;      //PC6 is set to input mode to capture data from Echo pin
		 GPIOB->MODER |=0x10000000;	 //Setting PB14 as output for RED led.
		 GPIOB->MODER|= (1<<10);     //Select PB5 as Output for controlling BLDC Motor
		 GPIOE->MODER |= 0x100;
		
		 TIM2->PSC=0; 							 // Prescaler of Timer2 is set to zero
		 GPIOB->ODR = 0x00;			     // Clearing ODR register

		 int y=0;
	
		 while(1) // This loop initializes BLDC Motor
			{
				if(GPIOC->IDR & 0X2000) //Senses Input from the USER Push BUtton connnected to PC13
				{     
						while(y<250)
						{
							GPIOB->ODR^=(1<<5); //Sending PWM Signal to PB5
							delay(999);     // Delays 1000 micro seconds 
							GPIOB->ODR^=(1<<5);
							delay(18999);    // Delays 19000 micro seconds 
							y=y+1;
						}
						break;
				}
			}        
			while(1)
				{
						if(GPIOC->IDR & 0X2000) //Senses Input from the USER Push BUtton
						{ 
							 while(1)
								{
										data=0;
										TIM2_us_Delay(10);	// Sends 10micro seconds pulse to Trigger pin
								  	GPIOB->ODR =0x80;		// Making PB7 pin as high
									  TIM2_us_Delay(10);
										GPIOB->ODR =0x00;            
										TIM2_us_Delay(10);
										while(GPIOC->IDR & 0x40) // Reading on time of input pulse from PC6(ECHO Pin)
											{
												data=data+1;
											}
										
										if (data > 0)  
											{
												 time = data*(0.0625*0.000001);	// Coverting data in to time
												 dist = ((time*340)/2)*1000;		// Converting time in to distance
												 if (dist <100)  // If distance less than 10cm speed increases
													 {
												      GPIOB->ODR =0x4000;
											        GPIOB->ODR^=(1<<5); //Turns Connected to PB5
											        delay(2000); // Delays 2000 micro seconds
											        GPIOB->ODR^=(1<<5);
											        delay(20000-2000);
													    dist=0;
												   }
												 else if (data>=100)// If distance greater than 10cm speed decreases
												   {
															GPIOB->ODR^=(1<<5); //Turns Connected to PB5
															delay(1100); // Delays 1100 micro seconds
															GPIOB->ODR^=(1<<5);
															delay(20000-1100);
															dist=0;
													}
											
											}
											
							}
										
					 }
				}
								  
}  

void TIM2_us_Delay(uint32_t delay)
	{                             
		uint32_t val;
		val = (delay*16)-1; // calculating ARR val to produce delay
		TIM2->ARR =val;
		TIM2->SR=0;// Initializing status register to zero.
		TIM2->CR1 |=1; // starting the timer 4.
		while((TIM2->SR & 0x01)==0);   //Reset the update interrupt flag
	}

void Timer_initialize(void)
	{
		RCC->APB1ENR |=100; // This line enables Timer4.
		TIM4->PSC=15; //Prescaler to set clock frequency at 1Mhz
		TIM4->CR1 |=(1<<0); //Enable the Timer
	}

void delay(int dd)
{
	TIM4->ARR = dd;
	TIM4->SR=0;// Initializing status register to zero.
	TIM4->CR1 |=1; // starting the timer 4.
	while((TIM4->SR & 0x01)==0); //Wait untill ARR value becomes zero.UIF flag is updated when underflow or overflow occurs
}
