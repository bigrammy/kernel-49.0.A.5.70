/*
 *
 * FocalTech TouchScreen driver.
 *
 * Copyright (c) 2010-2017, FocalTech Systems, Ltd., all rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
/************************************************************************
*
* File Name: focaltech_config.h
*
*    Author: Focaltech Driver Team
*
*   Created: 2016-08-08
*
*  Abstract: global configurations
*
*   Version: v1.0
*
************************************************************************/
#ifndef _LINUX_FOCLATECH_CONFIG_H_
#define _LINUX_FOCLATECH_CONFIG_H_

/**************************************************/
/****** G: A, I: B, S: C, U: D  ******************/
/****** chip type defines, do not modify *********/
#define _FT8716     0x87160805
#define _FT8736     0x87360806
#define _FT8006     0x80060807
#define _FT8606     0x86060808
#define _FT8607     0x86070809
#define _FTE716     0xE716080a

#define _FT5416     0x54160002
#define _FT5426     0x54260002
#define _FT5435     0x54350002
#define _FT5436     0x54360002
#define _FT5526     0x55260002
#define _FT5526I    0x5526B002
#define _FT5446     0x54460002
#define _FT5346     0x53460002
#define _FT5446I    0x5446B002
#define _FT5346I    0x5346B002
#define _FT7661     0x76610002
#define _FT7511     0x75110002
#define _FT7421     0x74210002
#define _FT7681     0x76810002
#define _FT3C47U    0x3C47D002
#define _FT3417     0x34170002
#define _FT3517     0x35170002
#define _FT3327     0x33270002
#define _FT3427     0x34270002

#define _FT5626     0x56260001
#define _FT5726     0x57260001
#define _FT5826B    0x5826B001
#define _FT5826S    0x5826C001
#define _FT7811     0x78110001
#define _FT3D47     0x3D470001
#define _FT3617     0x36170001
#define _FT3717     0x37170001
#define _FT3817B    0x3817B001

#define _FT6236U    0x6236D003
#define _FT6336G    0x6336A003
#define _FT6336U    0x6336D003
#define _FT6436U    0x6436D003

#define _FT3267     0x32670004
#define _FT3367     0x33670004

/*[Arima_8100][allen_yu] Modify dynamic detect SM31_module and SM_32_module 20170719 begin*/
#define SM31_IC_TYPE    0x07
#define SM32_IC_TYPE    0x12
/*[Arima_8100][allen_yu] 20170719 end*/

/*************************************************/

/*
 * choose your ic chip type of focaltech
 */
#define FTS_CHIP_TYPE   _FT5436

/******************* Enables *********************/
/*********** 1 to enable, 0 to disable ***********/

/*
 * show debug log info
 * enable it for debug, disable it for release
 */
#define FTS_DEBUG_EN                            1

/*
 * Linux MultiTouch Protocol
 * 1: Protocol B(default), 0: Protocol A
 */
#define FTS_MT_PROTOCOL_B_EN                    1


/*
 * Report Pressure in multitouch
 * 1:enable(default),0:disable
*/
#define FTS_REPORT_PRESSURE_EN                  1

/*
 * Force touch support
 * different pressure for multitouch
 * 1: true pressure for force touch
 * 0: constant pressure(default)
 */
#define FTS_FORCE_TOUCH_EN                      0

/*
 * Gesture function enable
 * default: disable
 */
#define FTS_GESTURE_EN                          0

/*
 * ESD check & protection
 * default: disable
 */
#define FTS_ESDCHECK_EN                         0

/*
 * Production test enable
 * 1: enable, 0:disable(default)
 */
/*[Arima_8100][allen_yu] FT5436 driver AATS test implement 20170606 begin*/
#define FTS_TEST_EN                             1
/*[Arima_8100][allen_yu] 20170606 end*/

/*
 * Glove mode enable
 * 1: enable, 0:disable(default)
 */
#define FTS_GLOVE_EN                            0
/*
 * cover enable
 * 1: enable, 0:disable(default)
 */
#define FTS_COVER_EN                            0
/*
 * Charger enable
 * 1: enable, 0:disable(default)
 */
#define FTS_CHARGER_EN                          0

/*
 * Proximity sensor
 * default: disable
 */
#define FTS_PSENSOR_EN                          0

/*
 * Nodes for tools, please keep enable
 */
#define FTS_SYSFS_NODE_EN                       1
#define FTS_APK_NODE_EN                         1

/*
 * Customer power enable
 * enable it when customer need control TP power
 * default: disable
 */
#define FTS_POWER_SOURCE_CUST_EN                1

/****************************************************/

/********************** Upgrade ****************************/
/*
 * auto upgrade, please keep enable
 */
#define FTS_AUTO_UPGRADE_EN                     1

/*
 * auto upgrade for lcd cfg
 * default: 0
 */
#define FTS_AUTO_UPGRADE_FOR_LCD_CFG_EN         0

/* auto cb check
 * default: disable
 */
#define FTS_AUTO_CLB_EN                         0

/* 
 * Check vendor_id number
 * 0:No check vendor_id (default)  
 * 1/2/3: Check vendor_id for vendor compatibility
 */
/*[Arima_8100][allen_yu] Modify dynamic detect SM31_module and SM_32_module 20170719 begin*/
#define FTS_GET_VENDOR_ID_NUM                   0
/*[Arima_8103][allen_yu] 201710719 end*/

