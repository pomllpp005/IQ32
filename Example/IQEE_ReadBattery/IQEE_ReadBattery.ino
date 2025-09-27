#include "iq32_board.h"

int main(void)
{
    IQ32_Init();
   
ssd1306_Fill(Black);
ssd1306_UpdateScreen();

  Wait_SW1();
  
 while(1){


   
        ssd1306_Fill(Black);
        OLED_ShowFloat(Battery_GetVoltage_Avg(),2,15,2); 
        HAL_Delay(10);
        ssd1306_UpdateScreen();

 

 }

    



} 
