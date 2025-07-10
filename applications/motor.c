/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-06     zhr       the first version
 */
#include <rtthread.h>

#define DBG_TAG "motor"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#include <rtdevice.h>
#include <drv_common.h>
#include "motor.h"
#include "ps2.h"

static struct rt_device_pwm* pwm_dev;

rt_uint32_t A_Period = 320000;
rt_uint32_t A_Pulse = 0;

rt_uint32_t B_Period = 320000;
rt_uint32_t B_Pulse = 0;

#define THREAD_PRIORITY 10
#define THREAD_TIMESLICE 400

float limit_change(float change, float max_change_rate) {
            if (change > max_change_rate) return max_change_rate;
            if (change < -max_change_rate) return -max_change_rate;
            return change;
        }

static void pwm_init(void)
{
    pwm_dev = (struct rt_device_pwm*)rt_device_find(PWM_DEV1_NAME);
    if(pwm_dev==RT_NULL)
        rt_kprintf("PWM device not found\n");
    rt_err_t temp1=rt_pwm_set(pwm_dev,PWMA_CHANNEL,A_Period,A_Pulse);
    rt_err_t temp2=rt_pwm_set(pwm_dev,PWMB_CHANNEL,B_Period,B_Pulse);
    if(temp1!=RT_EOK)
        rt_kprintf("pwm pulse not be setted with channel1 to %d\n",A_Pulse);
    else
        rt_kprintf("pwm pulse has be setted with channel1 to %d\n",A_Pulse);
    if(temp2!=RT_EOK)
        rt_kprintf("pwm pulse not be setted with channel2 to %d\n",B_Pulse);
    else
        rt_kprintf("pwm pulse has be setted with channel2 to %d\n",B_Pulse);
    rt_err_t temp3=rt_pwm_enable(pwm_dev,PWMA_CHANNEL);
    rt_err_t temp4=rt_pwm_enable(pwm_dev,PWMB_CHANNEL);
    if(temp3!=RT_EOK)
        rt_kprintf("PWMA_CHANNEL not enable\n");
    else
        rt_kprintf("PWMA_CHANNEL enable successfully\n");
    if(temp4!=RT_EOK)
        rt_kprintf("PWMB_CHANNEL not enable\n");
    else
        rt_kprintf("PWMB_CHANNEL enable successfully\n");

}
static void gpio_init(void)
{
    rt_pin_mode(AIN1, PIN_MODE_OUTPUT);
    rt_pin_mode(AIN2, PIN_MODE_OUTPUT);
    rt_pin_mode(BIN1, PIN_MODE_OUTPUT);
    rt_pin_mode(BIN2, PIN_MODE_OUTPUT);
}

void pwm_Amotor_set(rt_uint32_t value)
{
    rt_pwm_set(pwm_dev, PWMA_CHANNEL, A_Period, value);
}

void pwm_Bmotor_set(rt_uint32_t value)
{
    rt_pwm_set(pwm_dev, PWMB_CHANNEL, B_Period, value);
}

void A_motor_forward(void)
{
    rt_pin_write(AIN2,PIN_LOW);
    rt_pin_write(AIN1,PIN_HIGH);
}

void A_motor_backward(void)
{
    rt_pin_write(AIN1,PIN_LOW);
    rt_pin_write(AIN2,PIN_HIGH);
}

void B_motor_forward(void)
{
    rt_pin_write(BIN1,PIN_LOW);
    rt_pin_write(BIN2,PIN_HIGH);
}

void B_motor_backward(void)
{
    rt_pin_write(BIN1,PIN_HIGH);
    rt_pin_write(BIN2,PIN_LOW);
}

rt_int32_t myabs(rt_int32_t a)
{
    rt_int32_t temp;

    if(a < 0)
        temp = -a;  // 如果a小于0，取其负值
    else
        temp = a;  // 否则直接返回a

    return temp;  // 返回绝对值
}

void motor_pwm_set(rt_int32_t motor1, rt_int32_t motor2)
{
    if(motor1 > 0)  // 判断左电机的转向
    {
        A_motor_backward();  // 左电机正转
        pwm_Amotor_set(myabs(motor1));  // 设置左电机的PWM占空比
    }
    else
    {
        A_motor_forward();  // 左电机反转
        pwm_Amotor_set(myabs(motor1));  // 设置左电机的PWM占空比
    }
    if(motor2 > 0)  // 判断右电机的转向
    {
        B_motor_backward();  // 右电机正转
        pwm_Bmotor_set(myabs(motor2));  // 设置右电机的PWM占空比
    }
    else
    {
        B_motor_forward();  // 右电机反转
        pwm_Bmotor_set(myabs(motor2));  // 设置右电机的PWM占空比
    }
}

ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[1024];
static struct rt_thread pwm_thread;

static void motor_control(void *param)
{
    pwm_init();
    gpio_init();
    rt_int32_t speeda=0;
    rt_int32_t speedb=0;
    rt_int32_t  delta=0;
    rt_int32_t  usual=0;
    // 预定义的变量
    rt_int32_t prev_usual = 0;
    rt_int32_t prev_delta = 0;
    float alpha = 0.2;  // 平滑因子（0 - 1，越小平滑效果越强）
    rt_int32_t max_change_rate = 3280;  // 控制每次变化的最大步长

    while(1)
    {


        // 过滤器和增量限制后控制信号计算
        usual = (128 - PS2_LY) * 2400;
        delta = (127 - PS2_LX) * 1600;

        // 1. 使用低通滤波器平滑摇杆输入
        usual = alpha * usual + (1 - alpha) * prev_usual;
        delta = alpha * delta + (1 - alpha) * prev_delta;

        // 2. 限制每次变化的最大幅度
        usual = prev_usual + limit_change(usual - prev_usual, max_change_rate);
        delta = prev_delta + limit_change(delta - prev_delta, max_change_rate);

        // 3. 速度限制：限制最大速度
        if (usual > 320000) usual = 320000;
        if (delta > 256000) delta = 256000;

        // 计算电机的转速
        speeda = usual + delta;
        speedb = usual - delta;
        if (speeda > 320000) speeda = 320000;
        else if(speeda < -320000)   speeda=-320000;
        if (speedb > 256000) speedb = 256000;
        else if(speedb <-256000) speedb=-256000;
        // 更新前一时刻的值
        prev_usual = usual;
        prev_delta = delta;

        // 设置电机PWM信号
        motor_pwm_set(speeda, speedb);

        rt_thread_mdelay(10);
        // 辅助函数：限制变化幅度

    }
}

int motor_thread()
{
    rt_thread_init(&pwm_thread,
                    "motor",
                    motor_control,
                    RT_NULL,
                    &thread2_stack[0],
                    sizeof(thread2_stack),
                    THREAD_PRIORITY - 1,
                    THREAD_TIMESLICE);
    rt_thread_startup(&pwm_thread);
    return 0;
}

INIT_APP_EXPORT(motor_thread);
