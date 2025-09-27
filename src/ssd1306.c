#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <stdio.h>   
#include <math.h>
#include <stdlib.h>
#include <limits.h>  
#include <string.h>  // For memcpy

#if defined(SSD1306_USE_I2C)
void ssd1306_Reset(void) {
    /* สำหรับ I2C ไม่ต้องทำอะไร */
}

void ssd1306_WriteCommand(uint8_t byte) {
    HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x00, 1, &byte, 1, HAL_MAX_DELAY);
}

void ssd1306_WriteData(uint8_t* buffer, size_t buff_size) {
    HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x40, 1, buffer, buff_size, HAL_MAX_DELAY);
}

#elif defined(SSD1306_USE_SPI)
void ssd1306_Reset(void) {
    HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SSD1306_Reset_Port, SSD1306_Reset_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(SSD1306_Reset_Port, SSD1306_Reset_Pin, GPIO_PIN_SET);
    HAL_Delay(10);
}

void ssd1306_WriteCommand(uint8_t byte) {
    HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SSD1306_DC_Port, SSD1306_DC_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&SSD1306_SPI_PORT, &byte, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_SET);
}

void ssd1306_WriteData(uint8_t* buffer, size_t buff_size) {
    HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SSD1306_DC_Port, SSD1306_DC_Pin, GPIO_PIN_SET);
    HAL_SPI_Transmit(&SSD1306_SPI_PORT, buffer, buff_size, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_SET);
}
#else
#error "คุณต้องกำหนด SSD1306_USE_SPI หรือ SSD1306_USE_I2C"
#endif

// ----------------- Screen buffer -----------------
static uint8_t SSD1306_Buffer[SSD1306_BUFFER_SIZE];
static SSD1306_t SSD1306;


// ----------------- Buffer Functions -----------------
SSD1306_Error_t ssd1306_FillBuffer(uint8_t* buf, uint32_t len) {
    if(len <= SSD1306_BUFFER_SIZE) {
        memcpy(SSD1306_Buffer, buf, len);
        return SSD1306_OK;
    }
    return SSD1306_ERR;
}

void ssd1306_Fill(SSD1306_COLOR color) {
    memset(SSD1306_Buffer, (color == Black) ? 0x00 : 0xFF, sizeof(SSD1306_Buffer));
}

// ----------------- Screen Update -----------------
void ssd1306_UpdateScreen(void) {
    for(uint8_t i = 0; i < SSD1306_HEIGHT/8; i++) {
        ssd1306_WriteCommand(0xB0 + i);
        ssd1306_WriteCommand(0x00 + SSD1306_X_OFFSET_LOWER);
        ssd1306_WriteCommand(0x10 + SSD1306_X_OFFSET_UPPER);
        ssd1306_WriteData(&SSD1306_Buffer[SSD1306_WIDTH*i], SSD1306_WIDTH);
    }
}

// ----------------- Pixel -----------------
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color) {
    if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) return;
    if(color == White)
        SSD1306_Buffer[x + (y/8)*SSD1306_WIDTH] |= 1 << (y%8);
    else
        SSD1306_Buffer[x + (y/8)*SSD1306_WIDTH] &= ~(1 << (y%8));
}
// ----------------- Orientation Enum -----------------
// typedef enum {
//     SSD1306_NORMAL = 0,
//     SSD1306_MIRROR_H = 1,
//     SSD1306_MIRROR_V = 2,
//     SSD1306_MIRROR_HV = 3
// } SSD1306_Orientation_t;

// ----------------- Orientation / Screen Rotation -----------------
void ssd1306_RotateScreen(SSD1306_Orientation_t orientation) {
    switch(orientation) {
        case SSD1306_NORMAL:
            ssd1306_WriteCommand(0xC8); // COM scan normal
            ssd1306_WriteCommand(0xA1); // Segment remap normal
            break;
        case SSD1306_MIRROR_H:
            ssd1306_WriteCommand(0xC8);
            ssd1306_WriteCommand(0xA0);
            break;
        case SSD1306_MIRROR_V:
            ssd1306_WriteCommand(0xC0);
            ssd1306_WriteCommand(0xA1);
            break;
        case SSD1306_MIRROR_HV:
            ssd1306_WriteCommand(0xC0);
            ssd1306_WriteCommand(0xA0);
            break;
    }
}

