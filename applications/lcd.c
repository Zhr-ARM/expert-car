/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-15     zhr       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "drv_common.h"

#define DBG_TAG "lcd.h"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include "lcd.h"
#include "stm32g4xx_hal.h"
#include "font.h"

#define TRUE             1
#define FALSE            0

#define delay_ms(n) rt_thread_mdelay(n)

/*************SPI配置函数*******************
SCL空闲时低电平，第一个上升沿采样
模拟SPI
******************************************/

/**************************SPI模块发送函数************************************************

 *************************************************************************/
void SPI_SendByte(unsigned char byte)
{

  unsigned char counter;

  for(counter=0;counter<8;counter++)
  {
    SPI_SCK_0;

    if((byte&0x80)==0)
    {
      SPI_SDA_0;
    }
    else SPI_SDA_1;
    byte=byte<<1;

    SPI_SCK_1;

    SPI_SCK_0;

  }
}

void TFT_SEND_CMD(unsigned char o_command)
  {
    SPI_DC_0;
    SPI_CS_0;
    SPI_SendByte(o_command);
    SPI_CS_1;

    //SPI_DC_1;
  }
void TFT_SEND_DATA(unsigned char o_data)
  {
    SPI_DC_1;
    SPI_CS_0;
    SPI_SendByte(o_data);
    SPI_CS_1;

   }

 void TFT_Write_HalfWord(uint16_t da)
 {
     SPI_DC_1;
    SPI_CS_0;
    SPI_SendByte(da>>8);
     SPI_SendByte(da);
    SPI_CS_1;

 }

 void TFT_SET_ADD(unsigned short int x_start,unsigned short int y_start,unsigned short int x_end,unsigned short int y_end)
{
    unsigned short int x = x_start + TFT_COLUMN_OFFSET,y=x_end+ TFT_COLUMN_OFFSET;
    TFT_SEND_CMD(0x2a);     //Column address set
    TFT_SEND_DATA(x>>8);    //start column
    TFT_SEND_DATA(x);
    TFT_SEND_DATA(y>>8);    //end column
    TFT_SEND_DATA(y);
    x = y_start + TFT_LINE_OFFSET;
    y=y_end+ TFT_LINE_OFFSET;
    TFT_SEND_CMD(0x2b);     //Row address set
    TFT_SEND_DATA(x>>8);    //start row
    TFT_SEND_DATA(x);
    TFT_SEND_DATA(y>>8);    //end row
    TFT_SEND_DATA(y);
    TFT_SEND_CMD(0x2C);     //Memory write

}
void TFT_clear(void)
  {
    unsigned int ROW,column;
    TFT_SET_ADD(0,0,TFT_COLUMN_NUMBER-1,TFT_LINE_NUMBER-1);
    for(ROW=0;ROW<TFT_LINE_NUMBER;ROW++)             //ROW loop
      {

          for(column=0;column<TFT_COLUMN_NUMBER;column++)  //column loop
            {

                TFT_SEND_DATA(0xFF);
                TFT_SEND_DATA(0xFF);
            }
      }
  }

void TFT_full(unsigned int color)
  {
    unsigned int ROW,column;
    TFT_SET_ADD(0,0,TFT_COLUMN_NUMBER-1,TFT_LINE_NUMBER-1);
    for(ROW=0;ROW<TFT_LINE_NUMBER;ROW++)             //ROW loop
    {

        for(column=0;column<TFT_COLUMN_NUMBER ;column++) //column loop
        {

            TFT_SEND_DATA(color>>8);
              TFT_SEND_DATA(color);
        }
    }
  }

