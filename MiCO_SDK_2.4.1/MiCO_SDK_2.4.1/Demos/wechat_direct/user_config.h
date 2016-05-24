/**
  ******************************************************************************
  * @file    user_config.h 
  * @author  Eshen Wang
  * @version V1.0.0
  * @date    02-Mar-2016
  * @brief   This file provide user configurations for MiCO application.
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

#pragma once

#include "mico.h"

#ifdef __cplusplus
extern "C" {
#endif

/*User provided configurations*/
#define CONFIGURATION_VERSION               0x20000001 // if default configuration is changed, update this number
/*Application's configuration stores in flash, and loaded to ram when system boots up*/
typedef struct
{
  uint32_t          configDataVer;
  /*Wechat data*/
  mico_semaphore_t weixin_auth_sem;
  bool power_switch;
  unsigned int store;//¥Ê¥¢±Í÷æ
  char weixinapi_token[200];
  char deviceid[50];
  char devicelicence[300];
} application_config_t;

typedef struct _app_context_t
{
  /*Flash content*/
  application_config_t*     appConfig;
  
} app_context_t;

#ifdef __cplusplus
} /*extern "C" */
#endif
