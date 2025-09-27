#include "iq32_board.h"
int Number=0;
void Show_MUX_All(void)
{ 
  ssd1306_Fill(Black);  
  uint16_t sensorValues[16]={};

        // อ่านค่าช่อง MUX 0–16
        for (uint8_t ch = 0; ch < 16; ch++) {
             MUX_SelectChannel(ch);
            HAL_Delay(2); // ให้สัญญาณนิ่ง
            sensorValues[ch] = MUX_Read();
            // MUX_Read();
        }
    if (Read_SW2()) { 
        Number+=1;
        if(Number>15)Number=15;
    }
    if (Read_SW1()) { 
        Number-=1;
        if(Number==0)Number=0;
    }
     
  
    OLED_ShowSInt("Sensor ->", Number, 0 , 2);
    OLED_ShowInt(sensorValues[Number],70,2);

    HAL_Delay(20);
    ssd1306_UpdateScreen();
}

int main(void)
{
    IQ32_Init();
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();

    Wait_SW1(); // รอ SW1 กดครั้งแรก

    while (1) {
        // Show_MUX_All();
        uint16_t sensorValues[16]={};

        // อ่านค่าช่อง MUX 0–16
        for (uint8_t ch = 0; ch < 16; ch++) {
             MUX_SelectChannel(ch);
            HAL_Delay(2); // ให้สัญญาณนิ่ง
            sensorValues[ch] = MUX_Read();
            // MUX_Read();9-
        }
        OLED_ShowMuxGraph(sensorValues,1750);
        HAL_Delay(5);
    }
}
