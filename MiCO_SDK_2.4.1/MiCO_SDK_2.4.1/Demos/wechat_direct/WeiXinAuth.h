/**
  ******************************************************************************
  * @file    WeiXinAuth.h 
  * @author  Jacky Zhang
  * @version V1.0.0
  * @date    02-Mar-2016
  * @brief   WeiXinAuth data.
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
#include "HTTPUtils.h"
#include "SocketUtils.h"
#include "StringUtils.h"
#include "json.h"

#include "user_config.h"


#define WEXIN_original_ID  "gh_420af5d2de71"  // MiCO总动员微信公众号 original ID
#define WEIXIN_appid "wxa7fa266a6cb79b5b"
#define WEIXIN_secret "df91eb0c03b6aeb4cd7c8f218470aca7"
#define WEIXIN_product_id "6415"

/*weixin服务器支持https*/
#define WEIXIN_IOT_HOST "api.weixin.qq.com"
//get token
#define WEIXIN_IOT_GETTOKEN_REQUEST \
"GET https://api.weixin.qq.com/cgi-bin/token?grant_type=client_credential&appid=%s&secret=%s HTTP/1.1\r\n" \
"Host:api.weixin.qq.com\r\n" \
"\r\n"
      
//get qrcode(deviceid and devicelicense)
#define WEIXIN_IOT_GETQRCODE_REQUEST \
"GET https://api.weixin.qq.com/device/getqrcode?access_token=%s&product_id=%s HTTP/1.1\r\n" \
"Host:api.weixin.qq.com\r\n" \
"\r\n"
   
//post data for auth
#define WEIXIN_IOT_POSTAUTH_REQUEST \
"POST https://api.weixin.qq.com/device/authorize_device?access_token=%s HTTP/1.1\r\n" \
"Host:api.weixin.qq.com\r\n"\
"Content-Length:%d\r\n"\
"\r\n"  


//mac:D0BAE41761D4,每一行末尾的\表示连接符，其他加\表示转义，         
#define WEIXIN_IOT_POSTAUTH_DATA "{\
\"device_num\":\"1\",\
\"device_list\":[\
{\
\"id\":\"%s\",\
\"mac\":\"%s\",\
\"connect_protocol\":\"4\",\
\"auth_key\":\"\",\
\"close_strategy\":\"1\",\
\"conn_strategy\":\"1\",\
\"crypt_method\":\"0\",\
\"auth_ver\":\"0\",\
\"manu_mac_pos':\"-1\",\
\"ser_mac_pos\":\"-2\",\
\"ble_simple_protocol\":\"0\"\
}\
],\
\"op_type\":\"1\"\
}"

int start_weixin_auth(void);
