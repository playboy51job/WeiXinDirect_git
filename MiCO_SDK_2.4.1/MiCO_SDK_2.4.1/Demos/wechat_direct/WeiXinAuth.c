/**
******************************************************************************
* @file    WeiXinAuth.c 
* @author  Jacky Zhang
* @version V1.0.0
* @date    21-May-2015
* @brief   https request to get access to WeiXin Auth 
******************************************************************************
*
*  The MIT License
*  Copyright (c) 2014 MXCHIP Inc.
*
*  Permission is hereby granted, free of charge, to any person obtaining a 
copy 
*  of this software and associated documentation files (the "Software"), to 
deal
*  in the Software without restriction, including without limitation the 
rights 
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is 
furnished
*  to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY,
*  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
OR 
*  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.
******************************************************************************
*/

#include "WeiXinAuth.h"



#define WeiXinAuth_log(M, ...) custom_log("WeiXinAuth", M, ##__VA_ARGS__)
     
 //data struct
typedef struct _http_response_data{
        char *content;
        int content_length;
} http_response_data;

typedef enum
{ 
  WX_GETTOKEN,
  WX_GETRQCODE,
  WX_POSTAUTH,
  WX_EXIT,	
}SEQREQ;

//global variables
extern mico_Context_t* g_mico_context; 
extern app_context_t* g_app_context;
static http_response_data g_response = { NULL, 0 };//���Դ���һ����ַ��ȥ���շ�������
//function prototype
char *parseJSONData(char *jsondata,int num,char *token,char *deviceid,char *devicelicence);
int start_weixin_auth(void);//��ʼ΢����֤
void weixin_https_client_thread(void *arg);
void weixin_https_request();//��ִ��3��request
void updatetoken2NVRAM(char *token);
void weixin_formatMACAddr(char *destAddr, char *srcAddr);
int checkNVRAM(char *token,char *deviceid,char *devicelicence);


//���շ���������
static OSStatus onReceivedData(struct _HTTPHeader_t * httpHeader, 
                               uint32_t pos, 
                               uint8_t *data, 
                               size_t len,
                               void * userContext);
//����ṹ������
static void onClearData( struct _HTTPHeader_t * inHeader, void * inUserContext );


int start_weixin_auth(void)
{
  OSStatus err = kNoErr;
  mico_rtos_init_semaphore(&g_app_context->appConfig->weixin_auth_sem,1);
  /*https����ssl������Ҫռ�ô����ڴ棬������Ҫ���⿪һ���̲߳��ҷ������̶߳�ջ�ռ�
  application_start��ջ̫С(��mico_config.c����)������ssl��overflow*/
  err = mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "https_client", weixin_https_client_thread, 0x3000, (void*)NULL );
  require_noerr_action( err, exit, WeiXinAuth_log("ERROR: Unable to start the https client thread.") );
exit:
  return err;
}

void weixin_https_client_thread(void *arg)
{
  UNUSED_PARAMETER(arg);
  weixin_https_request();//����������ϣ�ɾ���߳�
  //�ͷ��ź�����΢����֤���
  mico_rtos_set_semaphore(&g_app_context->appConfig->weixin_auth_sem);
  mico_rtos_deinit_semaphore(&g_app_context->appConfig->weixin_auth_sem);
  mico_rtos_delete_thread(NULL);
}