// ----------------- Init -----------------
void ssd1306_Init(void) {
    ssd1306_Reset();
    HAL_Delay(100);
    ssd1306_SetDisplayOn(0);

    ssd1306_WriteCommand(0x20); // Memory addressing mode
    ssd1306_WriteCommand(0x00); // Horizontal

    ssd1306_WriteCommand(0xB0);
    ssd1306_RotateScreen(SSD1306_MIRROR_HV); // default orientation

    ssd1306_WriteCommand(0x00);
    ssd1306_WriteCommand(0x10);
    ssd1306_WriteCommand(0x40);
    ssd1306_SetContrast(0xFF);

    ssd1306_WriteCommand(0xA6); // Normal color
    ssd1306_WriteCommand(0xA8);  
    ssd1306_WriteCommand(0x1F); // 128x32 multiplex
    ssd1306_WriteCommand(0xDA);  
    ssd1306_WriteCommand(0x02); // COM pins
    ssd1306_WriteCommand(0xDB);
    ssd1306_WriteCommand(0x20);
    ssd1306_WriteCommand(0x8D);
    ssd1306_WriteCommand(0x14);
    ssd1306_SetDisplayOn(1);

    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();

    SSD1306.CurrentX = 0;
    SSD1306.CurrentY = 0;
    SSD1306.Initialized = 1;
}

// ----------------- Display -----------------
void ssd1306_SetDisplayOn(uint8_t on) {
    if(on) {
        ssd1306_WriteCommand(0xAF);
        SSD1306.DisplayOn = 1;
    } else {
        ssd1306_WriteCommand(0xAE);
        SSD1306.DisplayOn = 0;
    }
}

uint8_t ssd1306_GetDisplayOn() {
    return SSD1306.DisplayOn;
}

void ssd1306_SetContrast(uint8_t value) {
    ssd1306_WriteCommand(0x81);
    ssd1306_WriteCommand(value);
}

// ----------------- Text -----------------
void ssd1306_SetCursor(uint8_t x, uint8_t y) {
    SSD1306.CurrentX = x;
    SSD1306.CurrentY = y;
}

char ssd1306_WriteChar(char ch, SSD1306_Font_t Font, SSD1306_COLOR color) {
    if(ch < 32 || ch > 126) return 0;

    uint8_t char_width = Font.char_width ? Font.char_width[ch-32] : Font.width;
    if(SSD1306_WIDTH < (SSD1306.CurrentX + char_width) || 
       SSD1306_HEIGHT < (SSD1306.CurrentY + Font.height))
        return 0;

    for(uint32_t i = 0; i < Font.height; i++) {
        uint32_t b = Font.data[(ch-32)*Font.height + i];
        for(uint32_t j = 0; j < char_width; j++) {
            if((b << j) & 0x8000)
                ssd1306_DrawPixel(SSD1306.CurrentX + j, SSD1306.CurrentY + i, color);
            else
                ssd1306_DrawPixel(SSD1306.CurrentX + j, SSD1306.CurrentY + i, !color);
        }
    }

    SSD1306.CurrentX += char_width;
    return ch;
}

char ssd1306_WriteString(char* str, SSD1306_Font_t Font, SSD1306_COLOR color) {
    while(*str) {
        if(ssd1306_WriteChar(*str, Font, color) != *str) return *str;
        str++;
    }
    return *str;
}
void ssd1306_SetCursorLine(uint8_t col, uint8_t line, SSD1306_Font_t Font) {
    SSD1306.CurrentX = col;
    SSD1306.CurrentY = line * Font.height;
}
void OLED_ShowInt(int value, uint8_t x, uint8_t y) {
    char buf[16];
    sprintf(buf, "%d", value);
    ssd1306_SetCursorLine(x, y, Font_6x8);
    ssd1306_WriteString(buf, Font_6x8, White);
    
}
void OLED_ShowSInt(const char* str, int value, uint8_t x, uint8_t y)
{
    char buf[32];  // buffer สำหรับข้อความ + int
    if(str == NULL) str = "";

    // สร้าง string รวมข้อความ + ตัวเลข
    sprintf(buf, "%s%d", str, value);

    // ตั้ง cursor
    ssd1306_SetCursorLine(x, y, Font_6x8);

    // เขียน string บน OLED
    ssd1306_WriteString(buf, Font_6x8, White);
}

