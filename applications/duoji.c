#include "duoji.h"
    #include <rtthread.h>
    #include <rtdevice.h>
    #include <board.h>
    #include "drv_common.h"

    #define DBG_TAG "duoji"
    #define DBG_LVL DBG_LOG
    #include <rtdbg.h>

#include "ps2.h"

static struct rt_device_pwm* pwm_dev;

rt_uint32_t duoji1_period = 10000000-1;
rt_uint32_t duoji1_pulse = 750000;

rt_uint32_t duoji2_period = 10000000-1;
rt_uint32_t duoji2_pulse = 750000;

rt_uint8_t duoji1_flag=0;
rt_uint8_t duoji2_flag=0;

int pwm_init(void)
{
    pwm_dev = (struct rt_device_pwm*)rt_device_find(PWM_DEV_NAME);
    if(pwm_dev == RT_NULL){
        rt_kprintf("PWM device not found\n");
        return -1;
    }
    rt_err_t temp1 = rt_pwm_set(pwm_dev,PWM_DUOJI1_CHANNEL,duoji1_period,duoji1_pulse);
    rt_err_t temp2 = rt_pwm_set(pwm_dev,PWM_DUOJI2_CHANNEL,duoji2_period,duoji2_pulse);
    if(temp1!=RT_EOK)
    {
        rt_kprintf("Failed to set PWM parameters for channel3 %d\n",duoji1_pulse);
    }
    else
    {
        rt_kprintf("set PWM parameters for channel3 %d\n",duoji1_pulse);
    }
    if(temp2!=RT_EOK)
        {
            rt_kprintf("Failed to set PWM parameters for channel4 %d\n",duoji2_pulse);
        }
        else
        {
            rt_kprintf("set PWM parameters for channel4 %d\n",duoji2_pulse);
        }
    rt_err_t temp3 = rt_pwm_enable(pwm_dev,PWM_DUOJI1_CHANNEL);
    rt_err_t temp4 = rt_pwm_enable(pwm_dev,PWM_DUOJI2_CHANNEL);
    if(temp3!=RT_EOK)
    {
        rt_kprintf("Failed to enable PWM parameters for channel3 %d\n",duoji1_pulse);
    }
    else
    {
        rt_kprintf("enable PWM parameters for channel3 %d\n",duoji1_pulse);
    }
    if(temp4!=RT_EOK)
    {
        rt_kprintf("Failed to enable PWM parameters for channel4 %d\n",duoji2_pulse);
    }
    else
    {
        rt_kprintf("enable PWM parameters for channel4 %d\n",duoji2_pulse);
    }
    return RT_EOK;
}

void duoji_handle(void *parameter)
{
    pwm_init();
    int duoji1_speed=0;
    int duoji2_speed=0;
    uint8_t ps2_flag=0;
    uint8_t i=0;
    while(1)
    {
        rt_thread_mdelay(10);
        if(ps2_flag==1)
        {
            if(i++>=50)
            {
                i=0;
                ps2_flag=0;
            }
        }
        if(PS2_KEY==8)
        {
            ps2_flag=1;
            duoji1_pulse=950000;
            duoji2_pulse=600000;
        }
        if(PS2_RX!=63&&ps2_flag==0)
        {
        if(PS2_RY!=63)
            duoji1_speed = -(PS2_RY-128)*50;
        else
            duoji1_speed = 0;
        if((duoji1_speed+duoji1_pulse)<1250000&&(duoji1_speed+duoji1_pulse)>325000)
            duoji1_pulse += duoji1_speed;
        duoji2_speed = -(PS2_RX-127)*50;
        if((duoji2_speed+duoji2_pulse)<1250000&&(duoji2_speed+duoji2_pulse)>325000)
            duoji2_pulse += duoji2_speed;
        rt_pwm_set(pwm_dev,PWM_DUOJI1_CHANNEL,duoji1_period,duoji1_pulse);
        rt_pwm_set(pwm_dev,PWM_DUOJI2_CHANNEL,duoji2_period,duoji2_pulse);
        }
    }
}

int duoji_thread()
{
   rt_thread_t tid;
   tid = rt_thread_create("duoji", duoji_handle, RT_NULL, 2048, 10,40);//创建线程
   rt_thread_startup(tid);//启动线程
   return RT_EOK;
}


INIT_APP_EXPORT(duoji_thread);
