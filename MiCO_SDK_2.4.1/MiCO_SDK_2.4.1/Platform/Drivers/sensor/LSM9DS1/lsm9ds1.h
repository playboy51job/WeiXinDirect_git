/**
******************************************************************************
* @file    uvis25.h
* @author  William Xu
* @version V1.0.0
* @date    21-May-2015
* @brief   temperature and humidity control demo.
******************************************************************************
*
*  The MIT License
*  Copyright (c) 2014 MXCHIP Inc.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LSM9DS1_H
#define __LSM9DS1_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "mico_platform.h"
#include "platform.h"
  
#ifndef LSM9DS1_I2C_PORT
  #define LSM9DS1_I2C_PORT      MICO_I2C_NONE
#endif

/************************************************/
/* 	Magnetometer Section  *******************/		 	  
/************************************************/
/* Magnetometer Sensor Full Scale */
#define LSM9DS1_MAG_FS_MASK		(0x60)
#define LSM9DS1_MAG_FS_4G		(0x00)	/* Full scale 4 Gauss */
#define LSM9DS1_MAG_FS_8G		(0x20)	/* Full scale 8 Gauss */
#define LSM9DS1_MAG_FS_12G		(0x40)	/* Full scale 10 Gauss */
#define LSM9DS1_MAG_FS_16G		(0x60)	/* Full scale 16 Gauss */

/* ODR */
#define ODR_MAG_MASK			(0X1C)	/* Mask for odr change on mag */
#define LSM9DS1_MAG_ODR0_625		(0x00)	/* 0.625Hz output data rate */
#define LSM9DS1_MAG_ODR1_25		(0x04)	/* 1.25Hz output data rate */
#define LSM9DS1_MAG_ODR2_5		(0x08)	/* 2.5Hz output data rate */
#define LSM9DS1_MAG_ODR5		(0x0C)	/* 5Hz output data rate */
#define LSM9DS1_MAG_ODR10		(0x10)	/* 10Hz output data rate */
#define LSM9DS1_MAG_ODR20		(0x14)	/* 20Hz output data rate */
#define LSM9DS1_MAG_ODR40		(0x18)	/* 40Hz output data rate */
#define LSM9DS1_MAG_ODR80		(0x1C)	/* 80Hz output data rate */

#define MAG_ENABLE_ON_INPUT_OPEN 	0
#define LSM9DS1_MAG_MIN_POLL_PERIOD_MS	13
#define LSM9DS1_INT_M_GPIO_DEF		(-1)
#define LSM9DS1_M_POLL_INTERVAL_DEF	(100)
  
/**********************************************/
/* 	Accelerometer section defines	 	*/
/**********************************************/
#define LSM9DS1_ACC_MIN_POLL_PERIOD_MS	1

/* Accelerometer Sensor Full Scale */
#define LSM9DS1_ACC_FS_MASK		(0x18)
#define LSM9DS1_ACC_FS_2G 		(0x00)	/* Full scale 2g */
#define LSM9DS1_ACC_FS_4G 		(0x08)	/* Full scale 4g */
#define LSM9DS1_ACC_FS_8G 		(0x10)	/* Full scale 8g */

/* Accelerometer Anti-Aliasing Filter */
#define LSM9DS1_ACC_BW_408		(0X00)
#define LSM9DS1_ACC_BW_211		(0X01)
#define LSM9DS1_ACC_BW_105		(0X02)
#define LSM9DS1_ACC_BW_50		(0X03)
#define LSM9DS1_ACC_BW_MASK		(0X03)

#define LSM9DS1_INT1_GPIO_DEF		(-1)
#define LSM9DS1_INT2_GPIO_DEF		(-1)

#define LSM9DS1_ACC_ODR_OFF		(0x00)
#define LSM9DS1_ACC_ODR_MASK		(0xE0)
#define LSM9DS1_ACC_ODR_10		(0x20)
#define LSM9DS1_ACC_ODR_50		(0x40)
#define LSM9DS1_ACC_ODR_119		(0x60)
#define LSM9DS1_ACC_ODR_238		(0x80)
#define LSM9DS1_ACC_ODR_476		(0xA0)
#define LSM9DS1_ACC_ODR_952		(0xC0)

/**********************************************/
/* 	Gyroscope section defines	 	*/
/**********************************************/
#define LSM9DS1_GYR_MIN_POLL_PERIOD_MS	1

#define LSM9DS1_GYR_FS_MASK		(0x18)
#define LSM9DS1_GYR_FS_245DPS		(0x00)
#define LSM9DS1_GYR_FS_500DPS		(0x08)
#define LSM9DS1_GYR_FS_2000DPS		(0x18)

#define LSM9DS1_GYR_ODR_OFF		(0x00)
#define LSM9DS1_GYR_ODR_MASK		(0xE0)
#define LSM9DS1_GYR_ODR_14_9		(0x20)
#define LSM9DS1_GYR_ODR_59_5		(0x40)
#define LSM9DS1_GYR_ODR_119		(0x60)
#define LSM9DS1_GYR_ODR_238		(0x80)
#define LSM9DS1_GYR_ODR_476		(0xA0)
#define LSM9DS1_GYR_ODR_952		(0xC0)

#define LSM9DS1_GYR_BW_0		(0x00)
#define LSM9DS1_GYR_BW_1		(0x01)
#define LSM9DS1_GYR_BW_2		(0x02)
#define LSM9DS1_GYR_BW_3		(0x03)

#define LSM9DS1_GYR_POLL_INTERVAL_DEF	(100)
#define LSM9DS1_ACC_POLL_INTERVAL_DEF	(100)

OSStatus lsm9ds1_mag_sensor_init(void);
OSStatus lsm9ds1_mag_read_data(int16_t *MAG_X, int16_t *MAG_Y, int16_t *MAG_Z);
OSStatus lsm9ds1_mag_sensor_deinit(void);

OSStatus lsm9ds1_acc_gyr_sensor_init(void);
OSStatus lsm9ds1_acc_read_data(int16_t *ACC_X, int16_t *ACC_Y, int16_t *ACC_Z);
OSStatus lsm9ds1_gyr_read_data(int16_t *GYR_X, int16_t *GYR_Y, int16_t *GYR_Z);
OSStatus lsm9ds1_acc_gyr_sensor_deinit(void);

#endif  /* __LSM9DS1_H */