//ִ��3������ָ��
void weixin_https_request()
{
  OSStatus err=kNoErr;
  int client_fd = -1;
  int ssl_errno = 0;
  mico_ssl_t client_ssl = NULL;
  fd_set readfds;
  char ipstr[16];
  struct sockaddr_t addr;
  HTTPHeader_t *httpHeader = NULL;
  char format_query[800]={0};//С���ڴ�й©
  char weixinapi_token[200]={0};
  char deviceid[50]={0};
  char devicelicence[300]={0};
  int seq=WX_GETTOKEN;
  
  //DNS����������������������ip��ַ
  while(1){
    err = gethostbyname(WEIXIN_IOT_HOST, (uint8_t *)ipstr, 16);//�п��ܻ������Ҫ��ν���
    WeiXinAuth_log( "gethostbyname" );
    sleep(1);
    if(err==kNoErr)  break;
  }
  
ssl_retry:
  //HTTPHeaderCreateWithCallback set some callback functions������һ����ַ��ȥ 
  httpHeader = HTTPHeaderCreateWithCallback( 1024, onReceivedData, onClearData, &g_response );
  require_action( httpHeader, exit, err = kNoMemoryErr );
  
  client_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
  addr.s_ip = inet_addr( ipstr );
  addr.s_port = 443;//https�˿�
  err = connect( client_fd, &addr, sizeof(addr) ); 
  require_noerr_string( err, exit, "connect https server failed" );
  
  ssl_version_set(TLS_V1_2_MODE);//΢��ssl������Э��汾
  client_ssl = ssl_connect( client_fd, 0, NULL, &ssl_errno );
  require_string( client_ssl != NULL, exit, "ERROR: ssl disconnect" );
  FD_ZERO( &readfds );
  FD_SET( client_fd, &readfds );
  
  WeiXinAuth_log( "ssl_connect successfully" );
  seq=WX_GETTOKEN;
  while(1)//��Ҫ����������˳�
  {
    HTTPHeaderClear( httpHeader );//�������
    memset(format_query,0,sizeof(format_query));//�������
    sleep(2);//�ȴ��ͷ�
    if(seq == WX_GETTOKEN)
    {
      sprintf(format_query,WEIXIN_IOT_GETTOKEN_REQUEST,WEIXIN_appid,WEIXIN_secret);
    }
    else if(seq == WX_GETRQCODE)
    {
      if(checkNVRAM(weixinapi_token,deviceid,devicelicence)==1){
        WeiXinAuth_log("checkNVRAM deviceid=%s",deviceid);
	    WeiXinAuth_log("checkNVRAM devicelicence=%s",devicelicence);
        seq = WX_POSTAUTH;  continue;
      }
      sprintf(format_query,WEIXIN_IOT_GETQRCODE_REQUEST,weixinapi_token,WEIXIN_product_id);
      
    }
    else if(seq == WX_POSTAUTH)
    {
      	  char *postdata=(char*)calloc(1,500);
		  char mac[18]={0};
		  weixin_formatMACAddr(mac,g_mico_context->micoStatus.mac);
	      sprintf(postdata,WEIXIN_IOT_POSTAUTH_DATA,deviceid,mac);
          sprintf(format_query,WEIXIN_IOT_POSTAUTH_REQUEST,weixinapi_token,strlen(postdata));
	      strcat(format_query,postdata);
          WeiXinAuth_log("WX_GETAUTH format_query=%s",format_query);
	      free(postdata);
    }
    else if(seq == WX_EXIT )
    {
      WeiXinAuth_log("wx_exit now");
      goto exit;	
    }
    //����������443�˿ڷ���HTTP Request 
    ssl_send( client_ssl, format_query, strlen(format_query) );
    select( client_fd + 1, &readfds, NULL, NULL, NULL );//���ȵķ�ʽ
    WeiXinAuth_log("waiting..........");
    if( FD_ISSET( client_fd, &readfds ) ){
      err = SocketReadHTTPSHeader( client_ssl, httpHeader );
      switch ( err ){
      case kNoErr:
        PrintHTTPHeader( httpHeader );
        err = SocketReadHTTPSBody( client_ssl, httpHeader );//get body data
        require_noerr( err, exit );
        //get data and print
        parseJSONData(g_response.content,seq,weixinapi_token,deviceid,devicelicence);
        seq++;
        break;
      case EWOULDBLOCK:
      case kNoSpaceErr:
      case kConnectionErr:
      default:
          WeiXinAuth_log("ERROR: HTTP Header parse error: %d", err);
          if( client_ssl ){
               ssl_close( client_ssl );
               client_ssl=NULL;
          }
          SocketClose( &client_fd );
          client_fd=-1;
          HTTPHeaderDestory( &httpHeader );
          WeiXinAuth_log("retry after 2s......");
          sleep(2);// retry after 2s
          goto ssl_retry;
      }
    }
  }
exit:
  WeiXinAuth_log( "Exit: Client exit with err = %d, fd:%d", err, client_fd );
  if( client_ssl )  ssl_close( client_ssl );
  SocketClose( &client_fd );
  HTTPHeaderDestory( &httpHeader );
  if(err!=kNoErr)
  {
    goto ssl_retry;//ȷ���ɹ�
  }
}
//D0:BA:E4:07:61:D4
//skip :
static void weixin_formatMACAddr(char *destAddr, char *srcAddr)
{
  sprintf((char *)destAddr, "%c%c%c%c%c%c%c%c%c%c%c%c",\
                    toupper(*(char *)srcAddr),toupper(*((char *)(srcAddr)+1)),\
                    toupper(*((char *)(srcAddr)+3)),toupper(*((char *)(srcAddr)+4)),\
                    toupper(*((char *)(srcAddr)+6)),toupper(*((char *)(srcAddr)+7)),\
                    toupper(*((char *)(srcAddr)+9)),toupper(*((char *)(srcAddr)+10)),\
                    toupper(*((char *)(srcAddr)+12)),toupper(*((char *)(srcAddr)+13)),\
                    toupper(*((char *)(srcAddr)+15)),toupper(*((char *)(srcAddr)+16)));
}


