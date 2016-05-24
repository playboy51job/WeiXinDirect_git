/**
  ******************************************************************************
  * @file    micokit_ext.h
  * @author  Eshen Wang
  * @version V1.0.0
  * @date    8-May-2015
  * @brief   micokit extension board peripherals operations..
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, MXCHIP Inc. SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2014 MXCHIP Inc.</center></h2>
  ******************************************************************************
  */ 

#ifndef __MICOKIT_EXT_H_
#define __MICOKIT_EXT_H_

#include "Common.h"

//------------------------- MicoKit-EXT board modules drivers ------------------
#include "rgb_led/P9813/rgb_led.h"
#include "display/VGM128064/oled.h"
#include "motor/dc_motor/dc_motor.h"
#include "keypad/gpio_button/button.h"

#include "sensor/light_adc/light_sensor.h"
#include "sensor/APDS9930/apds9930.h"
#include "sensor/infrared_adc/infrared_reflective.h"


#include "motion_sensor.h"
#include "temp_hum_sensor.h"

//--------------------------- MicoKit-EXT board info ---------------------------
#define DEV_KIT_MANUFACTURER    "MXCHIP"
#define DEV_KIT_NAME            "MiCOKit3288"

#define MFG_TEST_MAX_MODULE_NUM      8

OSStatus micokit_ext_init(void);    // MicoKit-EXT board init
OSStatus user_modules_init(void);   // init modules on MicoKit-EXT board

#endif  // __MICOKIT_EXT_H_
