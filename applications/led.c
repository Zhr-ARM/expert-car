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

#define DBG_TAG "led"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#include <rtdevice.h>
#include <drv_common.h>

#define PIN_LED1  GET_PIN(C, 13)

void led_handle(void *parameter)
{
    rt_pin_mode(PIN_LED1, PIN_MODE_OUTPUT);

    while(1)
    {
    rt_pin_write(PIN_LED1,PIN_LOW);
    rt_thread_mdelay(500);
    rt_pin_write(PIN_LED1,PIN_HIGH);
    rt_thread_mdelay(500);

    }
}

int led_thread()
{
   rt_thread_t tid;
   tid = rt_thread_create("led", led_handle, RT_NULL, 1024, 20, 10);//创建线程
   rt_thread_startup(tid);//启动线程
   return RT_EOK;
}
//INIT_APP_EXPORT(led_thread);