/*
 * vendor_id(s) for vendor(s) to be compatible with.
 * a confirmation of vendor_id(s) is recommended.
 * FTS_GET_VENDOR_ID_NUM == 0, no check vendor id, you may ignore them
 * FTS_GET_VENDOR_ID_NUM >= 1, compatible with FTS_VENDOR_1_ID
 * FTS_GET_VENDOR_ID_NUM >= 2, compatible with FTS_VENDOR_2_ID
 * FTS_GET_VENDOR_ID_NUM == 3, compatible with FTS_VENDOR_3_ID
 */

/*[Arima_8100][allen_yu] Modify dynamic detect SM31_module and SM_32_module 20170719 begin*/
/*[Arima_8103][allen_yu] Fix FT5436 invalid FW to upgrade error 201710718 begin*/
#define FTS_VENDOR_1_ID                         0x00
#define FTS_VENDOR_2_ID                         0x00
#define FTS_VENDOR_3_ID                         0x00
/*[Arima_8100][allen_yu] 20170719 end*/

/*
 * FW_APP.i file for auto upgrade, you must replace it with your own
 * define your own fw_app, the sample one to be replaced is invalid
 * NOTE: if FTS_GET_VENDOR_ID_NUM >= 1, it's the fw corresponding with FTS_VENDOR_1_ID
 */
/*[Arima_8103][allen_yu] Upgrade focaltech touch firmware to V0c 20180424 begin*/
/*[Arima_8103][allen_yu] Upgrade focaltech touch firmware to V0b 20171114 begin*/ 
/*[Arima_8103][allen_yu] Upgrade focaltech touch firmware to V0a (Cause LCM MIPI CLK change to 220) 20171011 begin*/
/*[Arima_8103][allen_yu] Upgrade focaltech touch firmware to V09 20170901 begin*/ 
/*[Arima_8103][allen_yu] Upgrade focaltech touch firmware to V08 20170803 begin*/
/*[Arima_8103][bozhi_lin] Upgrade focaltech touch firmware to V07 20170720 begin*/
/*[Arima_8100][allen_yu] Upgrade focaltech touch firmware to V05 20170710 begin*/
/*[Arima_8100][allen_yu] Upgrade focaltech touch firmware to V04 20170706 begin*/
/*[Arima_8100][allen_yu] Upgrade focaltech touch firmware to V03 20170627 begin*/
/*[Arima_8100][allen_yu] Dynamic detect SM31_TP module and SM32_TP module 20170606 begin*/
#define FTS_UPGRADE_FW_APP                      "include/firmware/FT5436_SM32_0x5a_V0c_D01_20180424_app.i"
/*[Arima_8100][allen_yu] 20170606 end*/
/*[Arima_8100][allen_yu] 20170627 end*/
/*[Arima_8100][allen_yu] 20170706 end*/
/*[Arima_8100][allen_yu] 20170710 end*/
/*[Arima_8103][bozhi_lin] 20170720 end*/
/*[Arima_8103][allen_yu] 20170803 end*/
/*[Arima_8103][allen_yu] 20170901 end*/
/*[Arima_8103][allen_yu] 20171011 end*/
/*[Arima_8103][allen_yu] 20171114 end*/
/*[Arima_8103][allen_yu] 20180424 end*/

/*
 * if FTS_GET_VENDOR_ID_NUM >= 2, fw corrsponding with FTS_VENDOR_2_ID
 * define your own fw_app, the sample one is invalid
 */
/*[Arima_8100][allen_yu] Dynamic detect SM31_TP module and SM32_TP module 20170606 begin*/
#define FTS_UPGRADE_FW2_APP                     "include/firmware/FT3427_SM31_HD_V14_D01_20170218_app.i"
/*[Arima_8100][allen_yu] 20170606 end*/

/*
 * if FTS_GET_VENDOR_ID_NUM == 3, fw corrsponding with FTS_VENDOR_3_ID
 * define your own fw_app, the sample one is invalid
 */
/*[Arima_8103][allen_yu] Upgrade focaltech 2nd source touch firmware to V55 20180509 begin*/
/*[Arima_8103][allen_yu] Upgrade focaltech 2nd source touch firmware to V54 20171122 begin*/
/*[Arima_8103][allen_yu] Upgrade focaltech 2nd source touch firmware to V53 (Cause LCM MIPI CLK change to 220) 20171017 begin*/
/*[Arima_8103][allen_yu] Upgrade focaltech 2nd source touch firmware to V52 20171003 begin*/
/*[Arima_8103][allen_yu] Dynamic detect main source and second source 20170925 begin*/
#define FTS_UPGRADE_FW3_APP                     "include/firmware/FT5436_SM32_0x67_V55_D01_20180507_app.i"
/*[Arima_8103][allen_yu] 20170925 end*/
/*[Arima_8103][allen_yu] 20171003 end*/
/*[Arima_8103][allen_yu] 20171017 end*/
/*[Arima_8103][allen_yu] 20171122 end*/
/*[Arima_8103][allen_yu] 20180509 end*/

/*
 * lcd_cfg.i file for lcd cfg upgrade
 * define your own lcd_cfg.i, the sample one is invalid
 */
//Note: Not used! Please Ignore it!
//#define FTS_UPGRADE_LCD_CFG                     "include/firmware/lcd_cfg.i"

/*
 * upgrade stress test for debug
 * enable it for upgrade debug if needed
 * default: disable
 */
#define FTS_UPGRADE_STRESS_TEST                 0
/* stress test times, default: 1000 */
#define FTS_UPGRADE_TEST_NUMBER                 1000

/*********************************************************/

#endif /* _LINUX_FOCLATECH_CONFIG_H_ */