void TFT_init(void)             ////ST7789V2
  {
    rt_pin_mode(SPI_SCK,PIN_MODE_OUTPUT);
    rt_pin_mode(SPI_SDA,PIN_MODE_OUTPUT);
    rt_pin_mode(SPI_RST,PIN_MODE_OUTPUT);
    rt_pin_mode(SPI_DC,PIN_MODE_OUTPUT);
    rt_pin_mode(SPI_CS,PIN_MODE_OUTPUT);
    rt_pin_mode(SPI_BLK,PIN_MODE_OUTPUT);
    SPI_SCK_1;          //特别注意！！
    SPI_RST_0;
    delay_ms(1000);
    SPI_RST_1;
    delay_ms(1000);
    TFT_SEND_CMD(0x11);             //Sleep Out
    delay_ms(120);               //DELAY120ms
          //-----------------------ST7789V Frame rate setting-----------------//
//************************************************
                TFT_SEND_CMD(0x3A);        //65k mode
                TFT_SEND_DATA(0x05);
                TFT_SEND_CMD(0xC5);         //VCOM
                TFT_SEND_DATA(0x1A);
                TFT_SEND_CMD(0x36);                 // 屏幕显示方向设置
                TFT_SEND_DATA(0x00);
                //-------------ST7789V Frame rate setting-----------//
                TFT_SEND_CMD(0xb2);     //Porch Setting
                TFT_SEND_DATA(0x05);
                TFT_SEND_DATA(0x05);
                TFT_SEND_DATA(0x00);
                TFT_SEND_DATA(0x33);
                TFT_SEND_DATA(0x33);

                TFT_SEND_CMD(0xb7);         //Gate Control
                TFT_SEND_DATA(0x05);            //12.2v   -10.43v
                //--------------ST7789V Power setting---------------//
                TFT_SEND_CMD(0xBB);//VCOM
                TFT_SEND_DATA(0x3F);

                TFT_SEND_CMD(0xC0); //Power control
                TFT_SEND_DATA(0x2c);

                TFT_SEND_CMD(0xC2);     //VDV and VRH Command Enable
                TFT_SEND_DATA(0x01);

                TFT_SEND_CMD(0xC3);         //VRH Set
                TFT_SEND_DATA(0x0F);        //4.3+( vcom+vcom offset+vdv)

                TFT_SEND_CMD(0xC4);         //VDV Set
                TFT_SEND_DATA(0x20);                //0v

                TFT_SEND_CMD(0xC6);             //Frame Rate Control in Normal Mode
                TFT_SEND_DATA(0X01);            //111Hz

                TFT_SEND_CMD(0xd0);             //Power Control 1
                TFT_SEND_DATA(0xa4);
                TFT_SEND_DATA(0xa1);

                TFT_SEND_CMD(0xE8);             //Power Control 1
                TFT_SEND_DATA(0x03);

                TFT_SEND_CMD(0xE9);             //Equalize time control
                TFT_SEND_DATA(0x09);
                TFT_SEND_DATA(0x09);
                TFT_SEND_DATA(0x08);
                //---------------ST7789V gamma setting-------------//
                TFT_SEND_CMD(0xE0); //Set Gamma
                TFT_SEND_DATA(0xD0);
                TFT_SEND_DATA(0x05);
                TFT_SEND_DATA(0x09);
                TFT_SEND_DATA(0x09);
                TFT_SEND_DATA(0x08);
                TFT_SEND_DATA(0x14);
                TFT_SEND_DATA(0x28);
                TFT_SEND_DATA(0x33);
                TFT_SEND_DATA(0x3F);
                TFT_SEND_DATA(0x07);
                TFT_SEND_DATA(0x13);
                TFT_SEND_DATA(0x14);
                TFT_SEND_DATA(0x28);
                TFT_SEND_DATA(0x30);

                TFT_SEND_CMD(0XE1); //Set Gamma
                TFT_SEND_DATA(0xD0);
                TFT_SEND_DATA(0x05);
                TFT_SEND_DATA(0x09);
                TFT_SEND_DATA(0x09);
                TFT_SEND_DATA(0x08);
                TFT_SEND_DATA(0x03);
                TFT_SEND_DATA(0x24);
                TFT_SEND_DATA(0x32);
                TFT_SEND_DATA(0x32);
                TFT_SEND_DATA(0x3B);
                TFT_SEND_DATA(0x14);
                TFT_SEND_DATA(0x13);
                TFT_SEND_DATA(0x28);
                TFT_SEND_DATA(0x2F);

                TFT_SEND_CMD(0x21);         //反显

                TFT_SEND_CMD(0x29);         //开启显示

  }

