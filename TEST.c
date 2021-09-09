#include "stm32l1xx.h"
#include "stm32l1xx_ll_system.h"
#include "stm32l1xx_ll_bus.h"
#include "stm32l1xx_ll_utils.h"
#include "stm32l1xx_ll_pwr.h"
#include "stm32l1xx_ll_gpio.h"
#include "stm32l1xx_ll_rcc.h"

#include "stm32l1xx_ll_lcd.h"
#include "stm32l152_glass_lcd.h"
#include "stdio.h"

void SystemClock_Config();
char show_lcd[7];  //Declare LCE
int state_lcd;     //state LCE on/off

int main()
{
	uint8_t usr_button;
	LL_GPIO_InitTypeDef GPIO_InitStruct; //Declare struct for GPIO config
	SystemClock_Config();
	
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA); //Enable GPIOA clock
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB); //Enable GPIOB clock
	
	//////////////INPUT/////////////////////
	
	GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	
  GPIO_InitStruct.Pin = LL_GPIO_PIN_0;	
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	////////////OUTPUT////////////////
	
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	
	GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);//write configuration to GPIOB registor
	GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);//configuration to GPIOB for pin 7
	

	///////////***LCD***/////////////
	SystemClock_Config(); //Max-performance configure
  LCD_GLASS_Init();	//LCD low-level init
		
	
	while(1)
	{
		state_lcd  = LL_GPIO_IsOutputPinSet(GPIOB, LL_GPIO_PIN_6);
		usr_button = LL_GPIO_IsInputPinSet (GPIOA,LL_GPIO_PIN_0);
		
		if(usr_button == 1)
		 {
				LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_6);
		    LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_7);
			 
			  if(state_lcd == 0)
				{
					sprintf(show_lcd, " Start");
		            LCD_GLASS_DisplayString((uint8_t*)show_lcd);
					      LCD_GLASS_BlinkConfig(LCD_BLINKMODE_ALLSEG_ALLCOM, LCD_BLINKFREQUENCY_DIV512);
								LL_mDelay(1800);
						    LCD_GLASS_BlinkConfig(LCD_BLINKMODE_OFF, LCD_BLINKFREQUENCY_DIV512);
					
					sprintf(show_lcd, "  PLAYER 2");
		            LCD_GLASS_ScrollSentence((uint8_t*) show_lcd , 2 , 250); 
					 
				}
				else
				{
					sprintf(show_lcd, "LD*off");
		            LCD_GLASS_DisplayString((uint8_t*) show_lcd );
				}
				while(usr_button != 0)	//if button is hold so loop 	
	               usr_button = LL_GPIO_IsInputPinSet(GPIOA,LL_GPIO_PIN_0);//loop here		
							   LCD_GLASS_DisplayBar(LCD_BAR_0);
		 }		
	}	       
}

void SystemClock_Config(void)
{
  /* Enable ACC64 access and set FLASH latency */ 
  LL_FLASH_Enable64bitAccess();; 
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

  /* Set Voltage scale1 as MCU will run at 32MHz */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  
  /* Poll VOSF bit of in PWR_CSR. Wait until it is reset to 0 */
  while (LL_PWR_IsActiveFlag_VOSF() != 0)
  {
  };
  
  /* Enable HSI if not already activated*/
  if (LL_RCC_HSI_IsReady() == 0)
  {
    /* HSI configuration and activation */
    LL_RCC_HSI_Enable();
    while(LL_RCC_HSI_IsReady() != 1)
    {
    };
  }
  
	
  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLL_MUL_6, LL_RCC_PLL_DIV_3);

  LL_RCC_PLL_Enable();
  while(LL_RCC_PLL_IsReady() != 1)
  {
  };
  
  /* Sysclk activation on the main PLL */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  };
  
  /* Set APB1 & APB2 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  /* Set systick to 1ms in using frequency set to 32MHz                             */
  /* This frequency can be calculated through LL RCC macro                          */
  /* ex: __LL_RCC_CALC_PLLCLK_FREQ (HSI_VALUE, LL_RCC_PLL_MUL_6, LL_RCC_PLL_DIV_3); */
  LL_Init1msTick(32000000);
  
  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(32000000);
}
