/*
 * Oled.h
 * Functions for writing to the OLED screen
 * Basically wraps drawing text and numbers to the OLED
 * Uses font.h -> 12 pixel wide fixed width
 */
#ifndef _OLED_SSD1306_H
#define _OLED_SSD1306_H

#define DEVICEADDR_OLED  0x3c

void Oled_DisplayOn(void);
void Oled_DisplayOff(void);
void OLED_Sync();
void Oled_DrawArea(uint8_t x0, uint8_t y0, uint8_t wide, uint8_t height, const uint8_t* ptr);
void Set_ShowPos(uint8_t x, uint8_t y);
void Oled_DisplayFlip();
void GPIO_Init_OLED(void);
void Init_Oled(uint8_t leftHanded);
const uint8_t* Data_Command(uint8_t len, const uint8_t* ptr);
void Clear_Screen(void); //Clear the screen
/*Functions for writing to the screen*/
void OLED_DrawString(const char* string, const uint8_t length);
void OLED_DrawChar(char c, uint8_t x);
void OLED_DrawExtraFontChars(uint8_t id, uint8_t x);
void OLED_DrawSymbolChar(uint8_t id, uint8_t x);
void OLED_DrawWideChar(uint8_t id, uint8_t x);
void OLED_DrawTwoNumber(uint8_t in, uint8_t x);
void OLED_DrawThreeNumber(uint16_t in, uint8_t x);
void OLED_DrawIDLELogo();
void OLED_DrawSymbol(uint8_t x, uint8_t symbol);

void OLED_SetOrientation(uint8_t ori);
uint8_t OLED_GetOrientation();
#endif