void OLED_ShowFloat(float value, uint8_t decimalPlaces, uint8_t x, uint8_t y) {
    char buf[24];
    float factor = powf(10, decimalPlaces);
    
    // ปัดค่าก่อนแยกส่วนจำนวนเต็มและทศนิยม
    long total = (long)roundf(value * factor);
    long intPart = total / (long)factor;
    long fracPart = labs(total % (long)factor); // ใช้ labs เผื่อค่าลบ
    
    // สร้าง string แสดงผล
    sprintf(buf, "%ld.%0*ld", intPart, decimalPlaces, fracPart);
    
    // แสดงบนจอ
    ssd1306_SetCursorLine(x, y, Font_6x8);
    ssd1306_WriteString(buf, Font_6x8, White);
}


void ssd1306_FillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color) {
    for (uint8_t x = x1; x <= x2; x++) {
        for (uint8_t y = y1; y <= y2; y++) {
            ssd1306_DrawPixel(x, y, color);
        }
    }
}
void OLED_ShowMuxGraph(uint16_t* muxValues, uint16_t threshold)
{
    // เคลียร์หน้าจอ
    ssd1306_Fill(Black);
    
    // นับจำนวน active sensors
    uint8_t activeCount = 0;
    for(int i = 0; i < 16; i++) {
        if(muxValues[i] >= threshold) {
            activeCount++;
        }
    }
    
    // แสดงข้อมูลบนบรรทัดแรก
    char info_str[32];
    sprintf(info_str, "MUX Thr:%d Act:%d", threshold, activeCount);
    ssd1306_SetCursorLine(0, 0, Font_6x8);
    ssd1306_WriteString(info_str, Font_6x8, White);
    
    // คำนวณตำแหน่งของแต่ละแท่ง
    uint8_t totalBarsWidth = (BAR_WIDTH * 16) + (BAR_SPACING * 15);
    uint8_t startX = (SSD1306_WIDTH - totalBarsWidth) / 2; // จัดกลาง
    uint8_t graphY = 12; // เริ่มกราฟจากบรรทัดที่ 2
    
    // วาดแท่งกราฟ
    for(int i = 0; i < 16; i++) {
        uint8_t barX = startX + (i * (BAR_WIDTH + BAR_SPACING));
        
        // ตรวจสอบว่าค่าเกิน threshold หรือไม่
        if(muxValues[i] >= threshold) {
            // วาดแท่งที่เต็ม (สี White)
            ssd1306_FillRectangle(barX, graphY, 
                                barX + BAR_WIDTH - 1, 
                                graphY + GRAPH_HEIGHT - 1, 
                                White);
        }
        //  else {
        //     // วาดเฉพาะกรอบแท่ง (แท่งว่าง)
        //     // วาดขอบบน
        //     for(int x = 0; x < BAR_WIDTH; x++) {
        //         ssd1306_DrawPixel(barX + x, graphY, White);
        //     }
        //     // วาดขอบล่าง  
        //     for(int x = 0; x < BAR_WIDTH; x++) {
        //         ssd1306_DrawPixel(barX + x, graphY + GRAPH_HEIGHT - 1, White);
        //     }
        //     // วาดขอบซ้าย
        //     for(int y = 0; y < GRAPH_HEIGHT; y++) {
        //         ssd1306_DrawPixel(barX, graphY + y, White);
        //     }
        //     // วาดขอบขวา
        //     for(int y = 0; y < GRAPH_HEIGHT; y++) {
        //         ssd1306_DrawPixel(barX + BAR_WIDTH - 1, graphY + y, White);
        //     }
        // }
        
        // แสดงหมายเลขช่อง (0-F) ใต้แท่ง
        if(i < 10) {
            // หมายเลข 0-9
            char num_str[2];
            sprintf(num_str, "%d", i);
            ssd1306_SetCursor(barX + 1, graphY + GRAPH_HEIGHT + 2);
            ssd1306_WriteString(num_str, Font_6x8, White);
        } else {
            // หมายเลข A-F (10-15)
            char hex_str[2];
            sprintf(hex_str, "%c", 'A' + (i - 10));
            ssd1306_SetCursor(barX + 1, graphY + GRAPH_HEIGHT + 2);
            ssd1306_WriteString(hex_str, Font_6x8, White);
        }
    }
    
    // อัพเดทหน้าจอ
    ssd1306_UpdateScreen();
}