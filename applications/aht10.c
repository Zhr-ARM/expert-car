/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-08     zhr       the first version
 */
#include <rtthread.h>

#define DBG_TAG "ath10"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#include <rtdevice.h>
#include <drv_common.h>
#include "lcd.h"
#include "string.h"
#include "stdio.h"
#include "key.h"
#include "ps2.h"
#include "hardware.h"


#define DHT11_PIN GET_PIN(C,12)
#define DHT11_DQ_OUT(x)     do{ x ? \
                                rt_pin_write(DHT11_PIN,PIN_HIGH):    \
                                rt_pin_write(DHT11_PIN,PIN_LOW); \
                            }while(0)
#define DHT11_DQ_IN         rt_pin_read(DHT11_PIN)
#define delay_ms(x) rt_thread_mdelay(x)
#define delay_us(x) rt_hw_us_delay(x/2)

char dht11_data[5] = {0};

void dht11_gpio_input(void)
{
    rt_pin_mode(DHT11_PIN, PIN_MODE_INPUT);
}

void dht11_gpio_output(void)
{
    rt_pin_mode(DHT11_PIN, PIN_MODE_OUTPUT);
}

void dht11_start(void)
{
    dht11_gpio_output();
    DHT11_DQ_OUT(1);
    DHT11_DQ_OUT(0);
    delay_ms(20);
    DHT11_DQ_OUT(1);

    dht11_gpio_input();
    while(DHT11_DQ_IN);     //等待DHT11拉低电平
    while(!DHT11_DQ_IN);    //等待DHT11拉高电平
    while(DHT11_DQ_IN);     //等待DHT11拉低电平
}

uint8_t dht11_read_byte(void)
{
    uint8_t temp = 0;
    uint8_t i = 0;
    uint8_t read_data = 0;

    for(i = 0; i < 8; i++)
    {
        while(!DHT11_DQ_IN);
        delay_us(50);
        if(DHT11_DQ_IN == 1)
        {
            temp = 1;
            while(DHT11_DQ_IN);
        }
        else
            temp = 0;

        read_data = read_data << 1;
        read_data |= temp;
    }

    return read_data;
}

void dht11_read(uint8_t *result)
{
    uint8_t i = 0;

    dht11_start();
    dht11_gpio_input();

    for(i = 0; i < 5; i++)
        dht11_data[i] = dht11_read_byte();

    if(dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3] == dht11_data[4])
    {
        memcpy(result, dht11_data, 4);
        rt_kprintf("%d.%dRH ,", dht11_data[0], dht11_data[1]);
        rt_kprintf("%d.%dCH\r\n", dht11_data[2], dht11_data[3]);
    }

}
float humidity, temperature;
char humidity_buf[20]={0};
char temperature_buf[20]={0};


extern ADC_HandleTypeDef hadc2;
extern void MX_ADC2_Init(void);
extern ADC_ChannelConfTypeDef sConfig;
uint16_t ADC_Value[]={0};