void display_char16_16(unsigned int x,unsigned int y,unsigned long color,unsigned char word_serial_number)
{
   unsigned int column;
  unsigned char tm=0,temp=0,xxx=0;

  TFT_SET_ADD(x,y,x+15,y+15);
  for(column=0;column<32;column++)  //column loop
          {
        temp=chines_word[  word_serial_number ][xxx];
        for(tm=0;tm<8;tm++)
        {
        if(temp&0x01)
          {
          TFT_SEND_DATA(color>>8);
          TFT_SEND_DATA(color);
          }
        else
          {
          TFT_SEND_DATA(0XFF);
          TFT_SEND_DATA(0XFF);
          }
          temp>>=1;
        }
        xxx++;

      }
}
void Picture_display(const unsigned char *ptr_pic)
{
    unsigned long  number;
    TFT_SET_ADD(0,0,PIC_LEN-1,PIC_HIG-1);
    for(number=0;number<PIC_NUM;number++)
          {
//            data=*ptr_pic++;
//            data=~data;
            TFT_SEND_DATA(*ptr_pic++);
          }
}
void display_ascii_16_16(unsigned int x,unsigned int y,unsigned long color,unsigned char word_serial_number)
{
   unsigned int column;
  unsigned char tm=0,temp=0,xxx=0;

  TFT_SET_ADD(x,y,x+15,y+15);
  for(column=0;column<32;column++)  //column loop
          {
        temp=ascii_8X16[  word_serial_number ][xxx];
        for(tm=0;tm<8;tm++)
        {
        if(temp&0x01)
          {
          TFT_SEND_DATA(color>>8);
          TFT_SEND_DATA(color);
          }
        else
          {
          TFT_SEND_DATA(0XFF);
          TFT_SEND_DATA(0XFF);
          }
          temp>>=1;
        }
        xxx++;

      }
}

void display_string(unsigned int x,unsigned int y,unsigned long color,uint8_t *data)
{
    while(*data != '\0')
    {
        display_ascii_16_16(x,y,BLACK,(*data-32));
        x+=16;
        data++;
    }
}

void Gui_DrawPoint(unsigned int x,unsigned int y,unsigned long color)
{
    TFT_SET_ADD(x,y,x+1,y+1);
    TFT_Write_HalfWord(color);
}
void Gui_Drawmap(unsigned int x,unsigned int y,unsigned long color)
{
    // 绘制一个 2x2 的正方形
    Gui_DrawPoint(x, y, color);           // 中心点
    Gui_DrawPoint(x-1, y-1, color);       // 左上
    Gui_DrawPoint(x+1, y-1, color);       // 右上
    Gui_DrawPoint(x-1, y+1, color);       // 左下
    Gui_DrawPoint(x+1, y+1, color);       // 右下
}
void Gui_Circle(uint16_t X,uint16_t Y,uint16_t R,uint16_t fc)
{//Bresenham算法
    unsigned short  a,b;
    int c;
    a=0;
    b=R;
    c=3-2*R;
    while (a<b)
    {
        Gui_DrawPoint(X+a,Y+b,fc);     //        7
        Gui_DrawPoint(X-a,Y+b,fc);     //        6
        Gui_DrawPoint(X+a,Y-b,fc);     //        2
        Gui_DrawPoint(X-a,Y-b,fc);     //        3
        Gui_DrawPoint(X+b,Y+a,fc);     //        8
        Gui_DrawPoint(X-b,Y+a,fc);     //        5
        Gui_DrawPoint(X+b,Y-a,fc);     //        1
        Gui_DrawPoint(X-b,Y-a,fc);     //        4

        if(c<0) c=c+4*a+6;
        else
        {
            c=c+4*(a-b)+10;
            b-=1;
        }
       a+=1;
    }
    if (a==b)
    {
        Gui_DrawPoint(X+a,Y+b,fc);
        Gui_DrawPoint(X+a,Y+b,fc);
        Gui_DrawPoint(X+a,Y-b,fc);
        Gui_DrawPoint(X-a,Y-b,fc);
        Gui_DrawPoint(X+b,Y+a,fc);
        Gui_DrawPoint(X-b,Y+a,fc);
        Gui_DrawPoint(X+b,Y-a,fc);
        Gui_DrawPoint(X-b,Y-a,fc);
    }

}

