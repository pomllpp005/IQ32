#include <iq32_board.h>

void test_mpu6500_step_by_step()
{
    ssd1306_Fill(Black);
    ssd1306_SetCursorLine(0, 0, Font_6x8);
    ssd1306_WriteString("Testing MPU6500", Font_6x8, White);
    ssd1306_UpdateScreen();
    HAL_Delay(2000);

    // Step 1: ตรวจสอบ SPI
    ssd1306_Fill(Black);
    ssd1306_SetCursorLine(0, 0, Font_6x8);
    ssd1306_WriteString("Step 1: SPI Test", Font_6x8, White);
    ssd1306_UpdateScreen();
    HAL_Delay(1000);

    // Step 2: Init MPU6500
    MPU6500_Init();
    
    // Step 3: ตรวจสอบ WHO_AM_I
    debug_mpu6500();

    // Step 4: อ่านข้อมูลทีละส่วน
    while(1) {
        static uint8_t test_mode = 0;
        
        if(Read_SW1()) {
            test_mode = (test_mode + 1) % 3;
            HAL_Delay(300);
        }

        switch(test_mode) {
            case 0: {
                // แสดง Raw Data
                uint8_t raw_data[14];
                if(MPU6500_ReadRegs(0x3B, raw_data, 14)) {
                    ssd1306_Fill(Black);
                    ssd1306_SetCursorLine(0, 0, Font_6x8);
                    ssd1306_WriteString("Raw Data Mode", Font_6x8, White);
                    
                    OLED_ShowSInt("AX_H:", raw_data[0], 0, 1);
                    OLED_ShowSInt("AX_L:", raw_data[1], 8, 1);
                    OLED_ShowSInt("AY_H:", raw_data[2], 0, 2);
                    OLED_ShowSInt("AY_L:", raw_data[3], 8, 2);
                    
                    // คำนวณ AX
                    int16_t ax_raw = (raw_data[0] << 8) | raw_data[1];
                    OLED_ShowSInt("AX:", ax_raw, 0, 3);
                } else {
                    ssd1306_Fill(Black);
                    ssd1306_SetCursorLine(0, 0, Font_6x8);
                    ssd1306_WriteString("Read Error!", Font_6x8, White);
                }
                break;
            }
            case 1: {
                // แสดงข้อมูลที่แปลงแล้ว
                float ax, ay, az, gx, gy, gz;
                if(MPU6500_ReadAccelGyro(&ax, &ay, &az, &gx, &gy, &gz)) {
                    ssd1306_Fill(Black);
                    ssd1306_SetCursorLine(0, 0, Font_6x8);
                    ssd1306_WriteString("Converted Data", Font_6x8, White);
                    
                    OLED_ShowFloat(ax, 2, 0, 1);
                    OLED_ShowFloat(ay, 2, 0, 2);
                    OLED_ShowFloat(az, 2, 0, 3);
                } else {
                    ssd1306_Fill(Black);
                    ssd1306_SetCursorLine(0, 0, Font_6x8);
                    ssd1306_WriteString("Convert Error!", Font_6x8, White);
                }
                break;
            }
            case 2: {
                // แสดง Gyro
                float ax, ay, az, gx, gy, gz;
                if(MPU6500_ReadAccelGyro(&ax, &ay, &az, &gx, &gy, &gz)) {
                    ssd1306_Fill(Black);
                    ssd1306_SetCursorLine(0, 0, Font_6x8);
                    ssd1306_WriteString("Gyro Data", Font_6x8, White);
                    
                    OLED_ShowFloat(gx, 1, 0, 1);
                    OLED_ShowFloat(gy, 1, 0, 2);
                    OLED_ShowFloat(gz, 1, 0, 3);
                }
                break;
            }
        }
        
        ssd1306_UpdateScreen();
        

        if(Read_SW2()) break;
    }
}

// ===== Main Function ที่แก้ไข =====
int main(void) {
    IQ32_Init();
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();

    // แสดงข้อความต้อนรับ
    ssd1306_Fill(Black);
    ssd1306_SetCursorLine(0, 0, Font_6x8);
    ssd1306_WriteString("IQ32 MPU6500", Font_6x8, White);
    ssd1306_SetCursorLine(0, 1, Font_6x8);
    ssd1306_WriteString("Press SW1 to", Font_6x8, White);
    ssd1306_SetCursorLine(0, 2, Font_6x8);
    ssd1306_WriteString("start test", Font_6x8, White);
    ssd1306_UpdateScreen();

    Wait_SW1(); // รอ SW1 กดครั้งแรก
    
    // เรียกใช้ test function
    test_mpu6500_step_by_step();
    
    return 0;
}