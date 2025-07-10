/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-05     zhr       the first version
 */
#ifndef APPLICATIONS_DUOJI_H_
#define APPLICATIONS_DUOJI_H_

#include "drv_common.h"

int pwm_init(void);

#define PWM_DEV_NAME  "pwm8"
#define PWM_DUOJI1_CHANNEL 3
#define PWM_DUOJI2_CHANNEL 4

extern rt_uint32_t duoji1_period;
extern rt_uint32_t duoji1_pulse;

extern rt_uint32_t duoji2_period;
extern rt_uint32_t duoji2_pulse;

#endif /* APPLICATIONS_DUOJI_H_ */
