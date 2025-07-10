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

#define DBG_TAG "key"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include "hardware.h"
#include "lcd.h"
#include "stdio.h"
#include "string.h"
#include "ps2.h"
#include "aht10.h"
#include "hardware.h"

#define key1 GET_PIN(A,8)
#define key2 GET_PIN(B,1)

uint8_t beep_value2;
uint8_t buf=0;
void irq_callback(void *args)
{
    beep_value2=1;
    rt_uint32_t sign = (rt_uint32_t) args;
    if(buf>number)
        buf=0;
    switch (sign)
    {
    case key1 :
        rt_kprintf("1%d\r\n",buf);
        if(buf<=99)
            buf++;
        break;
    case key2 :
        rt_kprintf("2%d\r\n",buf);
        if(buf>0)
            buf--;
        break;
    default:
        rt_kprintf("error sign= %d !", sign);
        break;
    }
}

void key_handle(void *parameter)
{
   rt_pin_mode(key1,PIN_MODE_INPUT_PULLUP);
   rt_pin_mode(key2,PIN_MODE_INPUT_PULLUP);

   rt_pin_attach_irq(key1, PIN_IRQ_MODE_FALLING,irq_callback, (void *)key1);
   rt_pin_attach_irq(key2, PIN_IRQ_MODE_FALLING,irq_callback, (void *)key2);

   rt_pin_irq_enable(key1, PIN_IRQ_ENABLE);
   rt_pin_irq_enable(key2, PIN_IRQ_ENABLE);
   uint8_t i=0;
   while(1)
   {
       if(beep_value2==1)
       {
           if(i++>=25)
           {
               beep_value2=0;
               i=0;
           }
       }
       rt_thread_mdelay(10);
   }
}

int key_thread()
{
   rt_thread_t tid;
   tid = rt_thread_create("key", key_handle, RT_NULL, 1024, 7, 5);//创建线程
   rt_thread_startup(tid);//启动线程
   return RT_EOK;
}

INIT_APP_EXPORT(key_thread);
