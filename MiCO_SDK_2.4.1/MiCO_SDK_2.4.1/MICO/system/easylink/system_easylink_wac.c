/**
******************************************************************************
* @file    mico_easylink_wac.c
* @author  William Xu
* @version V1.0.0
* @date    05-May-2014
* @brief   This file provide header file for start a Apple WAC (wireless accessory
*          configuration) function thread.
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



#include "mico.h"
#include "MFi_WAC.h"

#include "StringUtils.h"

#define BUNDLE_SEED_ID          "C6P64J2MZX"  
#define EA_PROTOCOL             "com.issc.datapath"

const char *eaProtocols[1] = {EA_PROTOCOL};

OSStatus system_easylink_wac_start( mico_Context_t * const inContext )
{
    OSStatus err = kNoErr;
    IPStatusTypedef para;
    uint8_t major_ver, minor_ver, revision;
    
    mfi_wac_lib_version( &major_ver, &minor_ver, &revision );
    system_log( "Import MFi WAC library v%d.%d.%d", major_ver, minor_ver, revision );

    WACPlatformParameters_t* WAC_Params = NULL;
    WAC_Params = calloc(1, sizeof(WACPlatformParameters_t));
    require(WAC_Params, exit);

    micoWlanGetIPStatus(&para, Station);

    str2hex((unsigned char *)para.mac, WAC_Params->macAddress, 6);
    WAC_Params->isUnconfigured          = 1;
    WAC_Params->supportsAirPlay         = 0;
    WAC_Params->supportsAirPrint        = 0;
    WAC_Params->supports2_4GHzWiFi      = 1;
    WAC_Params->supports5GHzWiFi        = 0;
    WAC_Params->supportsWakeOnWireless  = 0;

    WAC_Params->firmwareRevision =  FIRMWARE_REVISION;
    WAC_Params->hardwareRevision =  HARDWARE_REVISION;
    WAC_Params->serialNumber =      SERIAL_NUMBER;
    WAC_Params->name =              inContext->flashContentInRam.micoSystemConfig.name;
    WAC_Params->model =             MODEL;
    WAC_Params->manufacturer =      MANUFACTURER;

    WAC_Params->numEAProtocols =    1;
    WAC_Params->eaBundleSeedID =    BUNDLE_SEED_ID;
    WAC_Params->eaProtocols =       (char **)eaProtocols;

    err = mfi_wac_start( inContext, WAC_Params, MICO_I2C_CP, 1200 );
    require_noerr(err, exit);
    
exit:
    free(WAC_Params);
    return err; 
}