//画线函数，使用Bresenham 画线算法
void Gui_DrawLine(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t Color)
{
int dx,             // difference in x's
    dy,             // difference in y's
    dx2,            // dx,dy * 2
    dy2,
    x_inc,          // amount in pixel space to move during drawing
    y_inc,          // amount in pixel space to move during drawing
    error,          // the discriminant i.e. error i.e. decision variable
    index;          // used for looping

    TFT_SET_ADD(x0,y0,x0,y0);
    dx = x1-x0;//计算x距离
    dy = y1-y0;//计算y距离

    if (dx>=0)
    {
        x_inc = 1;
    }
    else
    {
        x_inc = -1;
        dx    = -dx;
    }

    if (dy>=0)
    {
        y_inc = 1;
    }
    else
    {
        y_inc = -1;
        dy    = -dy;
    }

    dx2 = dx << 1;
    dy2 = dy << 1;

    if (dx > dy)//x距离大于y距离，那么每个x轴上只有一个点，每个y轴上有若干个点
    {//且线的点数等于x距离，以x轴递增画点
        // initialize error term
        error = dy2 - dx;

        // draw the line
        for (index=0; index <= dx; index++)//要画的点数不会超过x距离
        {
            //画点
            Gui_DrawPoint(x0,y0,Color);

            // test if error has overflowed
            if (error >= 0) //是否需要增加y坐标值
            {
                error-=dx2;

                // move to next line
                y0+=y_inc;//增加y坐标值
            } // end if error overflowed

            // adjust the error term
            error+=dy2;

            // move to the next pixel
            x0+=x_inc;//x坐标值每次画点后都递增1
        } // end for
    } // end if |slope| <= 1
    else//y轴大于x轴，则每个y轴上只有一个点，x轴若干个点
    {//以y轴为递增画点
        // initialize error term
        error = dx2 - dy;

        // draw the line
        for (index=0; index <= dy; index++)
        {
            // set the pixel
            Gui_DrawPoint(x0,y0,Color);

            // test if error overflowed
            if (error >= 0)
            {
                error-=dy2;

                // move to next line
                x0+=x_inc;
            } // end if error overflowed

            // adjust the error term
            error+=dx2;

            // move to the next pixel
            y0+=y_inc;
        } // end for
    } // end else |slope| > 1
}



void Gui_box(uint16_t x, uint16_t y, uint16_t w, uint16_t h,uint16_t bc)
{
    Gui_DrawLine(x,y,x+w,y,0xEF7D);
    Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0x2965);
    Gui_DrawLine(x,y+h,x+w,y+h,0x2965);
    Gui_DrawLine(x,y,x,y+h,0xEF7D);
    Gui_DrawLine(x+1,y+1,x+1+w-2,y+1+h-2,bc);
}
void Gui_box2(uint16_t x,uint16_t y,uint16_t w,uint16_t h, uint8_t mode)
{
    if (mode==0)    {
        Gui_DrawLine(x,y,x+w,y,0xEF7D);
        Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0x2965);
        Gui_DrawLine(x,y+h,x+w,y+h,0x2965);
        Gui_DrawLine(x,y,x,y+h,0xEF7D);
        }
    if (mode==1)    {
        Gui_DrawLine(x,y,x+w,y,0x2965);
        Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0xEF7D);
        Gui_DrawLine(x,y+h,x+w,y+h,0xEF7D);
        Gui_DrawLine(x,y,x,y+h,0x2965);
    }
    if (mode==2)    {
        Gui_DrawLine(x,y,x+w,y,0xffff);
        Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0xffff);
        Gui_DrawLine(x,y+h,x+w,y+h,0xffff);
        Gui_DrawLine(x,y,x,y+h,0xffff);
    }
}


/**************************************************************************************
功能描述: 在屏幕显示一凸起的按钮框
输    入: u16 x1,y1,x2,y2 按钮框左上角和右下角坐标
输    出: 无
**************************************************************************************/
void DisplayButtonDown(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
    Gui_DrawLine(x1,  y1,  x2,y1, GRAY2);  //H
    Gui_DrawLine(x1+1,y1+1,x2,y1+1, GRAY1);  //H
    Gui_DrawLine(x1,  y1,  x1,y2, GRAY2);  //V
    Gui_DrawLine(x1+1,y1+1,x1+1,y2, GRAY1);  //V
    Gui_DrawLine(x1,  y2,  x2,y2, WHITE);  //H
    Gui_DrawLine(x2,  y1,  x2,y2, WHITE);  //V
}

/**************************************************************************************
功能描述: 在屏幕显示一凹下的按钮框
输    入: u16 x1,y1,x2,y2 按钮框左上角和右下角坐标
输    出: 无
**************************************************************************************/
void DisplayButtonUp(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
    Gui_DrawLine(x1,  y1,  x2,y1, WHITE); //H
    Gui_DrawLine(x1,  y1,  x1,y2, WHITE); //V

    Gui_DrawLine(x1+1,y2-1,x2,y2-1, GRAY1);  //H
    Gui_DrawLine(x1,  y2,  x2,y2, GRAY2);  //H
    Gui_DrawLine(x2-1,y1+1,x2-1,y2, GRAY1);  //V
    Gui_DrawLine(x2  ,y1  ,x2,y2, GRAY2); //V
}
