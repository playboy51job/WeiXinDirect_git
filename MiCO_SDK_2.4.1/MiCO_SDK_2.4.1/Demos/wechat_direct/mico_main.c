/**
  ******************************************************************************
  * @file    mico_main.c 
  * @author  William Xu
  * @version V1.0.0
  * @date    05-May-2014
  * @brief   Mico application entrance, addd user application functons and threads.
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

#include "mico.h"
#include "user_config.h"
#include "airkiss_cloud.h"
#include "WeiXinAuth.h"


#ifdef USE_MiCOKit_EXT
#include "MiCOKit_EXT/micokit_ext.h"
#endif

#define mico_main_log(format, ...)  custom_log("mico_main", format, ##__VA_ARGS__)

app_context_t* g_app_context = NULL;
mico_Context_t* g_mico_context = NULL;
void check_linkstatus(void);//检查链路情况

/****************************************************************************/
/* MICO system callback: Restore default configuration provided by application */
void appRestoreDefault_callback(void * const user_config_data, uint32_t size)
{
  UNUSED_PARAMETER(size);
  application_config_t* appConfig = user_config_data;
  
  /*restore user data here*/
  appConfig->configDataVer = CONFIGURATION_VERSION;
  appConfig->store=0;

  appConfig->power_switch = false;
  memset(appConfig->weixinapi_token,0,sizeof(appConfig->weixinapi_token));
  memset(appConfig->deviceid,0,sizeof(appConfig->deviceid));
  memset(appConfig->devicelicence,0,sizeof(appConfig->devicelicence));
  mico_system_context_update(g_mico_context);
}

/* Application Entrance */
OSStatus application_start( void *arg )
{
  OSStatus err = kNoErr;
  int ret = -1;
  /* Create application context */
  g_app_context = ( app_context_t *)calloc(1, sizeof(app_context_t) );
  require_action( g_app_context, exit, err = kNoMemoryErr );

  // 从flash中获取系统信息 
  g_mico_context = mico_system_context_init( sizeof( application_config_t) );
  require_action( g_mico_context, exit, err = kNoMemoryErr );
  
  //获取应用信息
  g_app_context->appConfig = mico_system_context_get_user_data( g_mico_context );
  
  /* mico system initialize */
  err = mico_system_init( g_mico_context );
  require_noerr( err, exit );
  
  /* rgb led status initialize */
#ifdef USE_MiCOKit_EXT
  hsb2rgb_led_init();
#endif  
  
  /*系统初始化会读取flash配置信息，如有数据无需配网。
  如没有数据需要手机配合设备入网*/
  /* Wait for wlan connection*/
  /* 检查链路状态*/
  check_linkstatus();
  mico_main_log( "wifi connected successful" );

  start_weixin_auth();//微信认证流程，获取deviceid和devicelicense
  mico_rtos_get_semaphore(&g_app_context->appConfig->weixin_auth_sem,MICO_NEVER_TIMEOUT);
  
  mico_main_log( "deviceid and devicelicense is ready.............." );
  /* start wechat direct connection */
  ret = wechat_direct_connect();
  require_action((0 == ret), exit, mico_main_log("start wechat connect failed! ret=%d.", ret));

exit:
  mico_rtos_delete_thread(NULL);
  return err;
}



void check_linkstatus(void)
{
  int i=0;
  do
  {
    LinkStatusTypeDef link_status={0};
    micoWlanGetLinkStatus(&link_status);
    msleep(1000);
    i++;
    //检查连接是否成功
    if (link_status.is_connected == 1 && i > 3)
      break;
  }while(1);
}
