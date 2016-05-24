/**
  ******************************************************************************
  * @file    airkiss_cloud.c 
  * @author  Eshen Wang
  * @version V1.0.0
  * @date    02-Mar-2016
  * @brief   Airkiss cloud demo.
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
#include "json.h"
#include "airkiss_cloudapi.h"
//#include "airkiss_porting.h"
#include "airkiss_cloud.h"

#include "WeiXinAuth.h"
#include "easylink/airkiss_discovery.h"

#ifdef USE_MiCOKit_EXT
#include "MiCOKit_EXT/micokit_ext.h"
#endif

#define WECHAT_ENTRY_THREAD_STACK_SIZE  (2 * 1024 )
#define WECHAT_STATUS_REPORT_THREAD_STACK_SIZE  (2 * 1024 )

#define airkiss_cloud_log(format, ...)  custom_log("airkiss_cloud", format, ##__VA_ARGS__)

extern app_context_t* g_app_context;

static mico_semaphore_t g_msg_send_sem = NULL;
static volatile int64_t g_recv_msg_id = 0;
static uint8_t g_recv_msg_type[8] = {0};
static uint8_t m_statusCBmsg[512] = {0};//状态反馈
uint32_t heapbuf[2*1024];//10k for airkiss heap
uint32_t g_funcid = 0;

void ReceiveResponseCB(uint32_t taskid, uint32_t errcode, uint32_t funcid, const uint8_t* body, uint32_t bodylen) {
}

//通知，接收微信消息
void ReceiveNotifyCB(uint32_t funcid, const uint8_t* body, uint32_t bodylen) {
  
  json_object *recv_json_object = NULL;
  json_object *item_obj = NULL;
  json_object *item_obj_msg_id = NULL;
  json_object *item_obj_msg_type = NULL;
  
  g_funcid = funcid;
  //字符串转json对象
  airkiss_cloud_log("recv body:%s",body);
  recv_json_object = json_tokener_parse((const char*)body);
  
  if (NULL != recv_json_object){
    // get msg_id for ack
    item_obj_msg_id = json_object_object_get(recv_json_object, "msg_id");
    g_recv_msg_id = json_object_get_int64(item_obj_msg_id); 
    airkiss_cloud_log("get msg id: %lld.", g_recv_msg_id);
    
    // get msg type,eg:"set"
    item_obj_msg_type = json_object_object_get(recv_json_object, "msg_type");
    memset(g_recv_msg_type, 0, sizeof(g_recv_msg_type));
    strncpy((char*)g_recv_msg_type, json_object_get_string(item_obj_msg_type),8); // get msg_type for ack
    airkiss_cloud_log("get msg type: %s.", g_recv_msg_type);
    
    //get switch,此处嵌套数据类型比较复杂
    item_obj = json_object_object_get(recv_json_object, "services");
    if(NULL != item_obj){
      item_obj = json_object_object_get(item_obj, "outlet");
      if(NULL != item_obj){
        g_app_context->appConfig->power_switch = 
          json_object_get_boolean(json_object_array_get_idx(json_object_object_get(item_obj, "port_on_off"),0));
        
        if(g_app_context->appConfig->power_switch){
          hsb2rgb_led_open(120, 100, 50);//变绿色灯
        }
        else{
          hsb2rgb_led_close();//关灯
        }
        mico_rtos_set_semaphore( &g_msg_send_sem);//ack应答，通知面板，告知状态
      }
    }
    // free memory of json object
    json_object_put(recv_json_object);
  }
}
//通知，告知设备登录状态
void ReceiveEventCB(EventValue event_value) {
  switch (event_value) {
  case EVENT_VALUE_LOGIN:
    airkiss_cloud_log("Device Login!");
    break;
  case EVENT_VALUE_LOGOUT:
    airkiss_cloud_log("Device Logout!");
    break;
  default:
    airkiss_cloud_log("Unknown event!");
    break;
  }
}



#ifdef AIRKISS_SUPPORT_MULTITHREAD
extern ak_mutex_t m_task_mutex;
extern ak_mutex_t m_malloc_mutex;//define in airkiss_porting_4004.c

void airkiss_status_report_thread(void *arg) {
        uint32_t taskid = 0;
#ifdef AIRKISS_SUPPORT_MULTITHREAD
	for (;;) {
                mico_rtos_get_semaphore( &g_msg_send_sem, MICO_WAIT_FOREVER );
                airkiss_cloud_log( "set ok, ack && report current status." );
                memset(m_statusCBmsg, 0, sizeof(m_statusCBmsg));
                if(g_app_context->appConfig->power_switch){
                  sprintf((char*)m_statusCBmsg, "{\"asy_error_code\": 0,\"asy_error_msg\": \"ok\",\"msg_id\":%lld,\"msg_type\":\"%s\",\"services\":{\"operation_status\":{\"status\":0},\"outlet\":{\"port_on_off\":[true,false],\"port_in_use\":[true,false]}}}", g_recv_msg_id, g_recv_msg_type);
                }
                else{
                  sprintf((char*)m_statusCBmsg, "{\"asy_error_code\": 0,\"asy_error_msg\": \"ok\",\"msg_id\":%lld,\"msg_type\":\"%s\",\"services\":{\"operation_status\":{\"status\":0},\"outlet\":{\"port_on_off\":[false,false],\"port_in_use\":[true,false]}}}", g_recv_msg_id, g_recv_msg_type);
                }
                airkiss_cloud_log("send status msg: %s", m_statusCBmsg);
		taskid = airkiss_cloud_sendmessage(g_funcid, (uint8_t *)m_statusCBmsg, strlen((const char *)m_statusCBmsg));
	}
#else
	taskid = airkiss_cloud_sendmessage(g_funcid, (uint8_t *)m_statusCBmsg, strlen((const char *)m_statusCBmsg));
    for (;;) {
    	airkiss_cloud_loop();
        mico_thread_msleep(100);
    }
#endif
}


#endif



void task_execute_sdk_runloop() {        
#ifdef AIRKISS_SUPPORT_MULTITHREAD
	while (0 != airkiss_cloud_init((uint8_t *)g_app_context->appConfig->devicelicence, (uint32_t)strlen((const char *)g_app_context->appConfig->devicelicence),
                                       &m_task_mutex, &m_malloc_mutex, heapbuf, sizeof(heapbuf))){
          airkiss_cloud_log("SDK init failed!!!");
          mico_thread_msleep(1000);
	}
#else
	while (0 != airkiss_cloud_init((uint8_t *)g_app_context->appConfig->devicelicence, (uint32_t)strlen((const char *)g_app_context->appConfig->devicelicence),
                                       0, 0, heapbuf, sizeof(heapbuf))){
		airkiss_cloud_log("SDK init failed!!!");
		mico_thread_msleep(1000);
	}
#endif

	//register callback functions
	airkiss_callbacks_t cbs;
	cbs.m_notifycb = ReceiveNotifyCB;
	cbs.m_respcb = ReceiveResponseCB;
	cbs.m_eventcb = ReceiveEventCB;
	airkiss_regist_callbacks(&cbs);
        
    //消息机制，类似iOS runloop
    uint32_t sleep_time;
	airkiss_cloud_log("Everything is ready!!");
	for (;;) {
		sleep_time = airkiss_cloud_loop();
		mico_thread_msleep(sleep_time);
	}
}
        

static void airkiss_cloud_entry(void *arg)
{
    airkiss_cloud_log("Airkiss lib version:%s", airkiss_cloud_version());
    task_execute_sdk_runloop();
    /* Never returns */
}


/* Wechat direct sdk demo entrance */
int wechat_direct_connect( void )
{
  int ret = -1;
  OSStatus err = kNoErr;
  //已确保微信数据有用
  //开启局域网发现协议
  err = airkiss_discovery_start( WEXIN_original_ID, g_app_context->appConfig->deviceid);
  require_noerr( err, exit );
  

  err = mico_rtos_init_semaphore(&g_msg_send_sem, 1);
  require_noerr( err, exit ); 
  
  /* start wechat direct demo task */
  err = mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "WechatEntry", 
                                airkiss_cloud_entry, WECHAT_ENTRY_THREAD_STACK_SIZE, (void*)NULL );
  require_noerr_action( err, exit, airkiss_cloud_log("ERROR: Unable to start WechatApp thread.") );
  
  //全局宏定义
  
#ifdef AIRKISS_SUPPORT_MULTITHREAD
  err = mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "WechatCloud", 
                                airkiss_status_report_thread, WECHAT_STATUS_REPORT_THREAD_STACK_SIZE, (void*)NULL );
  require_noerr_action( err, exit, airkiss_cloud_log("ERROR: Unable to start WechatCloud thread.") );
#endif
  
  ret = 0;
exit:
  return ret;
}
