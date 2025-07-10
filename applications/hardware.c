/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-07     zhr       the first version
 */

#include <rtthread.h>

#define DBG_TAG "hardware"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#include <rtdevice.h>
#include <drv_common.h>

#include "lcd.h"
#include "string.h"
#include "stdio.h"
#include "ps2.h"
#include "key.h"
#include "aht10.h"
#include "lcd.h"
#include "string.h"
#include "stdio.h"
#include "hardware.h"

#define BEEP_PIN    GET_PIN(D,2)
#define LED_PIN     GET_PIN(B,6)
#define DETECT_PIN  GET_PIN(B,9)

uint8_t people_number=0;
rt_uint8_t beep_value=0;
void hardware_init(void)
{
    rt_pin_mode(BEEP_PIN,PIN_MODE_OUTPUT);
    rt_pin_mode(LED_PIN,PIN_MODE_OUTPUT);
    rt_pin_mode(DETECT_PIN,PIN_MODE_INPUT);
    rt_pin_write(BEEP_PIN, PIN_HIGH);
    rt_pin_write(LED_PIN,PIN_LOW);
    rt_thread_mdelay(2000);
}

void beep_handle(uint8_t value)
{
    if(value==1)
        rt_pin_write(BEEP_PIN, PIN_LOW);
    else if(value==0)
        rt_pin_write(BEEP_PIN, PIN_HIGH);
}

void led_solve(uint8_t value)
{
    if(value==1)
       rt_pin_write(LED_PIN, PIN_HIGH);
    else if(value==0)
       rt_pin_write(LED_PIN, PIN_LOW);
}

void people_detect(void)
{
    if(rt_pin_read(DETECT_PIN)==PIN_HIGH||beep_value==1||beep_value2==1||beep_value3==1)
    {
        rt_thread_mdelay(200);
        beep_handle(1);
        led_solve(1);
    }
    else
    {
        beep_handle(0);
        led_solve(0);
    }
}

void hardware_entry(void *parameter)
{
    hardware_init();
    uint8_t n=0;
    uint8_t n_flag=1;
    while(1)
    {
        people_detect();
        rt_thread_mdelay(50);
        if(rt_pin_read(DETECT_PIN)==PIN_HIGH&&n==0)
        {
             people_number++;
             n_flag=1;
        }
        if(n_flag==1)
        {
           if(n++>=50)
           {
               n=0;
               n_flag=0;
           }
        }
    }
}

int hardware_thread()
{
   rt_thread_t tid;
   tid = rt_thread_create("hardware", hardware_entry, RT_NULL, 1024, 7, 5);//创建线程
   rt_thread_startup(tid);//启动线程
   return RT_EOK;
}
INIT_APP_EXPORT(hardware_thread);
