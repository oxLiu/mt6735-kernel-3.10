/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

#ifndef TOUCHPANEL_H__
#define TOUCHPANEL_H__

/* Pre-defined definition */
#define TPD_TYPE_CAPACITIVE
#define TPD_TYPE_RESISTIVE
#define TPD_POWER_SOURCE         
#define TPD_I2C_NUMBER           1
#define TPD_WAKEUP_TRIAL         60
#define TPD_WAKEUP_DELAY         100

#define TPD_VELOCITY_CUSTOM_X 12
#define TPD_VELOCITY_CUSTOM_Y 16
//#define TPD_AUTO_UPGRADE_SUPPORT

#define TPD_DELAY                (2*HZ/100)
//mofidy at 20120419 by zhu
//#define TPD_RES_X                480
//#define TPD_RES_Y                800
#define TPD_RES_X               LCM_WIDTH// 540
#define TPD_RES_Y               LCM_HEIGHT// 960
//mofidy at 20120419 by zhu end
#define TPD_CALIBRATION_MATRIX  {962,0,0,0,1600,0,0,0};

//#define TPD_HAVE_CALIBRATION
//#define TPD_HAVE_BUTTON
//#define TPD_HAVE_TREMBLE_ELIMINATION
#define TPD_HAVE_BUTTON
#define TPD_BUTTON_HEIGH        (100)
//mofidy at 20120419 by zhu
//#define TPD_KEY_COUNT           3
//#define TPD_KEYS                { KEY_MENU, KEY_HOMEPAGE ,KEY_BACK}
//#define TPD_KEYS_DIM            {{80,850,160,TPD_BUTTON_HEIGH},{240,850,160,TPD_BUTTON_HEIGH},{400,850,160,TPD_BUTTON_HEIGH}}
#if defined(XITAI_DISPLAY_LOCATION_CHARGE_LOG_QHD)//960X540
#define TPD_KEY_COUNT           3
#define TPD_KEYS                 { KEY_BACK,KEY_HOMEPAGE ,KEY_MENU}//{ KEY_MENU, KEY_HOME ,KEY_BACK,KEY_SEARCH}
#define TPD_KEYS_DIM     {{100,1000,100,TPD_BUTTON_HEIGH},{300,1000,100,TPD_BUTTON_HEIGH},{400,1000,100,TPD_BUTTON_HEIGH}} 
#elif defined(XITAI_DISPLAY_LOCATION_CHARGE_LOG_FWVGA)//854X480
#define TPD_KEY_COUNT           3
#define TPD_KEYS                 { KEY_MENU,KEY_HOMEPAGE ,KEY_BACK}//{ KEY_MENU, KEY_HOME ,KEY_BACK,KEY_SEARCH}
#define TPD_KEYS_DIM     {{80,910,160,TPD_BUTTON_HEIGH},{240,910,160,TPD_BUTTON_HEIGH},{400,910,160,TPD_BUTTON_HEIGH}} 
//#define TPD_KEYS_DIM     {{73,910,146,TPD_BUTTON_HEIGH},{219,910,146,TPD_BUTTON_HEIGH},{365,910,146,TPD_BUTTON_HEIGH}} 
#else//800X480
#define TPD_KEY_COUNT           4
#define TPD_KEYS                 { KEY_SEARCH,KEY_BACK,KEY_MENU,KEY_HOMEPAGE}
#define TPD_KEYS_DIM     {{60,870,120,TPD_BUTTON_HEIGH},{180,870,120,TPD_BUTTON_HEIGH},{300,870,120,TPD_BUTTON_HEIGH},{420,870,120,TPD_BUTTON_HEIGH}} 
#endif
//mofidy at 20120419 by zhu end 
#endif /* TOUCHPANEL_H__ */
