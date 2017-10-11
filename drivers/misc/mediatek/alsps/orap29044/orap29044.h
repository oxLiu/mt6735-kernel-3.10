/* 
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
/*
 * Definitions for orient1201 als/ps sensor chip.
 */
#ifndef __ORAP29044_H__
#define __ORAP29044_H__

#include <linux/ioctl.h>

#include <mach/devs.h>
#include <mach/mt_typedefs.h>
#include <mach/mt_gpio.h>
#include <mach/mt_pm_ldo.h>
#include <alsps.h>

#define POWER_NONE_MACRO MT65XX_POWER_NONE



#define ORAP29044_I2C_SLAVE_ADDR 0x88//0x38 //0x3A<-->SEL to VDD  0x38<-->SEL to GND
#if 0
/*REG address*/
#define APS_CONFIGUATION 0x00
#define APS_TIMING_CTL   0x01
#define APS_ALS_CTL      0x02
#define APS_INT_STATUS   0x03
#define APS_PS_CTL       0x04
#define APS_BOTH_DATA    0x05
#define APS_ALS_WINDOW   0x08


/*ALS control*/
#define ALS_LEVEL_64  0xA0
#define ALS_LEVEL_33  0x80
#define ALS_LEVEL_17  0x60
#define ALS_LEVEL_9   0x40
#define ALS_LEVEL_5   0x20
#define ALS_LEVEL_3   0x00

#else
/* registers */
#define ORAP1201_REG_VENDOR_REV                 0x06
#define ORAP1201_VENDOR                         1
#define ORAP1201_VENDOR_MASK                    0x0F
#define ORAP1201_REV                            4
#define ORAP1201_REV_SHIFT                      4
#define ORAP1201_REG_DEVICE                     0x22
#define ORAP1201_DEVICE                        	22 

// Table 1: all i2c registers and bits per register
#define REG_CMD_1		0x01 // configure, range is reg 1 bit 1
#define REG_CMD_2		0x02 // interrupt control

#define REG_INT_LOW_PROX	0x03 // 8 bits intr low thresh for prox
#define REG_INT_HIGH_PROX	0x04 // 8 bits intr high thresh for prox
#define REG_INT_LOW_ALS 	0x05 // 8 bits intr low thresh for ALS-IR
#define REG_INT_LOW_HIGH_ALS	0x06 // 8 bits(0-3,4-7) intr high/low thresh for ALS-IR
#define REG_INT_HIGH_ALS	0x07 // 8 bits intr high thresh for ALS-IR

#define REG_DATA_PROX		0x08 // 8 bits of PROX data
#define REG_DATA_LSB_ALS	0x09 // 8 bits of ALS data
#define REG_DATA_MSB_ALS	0x0A // 4 bits of ALS MSB data

#define ORIENT_TEST1 		0x0E // test write 0x00
#define ORIENT_TEST2 		0x0F // test write 0x00

#define ORIENT_MOD_MASK		0xE0
#define ORIENT_MOD_POWERDOWN	0

#define ORIENT_MOD_ALS_ONCE	1
#define ORIENT_MOD_IR_ONCE		2
#define ORIENT_MOD_PS_ONCE		3
#define ORIENT_MOD_RESERVED	4
#define ORIENT_MOD_ALS_CONT	5
#define ORIENT_MOD_IR_CONT		6
#define ORIENT_MOD_PS_CONT		7
#define ORIENT_MOD_DEFAULT		8

#define PROX_EN_MASK          0x80 // prox sense on mask, 1=on, 0=off
#define PROX_CONT_MASK        0x70 // prox sense contimnuous mask
//IR_CURRENT_MASK is now PROX_DR_MASK with just 0 or 1 settings
#define PROX_DR_MASK          0x08 // prox drive pulse 220ma sink mask def=0 110ma
#define ALS_EN_MASK           0x04 // prox sense enabled contimnuous mask
#define ALS_RANGE_HIGH_MASK   0x02 // ALS range high LUX mask
#define ALSIR_MODE_SPECT_MASK 0x01 // prox sense contimnuous mask

#define IR_CURRENT_MASK		0xC0
#define IR_FREQ_MASK		0x30
#define SENSOR_RANGE_MASK	0x03
#define ORIENT_RES_MASK		0x0C

#define  PROX_FLAG  7
#define  ALS_FLAG     3
#endif

#define ORAP29044_PS_MAX_NF		200
#define ORAP29044_PS_HT_ADD		20
#define ORAP29044_PS_LT_ADD		10

#endif





