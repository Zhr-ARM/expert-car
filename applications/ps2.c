/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-05     zhr       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "drv_common.h"

#define DBG_TAG "ps2.h"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include "ps2.h"
#include "hardware.h"

#define DELAY_TIME rt_hw_us_delay(5);

int PS2_LX, PS2_LY, PS2_RX, PS2_RY, PS2_KEY;
uint16_t Handkey;                                                         // 按键值读取，零时读取
uint8_t Comd[2] = {0x01, 0x42};                                           // 开始命令，请求数据
uint8_t Data[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // 数据存储数据
uint16_t MASK[] = {
    PSB_SELECT,
    PSB_L3,
    PSB_R3,
    PSB_START,
    PSB_PAD_UP,
    PSB_PAD_RIGHT,
    PSB_PAD_DOWN,
    PSB_PAD_LEFT,
    PSB_L2,
    PSB_R2,
    PSB_L1,
    PSB_R1,
    PSB_GREEN,
    PSB_RED,
    PSB_BLUE,
    PSB_PINK}; // 按键值和按键名

void PS2_Init(void)
{
    rt_pin_mode(CS,PIN_MODE_OUTPUT);
    rt_pin_mode(DO,PIN_MODE_OUTPUT);
    rt_pin_mode(CLK,PIN_MODE_OUTPUT);
    rt_pin_mode(DI_PIN,PIN_MODE_INPUT_PULLUP);
    CS_H;  // CS拉高
    CLK_H; // 时钟拉高
    DO_H;  // 命令位拉高
}

void PS2_Cmd(uint8_t CMD)
{
    volatile uint16_t ref = 0x01;
    Data[1] = 0;
    for (ref = 0x01; ref < 0x0100; ref <<= 1)
    {
        if (ref & CMD)
        {
            DO_H; // 输出一位控制位
        }
        else
            DO_L;

        CLK_H; // 时钟拉高
        DELAY_TIME;
        CLK_L;
        DELAY_TIME;
        CLK_H;
        if (DI)
            Data[1] = ref | Data[1];
    }
    rt_hw_us_delay(16);
}
//判断是否为红灯模式,0x41=模拟绿灯，0x73=模拟红灯
//返回值；0，红灯模式
//        其他，其他模式
uint8_t PS2_RedLight(void)
{
    CS_L;
    PS2_Cmd(Comd[0]);  //开始命令
    PS2_Cmd(Comd[1]);  //请求数据
    CS_H;
    if (Data[1] == 0X73)
        return 0;
    else
        return 1;

}

//读取手柄数据
void PS2_ReadData(void)
{
    volatile uint8_t byte = 0;
    volatile uint16_t ref = 0x01;
    CS_L;
    PS2_Cmd(Comd[0]);  //开始命令
    PS2_Cmd(Comd[1]);  //请求数据
    for (byte = 2; byte < 9; byte++)          //开始接受数据
    {
        for (ref = 0x01; ref < 0x100; ref <<= 1)
        {
            CLK_H;
            DELAY_TIME;
            CLK_L;
            DELAY_TIME;
            CLK_H;
              if (DI)
                  Data[byte] = ref | Data[byte];
        }
        rt_hw_us_delay(16);
    }
    CS_H;
}

//对读出来的PS2的数据进行处理,只处理按键部分
//只有一个按键按下时按下为0， 未按下为1
uint8_t PS2_DataKey()
{
    uint8_t index;

    PS2_ClearData();
    PS2_ReadData();

    Handkey = (Data[4] << 8) | Data[3];     //这是16个按键  按下为0， 未按下为1
    for (index = 0; index < 16; index++)
    {
        if ((Handkey & (1 << (MASK[index] - 1))) == 0)
            return index + 1;
    }
    return 0;          //没有任何按键按下
}

//得到一个摇杆的模拟量     范围0~256
uint8_t PS2_AnologData(uint8_t button)
{
    return Data[button];
}

//清除数据缓冲区
void PS2_ClearData(void)
{
    uint8_t a;
    for (a = 0; a < 9; a++)
        Data[a] = 0x00;
}

/******************************************************
Function:    void PS2_Vibration(u8 motor1, u8 motor2)
Description: 手柄震动函数，
Calls:       void PS2_Cmd(u8 CMD);
Input: motor1:右侧小震动电机 0x00关，其他开
       motor2:左侧大震动电机 0x40~0xFF 电机开，值越大 震动越大
******************************************************/
void PS2_Vibration(uint8_t motor1, uint8_t motor2)
{
    CS_L;
    rt_hw_us_delay(16);
    PS2_Cmd(0x01);  //开始命令
    PS2_Cmd(0x42);  //请求数据
    PS2_Cmd(0X00);
    PS2_Cmd(motor1);
    PS2_Cmd(motor2);
    PS2_Cmd(0X00);
    PS2_Cmd(0X00);
    PS2_Cmd(0X00);
    PS2_Cmd(0X00);
    CS_H;
    rt_hw_us_delay(16);
}
//short poll
void PS2_ShortPoll(void)
{
    CS_L;
    rt_hw_us_delay(16);
    PS2_Cmd(0x01);
    PS2_Cmd(0x42);
    PS2_Cmd(0X00);
    PS2_Cmd(0x00);
    PS2_Cmd(0x00);
    CS_H;
    rt_hw_us_delay(16);
}
//进入配置
void PS2_EnterConfing(void)
{
    CS_L;
    rt_hw_us_delay(16);
    PS2_Cmd(0x01);
    PS2_Cmd(0x43);
    PS2_Cmd(0X00);
    PS2_Cmd(0x01);
    PS2_Cmd(0x00);
    PS2_Cmd(0X00);
    PS2_Cmd(0X00);
    PS2_Cmd(0X00);
    PS2_Cmd(0X00);
    CS_H;
    rt_hw_us_delay(16);
}
//发送模式设置
void PS2_TurnOnAnalogMode(void)
{
    CS_L;
    PS2_Cmd(0x01);
    PS2_Cmd(0x44);
    PS2_Cmd(0X00);
    PS2_Cmd(0x01); //analog=0x01;digital=0x00  软件设置发送模式
    PS2_Cmd(0x03); //Ox03锁存设置，即不可通过按键“MODE”设置模式。
                   //0xEE不锁存软件设置，可通过按键“MODE”设置模式。
    PS2_Cmd(0X00);
    PS2_Cmd(0X00);
    PS2_Cmd(0X00);
    PS2_Cmd(0X00);
    CS_H;
    rt_hw_us_delay(16);
}
//振动设置
void PS2_VibrationMode(void)
{
    CS_L;
    rt_hw_us_delay(16);
    PS2_Cmd(0x01);
    PS2_Cmd(0x4D);
    PS2_Cmd(0X00);
    PS2_Cmd(0x00);
    PS2_Cmd(0X01);
    CS_H;
    rt_hw_us_delay(16);
}
//完成并保存配置
void PS2_ExitConfing(void)
{
    CS_L;
    rt_hw_us_delay(16);
    PS2_Cmd(0x01);
    PS2_Cmd(0x43);
    PS2_Cmd(0X00);
    PS2_Cmd(0x00);
    PS2_Cmd(0x5A);
    PS2_Cmd(0x5A);
    PS2_Cmd(0x5A);
    PS2_Cmd(0x5A);
    PS2_Cmd(0x5A);
    CS_H;
    rt_hw_us_delay(16);
}

//手柄配置初始化
void PS2_SetInit(void)
{
    PS2_ShortPoll();
    PS2_ShortPoll();
    PS2_ShortPoll();
    PS2_EnterConfing();     //进入配置模式
    PS2_TurnOnAnalogMode(); //“红绿灯”配置模式，并选择是否保存
    PS2_VibrationMode();    //开启震动模式
    PS2_ExitConfing();      //完成并保存配置
}

//读取手柄信息
void PS2_Receive (void)
{
        PS2_LX=PS2_AnologData(PSS_LX);
        PS2_LY=PS2_AnologData(PSS_LY);
        PS2_RX=PS2_AnologData(PSS_RX);
        PS2_RY=PS2_AnologData(PSS_RY);
        PS2_KEY=PS2_DataKey();
}

void PS2_handle(void *parameter)
{
    PS2_Init();    //=====ps2驱动端口初始化
    PS2_SetInit(); //=====ps2配置初始化,配置“红绿灯模式”，并选择是否可以修改
    PS2_Vibration(0x00, 0x40); //=====设置振动，motor1  0xFF开，其他关，motor2  0x40~0xFF
    rt_thread_mdelay(250);
    while(1)
    {
        PS2_LX = PS2_AnologData(PSS_LX);
        PS2_LY = PS2_AnologData(PSS_LY);
        PS2_RX = PS2_AnologData(PSS_RX);
        PS2_RY = PS2_AnologData(PSS_RY);
        PS2_KEY = PS2_DataKey();
        if(PS2_KEY !=0) beep_value=1;
        else beep_value=0;

        rt_kprintf("%d %d %d %d %d\n",PS2_LX,PS2_LY,PS2_RX,PS2_RY,PS2_KEY);
        rt_thread_mdelay(50);
    }
}

int PS2_Thread(void)
{
    rt_thread_t tid;
    tid = rt_thread_create("ps2", PS2_handle, RT_NULL, 4096,8,5);//创建线程
    rt_thread_startup(tid);//启动线程
    return RT_EOK;
}
INIT_APP_EXPORT(PS2_Thread);

