/**
******************************************************************************
* @file    rtos.c
* @author  William Xu
* @version V1.0.0
* @date    05-May-2014
* @brief   Definitions of the MiCO RTOS abstraction layer for the special case
*          of having no RTOS
******************************************************************************
*
*  The MIT License
*  Copyright (c) 2016 MXCHIP Inc.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy 
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights 
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is furnished
*  to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
*  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************
*/ 

#include "common.h"
#include "platform_peripheral.h"


#define DISABLE_INTERRUPTS() do { __asm("CPSID i"); } while (0)

#define ENABLE_INTERRUPTS() do { __asm("CPSIE i"); } while (0)

typedef volatile struct _noos_semaphore_t
{
  uint8_t count;
} noos_semaphore_t;


OSStatus mico_rtos_init_semaphore( mico_semaphore_t* semaphore, int count )
{
    noos_semaphore_t *noos_semaphore;
    UNUSED_PARAMETER( count );
    noos_semaphore = malloc(sizeof(noos_semaphore_t));
    noos_semaphore->count = 0;
    *semaphore = (void *)noos_semaphore;
    return kNoErr;
}

OSStatus mico_rtos_get_semaphore( mico_semaphore_t* semaphore, uint32_t timeout_ms )
{
    noos_semaphore_t *noos_semaphore = (noos_semaphore_t *)*semaphore;
    int delay_start;

    if( noos_semaphore == NULL)
        return kNotInitializedErr;

    delay_start = mico_get_time(); 
    while( noos_semaphore->count == 0){
      if(mico_get_time() >= delay_start + timeout_ms && timeout_ms != MICO_NEVER_TIMEOUT){
        return kTimeoutErr;
      }
    }

    DISABLE_INTERRUPTS();
    noos_semaphore->count--;
    ENABLE_INTERRUPTS();

    return kNoErr;
}

OSStatus mico_rtos_set_semaphore( mico_semaphore_t* semaphore )
{
    noos_semaphore_t *noos_semaphore = (noos_semaphore_t *)*semaphore;

    if( noos_semaphore == NULL)
        return kNotInitializedErr;

    DISABLE_INTERRUPTS();
    noos_semaphore->count++;
    ENABLE_INTERRUPTS();

    return kNoErr;
}

OSStatus mico_rtos_deinit_semaphore( mico_semaphore_t* semaphore )
{
    noos_semaphore_t *noos_semaphore = (noos_semaphore_t *)*semaphore;

    if( noos_semaphore == NULL)
        return kNotInitializedErr;

    free((void *)noos_semaphore);

    return kNoErr;
}


OSStatus mico_rtos_init_mutex( mico_mutex_t* mutex )
{
    UNUSED_PARAMETER(mutex);
    return kNoErr;
}


OSStatus mico_rtos_lock_mutex( mico_mutex_t* mutex )
{
    UNUSED_PARAMETER(mutex);
    return kNoErr;
}

OSStatus mico_rtos_unlock_mutex( mico_mutex_t* mutex )
{
    UNUSED_PARAMETER(mutex);
    return kNoErr;

}

OSStatus mico_rtos_deinit_mutex( mico_mutex_t* mutex )
{
    UNUSED_PARAMETER(mutex);
    return kNoErr;    
}

extern uint32_t mico_get_time_no_os(void);
uint32_t mico_get_time(void)
{
  return mico_get_time_no_os( );
}

void mico_thread_msleep(uint32_t milliseconds)
{
  int tick_delay_start = mico_get_time();
  while(mico_get_time() < tick_delay_start+milliseconds);
}

