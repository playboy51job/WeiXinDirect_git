/**
  ******************************************************************************
  * @file    airkiss_porting.c 
  * @author  Eshen Wang
  * @version V1.0.0
  * @date    02-Mar-2016
  * @brief   Implementions of portable functions for wechat direct sdk 
  *          base on MiCO platform.
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
#include "airkiss_porting.h"
#include <stdarg.h>

#define airkiss_porting_log(format, ...)  custom_log("airkiss_porting", format, ##__VA_ARGS__)

#ifdef AIRKISS_ENABLE_LOG
int airkiss_printfImp(const char *fmt, ...)
{
  va_list ap;
  int ret = 0;
  
  va_start(ap, fmt);
  ret = vprintf(fmt, ap);
  va_end(ap);
                                        
  return ret;
}
#endif

#ifdef AIRKISS_SUPPORT_MULTITHREAD
struct ak_mutex_t{
  mico_mutex_t m_mutex;
};

ak_mutex_t m_task_mutex;
ak_mutex_t m_malloc_mutex;

int airkiss_mutex_create(ak_mutex_t *mutex_ptr)
{
  OSStatus err = kNoErr;
  
  if(NULL == mutex_ptr){
    airkiss_porting_log("ERROR: mutex_ptr null!");
    return -1;
  }
  
  err = mico_rtos_init_mutex(&(mutex_ptr->m_mutex));
  if(kNoErr != err){
    airkiss_porting_log("ERROR: create airkiss mutex err=%d.", err);
    return -1;
  }
  else{
    airkiss_porting_log("INFO: create airkiss mutex success.");
    return 0;
  }
}

int airkiss_mutex_lock(ak_mutex_t *mutex_ptr)
{
  OSStatus err = kNoErr;
  
  if(NULL == mutex_ptr){
    airkiss_porting_log("ERROR: mutex_ptr null!");
    return -1;
  }
    
  err = mico_rtos_lock_mutex(&(mutex_ptr->m_mutex));
  if(kNoErr != err){
    airkiss_porting_log("ERROR: lock airkiss mutex err=%d.", err);
    return -1;
  }
  else{
    return 0;
  }
}

int airkiss_mutex_unlock(ak_mutex_t *mutex_ptr)
{
  OSStatus err = kNoErr;
    
  if(NULL == mutex_ptr){
    airkiss_porting_log("ERROR: mutex_ptr null!");
    return -1;
  }
  
  err = mico_rtos_unlock_mutex(&(mutex_ptr->m_mutex));
  if(kNoErr != err){
    airkiss_porting_log("ERROR: unlock airkiss mutex err=%d.", err);
    return -1;
  }
  else{
    return 0;
  }
}

int airkiss_mutex_delete(ak_mutex_t *mutex_ptr)
{
  OSStatus err = kNoErr;
    
  if(NULL == mutex_ptr){
    airkiss_porting_log("ERROR: mutex_ptr null!");
    return -1;
  }
  
  err = mico_rtos_deinit_mutex(&(mutex_ptr->m_mutex));
  if(kNoErr != err){
    airkiss_porting_log("ERROR: delete airkiss mutex err=%d.", err);
    return -1;
  }
  else{
    return 0;
  }
}
#endif

int airkiss_dns_gethost(char* url, uint32_t* ipaddr)
{
  int8_t ret = -1;
  uint8_t ip[16] = {0};
  
  if((NULL == url) || (NULL == ipaddr)){
    airkiss_porting_log("ERROR: airkiss_dns_gethost: invalid params!");
    return AK_DNS_FAILED;
  }
  
  ret = gethostbyname(url, ip, 16);
  if(0 != ret){
    airkiss_porting_log("ERROR: gethostbyname err=%d.", ret);
    return AK_DNS_FAILED;
  }
  else{ //got ipaddr 192.168.1.1 = (uint32_t)(192<<24)|(168<<16)|(1<<8)|(1)
    *ipaddr = inet_addr((char*)(&ip[0]));
    airkiss_porting_log("INFO: gethostbyname:[%s] => [%s] => %x.", url, ip, *ipaddr);
    return AK_DNS_SUCCESS;
  }
}

int airkiss_dns_checkstate(uint32_t* ipaddr)
{
  return AK_DNS_SUCCESS;
}

ak_socket airkiss_tcp_socket_create()
{
  ak_socket sock;
  sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock < 0) {
    return -1;
  }
  return sock;
}

int airkiss_tcp_connect(ak_socket sock, char* ipaddr, uint16_t port)
{
  uint32_t addr;
  int ret = -1;
  struct sockaddr_t sock_addr;
        
  addr = inet_addr(ipaddr);
  if(-1 == addr) {
    return AK_TCP_CONNECT_FAILED;
  }
  
  sock_addr.s_ip = addr;
  sock_addr.s_port = port;
  ret = connect(sock, &sock_addr, sizeof(struct sockaddr_t));
  if (ret < 0) {
    airkiss_porting_log("ERROR: Failed to connect socket %d.", sock);
    return AK_TCP_CONNECT_FAILED;
  }
  return AK_TCP_CONNECT_SUCCESS;
}

int airkiss_tcp_checkstate(ak_socket sock)
{
  return AK_TCP_CONNECT_SUCCESS;
}

int airkiss_tcp_send(ak_socket socket, char*buf, uint32_t len)
{
  return send(socket, buf, len, 0);
}

void airkiss_tcp_disconnect(ak_socket socket)
{
  close(socket);
}

int airkiss_tcp_recv(ak_socket socket, char *buf, uint32_t size, uint32_t timeout)
{
  int recvBytes;
  fd_set sockSet;
  int fdAct = 0;
  struct timeval_t tmo;
  FD_ZERO(&sockSet);
  FD_SET(socket, &sockSet);
  tmo.tv_sec = timeout/1000;
  timeout = timeout%1000;
  tmo.tv_usec = timeout*1000;
  
  if(NULL == buf){
    airkiss_porting_log("ERROR: input buf null!");
    return -1;
  }
  
  fdAct = select(socket + 1, &sockSet, NULL, NULL, &tmo);
  if (0 < fdAct) {
    airkiss_porting_log("INFO: fdAct is not 0:%d.", fdAct);
    if (FD_ISSET(socket, &sockSet)) {
      recvBytes = recv(socket, buf, size, 0);
      if (recvBytes < 0) {
        return -1;
      } else {
        return recvBytes;
      }
    }
  }
  return 0;
}

uint32_t airkiss_gettime_ms()
{
  return mico_get_time();
}