char power_buf[40]={0};
char detect_buf[40]={0};
void Get_adc()
{

    // 通道 0
    sConfig.Channel = ADC_CHANNEL_5;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_24CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    HAL_ADC_ConfigChannel(&hadc2, &sConfig);

    HAL_ADC_Start(&hadc2);
    HAL_ADC_PollForConversion(&hadc2, 100);
    ADC_Value[0] = HAL_ADC_GetValue(&hadc2);
    HAL_ADC_Stop(&hadc2);

    // 通道 1
    sConfig.Channel = ADC_CHANNEL_11;
    HAL_ADC_ConfigChannel(&hadc2, &sConfig);

    HAL_ADC_Start(&hadc2);
    HAL_ADC_PollForConversion(&hadc2, 100);
    ADC_Value[1] = HAL_ADC_GetValue(&hadc2);
    HAL_ADC_Stop(&hadc2);
}
uint16_t voltage1;
uint16_t voltage2;
uint8_t review_buf[20][3]={0};
char string1[20]={0};
char string2[20]={0};
char string3[20]={0};
char string4[10]={0};
uint8_t number=0;
uint8_t key_flag=0;
uint8_t dht11_result[4];
char people_buf[20]={0};
static void aht10_handle(void *parameter)
{
    MX_ADC2_Init();
    humidity = 0.0;
    temperature = 0.0;
    BL_1;
    SPI_SCK_0;
    TFT_init();
    TFT_full(WHITE);
    display_string(0, 0, GREEN, (uint8_t*)"humidity:");
    display_string(0, 40, GREEN, (uint8_t*)"temperature:");
    display_string(0, 100, BLACK, (uint8_t*)"smoke:");
    display_string(0, 260, BLUE, (uint8_t*)"find people");
    rt_thread_mdelay(3000);
    while(1)
    {
        for(uint8_t i=0;i<2;i++)
        {
             Get_adc();//分别存放通道的ADC
             if(i==0)
             {
                 voltage1= ADC_Value[0] * 330 / 4096 ;
                 sprintf(power_buf,"volt:%d.%d V",(int)(voltage1/100*11),(voltage1%100*11)+160);
                 display_string(0, 80, BLACK, (uint8_t*)power_buf);
             }
             else
             {
                 voltage2= ADC_Value[1] * 330 / 4096 ;
                 sprintf(detect_buf,"%d.%02d",(int)(ADC_Value[1]/25),(int)(ADC_Value[1]*4));
                 display_string(0, 120, BLACK, (uint8_t*)detect_buf);

             }
        }
        sprintf(humidity_buf,"%d.%d",(int)dht11_data[0],(int)dht11_data[1]);
        sprintf(temperature_buf,"%d.%d",(int)dht11_data[2], (int)dht11_data[3]);
        display_string(0, 20, BLACK, (uint8_t*)humidity_buf);
        display_string(0, 60, BLACK, (uint8_t*)temperature_buf);
        sprintf(people_buf,"%d",(int)people_number);
        display_string(0, 280, BLACK, (uint8_t*)people_buf);
                if(key_flag==1)
                {
                    sprintf(string1,"%d.%02d",(int)(review_buf[buf][2]/25),(int)(review_buf[buf][2]*4));
                    sprintf(string2,"%d.%d",(int)review_buf[buf][0],(int)(review_buf[buf][0]*10)%10);
                    sprintf(string3,"%d.%d",(int)review_buf[buf][1], (int)(review_buf[buf][1] * 10) % 10);
                    sprintf(string4," note:%d",buf);
                    display_string(0, 140, BLACK, (uint8_t*)string1);
                    display_string(0, 160, BLACK, (uint8_t*)string2);
                    display_string(0, 180, BLACK, (uint8_t*)string3);
                    display_string(0, 200, BLACK, (uint8_t*)string4);
                }
                else if(key_flag==0)
                {
                    memset(string1,0,sizeof(string1));
                    memset(string2,0,sizeof(string2));
                    memset(string3,0,sizeof(string3));
                    memset(string4,0,sizeof(string4));
                    display_string(0, 140, BLACK,  (uint8_t*)"                   ");
                    display_string(0, 160, BLACK,  (uint8_t*)"                   ");
                    display_string(0, 180, BLACK,  (uint8_t*)"                   ");
                    display_string(0, 200, BLACK,  (uint8_t*)"                   ");
                }
                rt_thread_mdelay(1000);
    }
}

int aht10_thread()
{
   rt_thread_t tid;
   tid = rt_thread_create("aht10", aht10_handle, RT_NULL, 4096,10,10);//创建线程
   rt_thread_startup(tid);//启动线程
   return RT_EOK;
}
INIT_APP_EXPORT(aht10_thread);
uint8_t beep_value3 = 0;
static void dht11_handle(void *parameter)
{
    uint8_t n=0;
    uint8_t i=0;
    while(1)
    {
        if(i++>=200)
        {
            if(dht11_data[0]>=90||dht11_data[0]<=10)
            {
                display_string(0, 220, BLACK, (uint8_t*)"humi error");
                beep_value3=1;
            }
            else if(dht11_data[2]>=40||dht11_data[2]<=1)
            {
                display_string(0, 240, BLACK, (uint8_t*)"temp error");
                beep_value3=1;
            }
            i=0;
        }


        if(n++>=50)
        {
        memset(dht11_result, 0, 4);
        dht11_read(dht11_result);
        n=0;
        }
        if(PS2_KEY==13)
        {
            review_buf[number][0]=(dht11_data[2]*10)+dht11_data[3];
            review_buf[number][1]=(dht11_data[0]*10)+dht11_data[1];
            review_buf[number++][2]=ADC_Value[1];
            //rt_kprintf("4%d\r\n",number);
        }
        if(PS2_KEY==12)
        {
            key_flag=1;
            rt_kprintf("12\r\n");
        }
        else if(PS2_KEY==10)
        {
           key_flag=0;
           rt_kprintf("10\r\n");
           for (int n = 0; n < number+1; n++) {
               for (int j = 0; j < 3; j++) {
               review_buf[number][j] = 0;
               }
           number=0;
           buf=0;
           }
        }
        rt_thread_mdelay(100);
    }
}
int dht11_thread()
{
   rt_thread_t tid;
   tid = rt_thread_create("dht11", dht11_handle, RT_NULL, 1024,7,5);//创建线程
   rt_thread_startup(tid);//启动线程
   return RT_EOK;
}
INIT_APP_EXPORT(dht11_thread);



