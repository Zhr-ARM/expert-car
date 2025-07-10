/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-05     zhr       the first version
 */
#ifndef APPLICATIONS_PS2_H_
#define APPLICATIONS_PS2_H_

#include "drv_common.h"

#define CLK     GET_PIN(A,0)
#define CS      GET_PIN(A,1)
#define DO      GET_PIN(A,2)
#define DI_PIN  GET_PIN(A,3)

#define DI   rt_pin_read(DI_PIN)           //PA3  输入

#define DO_H rt_pin_write(DO, PIN_HIGH)       //命令位高
#define DO_L rt_pin_write(DO, PIN_LOW)        //命令位低

#define CS_H rt_pin_write(CS, PIN_HIGH)       //CS拉高
#define CS_L rt_pin_write(CS, PIN_LOW)      //CS拉低

#define CLK_H rt_pin_write(CLK, PIN_HIGH)     //时钟拉高
#define CLK_L rt_pin_write(CLK, PIN_LOW)    //时钟拉低

#define PSB_SELECT      1
#define PSB_L3          2
#define PSB_R3          3
#define PSB_START       4
#define PSB_PAD_UP      5
#define PSB_PAD_RIGHT   6
#define PSB_PAD_DOWN    7
#define PSB_PAD_LEFT    8
#define PSB_L2          9
#define PSB_R2          10
#define PSB_L1          11
#define PSB_R1          12
#define PSB_GREEN       13
#define PSB_RED         14
#define PSB_BLUE        15
#define PSB_PINK        16

#define PSB_TRIANGLE    13
#define PSB_CIRCLE      14
#define PSB_CROSS       15
#define PSB_SQUARE      16

//#define WHAMMY_BAR        8

//These are stick values
#define PSS_RX 5                //右摇杆x轴
#define PSS_RY 6
#define PSS_LX 7
#define PSS_LY 8

extern uint8_t Data[9];
extern uint16_t MASK[16];
extern uint16_t Handkey;

void PS2_Init(void);
uint8_t PS2_RedLight(void);   //判断是否为红灯模式
void PS2_ReadData(void); //读手柄数据
void PS2_Cmd(uint8_t CMD);        //向手柄发送命令
uint8_t PS2_DataKey(void);        //按键值读取
uint8_t PS2_AnologData(uint8_t button); //得到一个摇杆的模拟量
void PS2_ClearData(void);     //清除数据缓冲区
void PS2_Vibration(uint8_t motor1, uint8_t motor2);//振动设置motor1  0xFF开，其他关，motor2  0x40~0xFF

void PS2_EnterConfing(void);     //进入配置
void PS2_TurnOnAnalogMode(void); //发送模拟量
void PS2_VibrationMode(void);    //振动设置
void PS2_ExitConfing(void);      //完成配置
void PS2_SetInit(void);          //配置初始化
void PS2_Receive (void);

extern int PS2_LX, PS2_LY, PS2_RX, PS2_RY, PS2_KEY;

void delay_us(uint32_t nus);

#endif /* APPLICATIONS_PS2_H_ */