void updatetoken2NVRAM(char *token)
{
  memset(g_app_context->appConfig->weixinapi_token,0,
         sizeof(g_app_context->appConfig->weixinapi_token));
  strcpy(g_app_context->appConfig->weixinapi_token,token);
  mico_system_context_update( g_mico_context);
}
void updateQRCode2NVRAM(char *deviceid,char *devicelicence)
{
  memset(g_app_context->appConfig->deviceid,0,
         sizeof(g_app_context->appConfig->deviceid));
  memset(g_app_context->appConfig->devicelicence,0,
         sizeof(g_app_context->appConfig->devicelicence));
  
  strcpy(g_app_context->appConfig->deviceid,deviceid);
  strcpy(g_app_context->appConfig->devicelicence,devicelicence);
  g_app_context->appConfig->store=100;//�Զ���100��ʾ������
  mico_system_context_update( g_mico_context);
}
//0 ��ʾû������,1��ʾ������
int checkNVRAM(char *token,char *deviceid,char *devicelicence)
{ 
  if(g_app_context->appConfig->store == 100) {//100Ϊ�Զ���������
    strcpy(token,g_app_context->appConfig->weixinapi_token);
    strcpy(deviceid,g_app_context->appConfig->deviceid);
    strcpy(devicelicence,g_app_context->appConfig->devicelicence);
    return 1;
  }
  else //û������
    return 0;
}

char *parseJSONData(char *jsondata,int num,char *token,char *deviceid,char *devicelicence)
{
  if(jsondata==NULL) return NULL;
  WeiXinAuth_log( "parseJSONData=%s",jsondata );
  json_object *object= json_tokener_parse(jsondata);//�ַ���ת����
  
  if(num==WX_GETTOKEN)
  {
    json_object_object_foreach(object, key, val) {
      if(!strcmp(key, "access_token")){
        strcpy(token,json_object_get_string(val));
        break;
      }
    }
     updatetoken2NVRAM(token);
  }
  else if(num == WX_GETRQCODE)
  {
    json_object_object_foreach(object, key, val) {
      if(!strcmp(key, "deviceid")){
        strcpy(deviceid,json_object_get_string(val));
      }
      if(!strcmp(key, "devicelicence")){
        strcpy(devicelicence,json_object_get_string(val));
      }
    }
    
    updateQRCode2NVRAM(deviceid,devicelicence);
    WeiXinAuth_log("store deviceid=%s",deviceid);
    WeiXinAuth_log("store devicelicence=%s",devicelicence);
  }
  else if(num == WX_POSTAUTH)
  {
  	//nothing to do
  }
  
  //parse finished,free memory
  json_object_put(object);//free memory 
  object=NULL;
  return NULL;
}



/* һ������������ܻ��յ�������ݷ���*/
static OSStatus onReceivedData( struct _HTTPHeader_t * inHeader, uint32_t inPos, uint8_t * inData, size_t inLen, void * inUserContext )
{
  OSStatus err = kNoErr;
  http_response_data *context = inUserContext;/*��ȡ�������Ķ�ջָ��*/
  if( inPos == 0 )
  {
  //��һ��callback��ʼ�����ڴ�
    context->content = malloc( inHeader->contentLength + 1);
    memset(context->content, 0x0, inHeader->contentLength + 1);
    require_action( context->content, exit, err = kNoMemoryErr );
    context->content_length = inHeader->contentLength;
  }
  memcpy( context->content + inPos, inData, inLen );
exit:
  return err;
}

/* Called when HTTPHeaderClear is called */
static void onClearData( struct _HTTPHeader_t * inHeader, void * inUserContext )
{
  UNUSED_PARAMETER( inHeader );
  //���ݱ���
  http_response_data *context = inUserContext;
  if( context->content ) {
    free( context->content );
    context->content = NULL;
  }
}



