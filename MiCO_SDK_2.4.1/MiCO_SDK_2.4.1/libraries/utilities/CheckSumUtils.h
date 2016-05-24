/**
  ******************************************************************************
  * @file    CheckSumUtils.h 
  * @author  William Xu
  * @version V1.0.0
  * @date    05-May-2014
  * @brief   This header contains function prototypes which aid in checksum calculations.
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


#ifndef __CheckSumUtils_h__
#define __CheckSumUtils_h__

#include "Common.h"



/*********************  CRC8 MiCO Check **************************************

******************CRC-8 XMODEM       x8+x5+x4+1******************************

*******************************************************************************/

typedef struct
{
  uint8_t crc;
} CRC8_Context;

/**
 * @brief  CRC8_Context init.
 * @param  inContext    CRC8_Context   
 * @retval              None 
 */
void CRC8_Init( CRC8_Context *inContext );


/**
 * @brief  CRC8 Cyclic Redundancy Check Value update.
 *
 * @param  inContext    CRC8_Context    
 * @param  inSrc        buffer to hold input data.  
 * @param  inLen        size of input data. 
 * @retval              None 
 */
void CRC8_Update( CRC8_Context *inContext, const void *inSrc, size_t inLen );


/**
 * @brief  CRC8 Cyclic Redundancy Check Value update.
 *
 * @param  inContext    CRC8_Context    
 * @param  ioutResult   buffer to hold CRC8 Cyclic Redundancy Check Value.  
 * @retval              None 
 */
void CRC8_Final( CRC8_Context *inContext, uint8_t *outResult );





/*********************  CRC16 MiCO Check  **************************************

******************CRC-16/XMODEM       x16+x12+x5+1******************************

*******************************************************************************/


typedef struct
{
  uint16_t crc;
} CRC16_Context;
/**
 * @brief  CRC16_Context init.
 * @param  inContext    CRC16_Context   
 * @retval              None 
 */
void CRC16_Init( CRC16_Context *inContext );


/**
 * @brief  CRC16 Cyclic Redundancy Check Value update.
 *
 * @param  inContext    CRC16_Context    
 * @param  inSrc        buffer to hold input data.  
 * @param  inLen        size of input data. 
 * @retval              None 
 */
void CRC16_Update( CRC16_Context *inContext, const void *inSrc, size_t inLen );


/**
 * @brief  CRC16 Cyclic Redundancy Check Value update.
 *
 * @param  inContext    CRC16_Context    
 * @param  ioutResult   buffer to hold CRC16 Cyclic Redundancy Check Value.  
 * @retval              None 
 */
void CRC16_Final( CRC16_Context*inContext, uint16_t *outResult );


#endif //__CheckSumUtils_h__


