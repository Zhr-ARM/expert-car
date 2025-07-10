/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-15     zhr       the first version
 */
#ifndef APPLICATIONS_LCD_H_
#define APPLICATIONS_LCD_H_

#define SPI_SCK GET_PIN(A,5)
#define SPI_SDA GET_PIN(A,7)
#define SPI_RST GET_PIN(B,5)
#define SPI_DC  GET_PIN(A,6)
#define SPI_CS  GET_PIN(A,4)
#define SPI_BLK GET_PIN(B,2)

#define SPI_SCK_0 rt_pin_write(SPI_SCK, PIN_LOW)
#define SPI_SCK_1 rt_pin_write(SPI_SCK, PIN_HIGH)
#define SPI_SDA_0 rt_pin_write(SPI_SDA, PIN_LOW)
#define SPI_SDA_1 rt_pin_write(SPI_SDA, PIN_HIGH)
#define SPI_RST_0 rt_pin_write(SPI_RST, PIN_LOW)
#define SPI_RST_1 rt_pin_write(SPI_RST, PIN_HIGH)
#define SPI_DC_0  rt_pin_write(SPI_DC,  PIN_LOW)
#define SPI_DC_1  rt_pin_write(SPI_DC,  PIN_HIGH)
#define SPI_CS_0  rt_pin_write(SPI_CS,  PIN_LOW)
#define SPI_CS_1  rt_pin_write(SPI_CS,  PIN_HIGH)
#define BL_0    rt_pin_write(SPI_BLK,  PIN_LOW)
#define BL_1    rt_pin_write(SPI_BLK, PIN_HIGH)

#define TFT_COLUMN_NUMBER 170
#define TFT_LINE_NUMBER 320
#define TFT_COLUMN_OFFSET 35
#define TFT_LINE_OFFSET 0

//定义常用颜色
#define RED     0xf800
#define GREEN   0x07e0
#define BLUE    0x001f
#define WHITE   0xffff
#define BLACK   0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D
#define GRAY1   0x8410
#define GRAY2   0x4208
void TFT_clear(void);
void TFT_full(unsigned int color);
void TFT_init(void) ;
void display_char16_16(unsigned int x,unsigned int y,unsigned long color,unsigned char word_serial_number);
void Picture_display(const unsigned char *ptr_pic);
void display_ascii_16_16(unsigned int x,unsigned int y,unsigned long color,unsigned char word_serial_number);
void display_string(unsigned int x,unsigned int y,unsigned long color,uint8_t *data);
void Gui_Circle(uint16_t X,uint16_t Y,uint16_t R,uint16_t fc);
void Gui_DrawLine(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t Color);
void Gui_box(uint16_t x, uint16_t y, uint16_t w, uint16_t h,uint16_t bc);
void Gui_box2(uint16_t x,uint16_t y,uint16_t w,uint16_t h, uint8_t mode);
void DisplayButtonDown(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void DisplayButtonUp(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void Gui_Drawmap(unsigned int x,unsigned int y,unsigned long color);

#endif /* APPLICATIONS_LCD_H_ */
