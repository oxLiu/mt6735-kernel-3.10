/* drivers/hwmon/mt6516/amit/orap29044.c - ORAP29044 ALS/PS driver
 * 
 * Author: MingHsien Hsieh <minghsien.hsieh@mediatek.com>
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

#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/irq.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/workqueue.h>
#include <linux/kobject.h>
#include <linux/earlysuspend.h>
#include <linux/platform_device.h>
#include <asm/atomic.h>
#include <linux/version.h>
#include <linux/wakelock.h>
#include <linux/hwmsensor.h>
#include <linux/hwmsen_dev.h>
#include <linux/sensors_io.h>
#include <asm/io.h>
#include <cust_eint.h>
#include <cust_alsps.h>
#include "orap29044.h"
#include <linux/hwmsen_helper.h>
#include <mach/mt_pm_ldo.h>

#ifdef MT6516
#include <mach/mt6516_devs.h>
#include <mach/mt6516_typedefs.h>
#include <mach/mt6516_gpio.h>
#include <mach/mt6516_pll.h>
#endif

#ifdef MT6573
#include <mach/mt6573_devs.h>
#include <mach/mt6573_typedefs.h>
#include <mach/mt6573_gpio.h>
#include <mach/mt6573_pll.h>
#endif

#ifdef MT6575
#include <mach/mt6575_devs.h>
#include <mach/mt6575_typedefs.h>
#include <mach/mt6575_gpio.h>
#include <mach/mt6575_pm_ldo.h>
#endif

#ifdef MT6577
#include <mach/mt6577_devs.h>
#include <mach/mt6577_typedefs.h>
#include <mach/mt6577_gpio.h>
#include <mach/mt6577_pm_ldo.h>
#endif
#ifdef MT6589
#include <mach/mt_typedefs.h>
#include <mach/mt_gpio.h>
#include <mach/mt_pm_ldo.h>

#endif
#ifdef MT6572
//#include <mach/mt_devs.h>
#include <mach/mt_typedefs.h>
#include <mach/mt_gpio.h>
#include <mach/mt_pm_ldo.h>
#endif
#ifdef MT6573
extern void mt65xx_eint_unmask(unsigned int line);
extern void mt65xx_eint_mask(unsigned int line);
extern void mt65xx_eint_set_polarity(kal_uint8 eintno, kal_bool ACT_Polarity);
extern void mt65xx_eint_set_hw_debounce(kal_uint8 eintno, kal_uint32 ms);
extern kal_uint32 mt65xx_eint_set_sens(kal_uint8 eintno, kal_bool sens);
extern void mt65xx_eint_registration(kal_uint8 eintno, kal_bool Dbounce_En,
                                     kal_bool ACT_Polarity, void (EINT_FUNC_PTR)(void),
                                     kal_bool auto_umask);

#endif

#ifdef MT6575
extern void mt65xx_eint_unmask(unsigned int line);
extern void mt65xx_eint_mask(unsigned int line);
extern void mt65xx_eint_set_polarity(kal_uint8 eintno, kal_bool ACT_Polarity);
extern void mt65xx_eint_set_hw_debounce(kal_uint8 eintno, kal_uint32 ms);
extern kal_uint32 mt65xx_eint_set_sens(kal_uint8 eintno, kal_bool sens);
extern void mt65xx_eint_registration(kal_uint8 eintno, kal_bool Dbounce_En,
                                     kal_bool ACT_Polarity, void (EINT_FUNC_PTR)(void),
                                     kal_bool auto_umask);

#endif
#ifdef MT6577
extern void mt65xx_eint_unmask(unsigned int line);
extern void mt65xx_eint_mask(unsigned int line);
extern void mt65xx_eint_set_polarity(unsigned int eint_num, unsigned int pol);
extern void mt65xx_eint_set_hw_debounce(unsigned int eint_num, unsigned int ms);
extern unsigned int mt65xx_eint_set_sens(unsigned int eint_num, unsigned int sens);
extern void mt65xx_eint_registration(unsigned int eint_num, unsigned int is_deb_en, unsigned int pol, void (EINT_FUNC_PTR)(void), unsigned int is_auto_umask);
#endif
#ifdef MT6589
	extern void mt65xx_eint_unmask(unsigned int line);
	extern void mt65xx_eint_mask(unsigned int line);
	extern void mt65xx_eint_set_polarity(unsigned int eint_num, unsigned int pol);
	extern void mt65xx_eint_set_hw_debounce(unsigned int eint_num, unsigned int ms);
	extern unsigned int mt65xx_eint_set_sens(unsigned int eint_num, unsigned int sens);
	extern void mt65xx_eint_registration(unsigned int eint_num, unsigned int is_deb_en, unsigned int pol, void (EINT_FUNC_PTR)(void), unsigned int is_auto_umask);
#endif
#ifdef MT6572
extern void mt65xx_eint_unmask(unsigned int line);
		extern void mt65xx_eint_mask(unsigned int line);
		extern void mt65xx_eint_set_polarity(unsigned int eint_num, unsigned int pol);
		extern void mt65xx_eint_set_hw_debounce(unsigned int eint_num, unsigned int ms);
		extern unsigned int mt65xx_eint_set_sens(unsigned int eint_num, unsigned int sens);
		extern void mt65xx_eint_registration(unsigned int eint_num, unsigned int is_deb_en, unsigned int pol, void (EINT_FUNC_PTR)(void), unsigned int is_auto_umask);

#endif
/*-------------------------MT6516&MT6573 define-------------------------------*/
#ifdef MT6516
#define POWER_NONE_MACRO MT6516_POWER_NONE
#endif

#ifdef MT6572
#define POWER_NONE_MACRO MT65XX_POWER_NONE
#endif

#ifdef MT6575
#define POWER_NONE_MACRO MT65XX_POWER_NONE
#endif

#ifdef MT6577
#define POWER_NONE_MACRO MT65XX_POWER_NONE
#endif
#ifdef MT6589
#define POWER_NONE_MACRO MT65XX_POWER_NONE
#endif
#ifdef MT6572
#define POWER_NONE_MACRO MT65XX_POWER_NONE
#endif
//#define OPEN_EINT 
/******************************************************************************
 * configuration
*******************************************************************************/
#define I2C_DRIVERID_ORAP29044 1201
/*----------------------------------------------------------------------------*/
#define ORAP29044_I2C_ADDR_RAR 0   /*!< the index in obj->hw->i2c_addr: alert response address */
#define ORAP29044_I2C_ADDR_ALS 1   /*!< the index in obj->hw->i2c_addr: ALS address */
#define ORAP29044_I2C_ADDR_PS  2   /*!< the index in obj->hw->i2c_addr: PS address */
#define ORAP29044_DEV_NAME     "orap29044"
/*----------------------------------------------------------------------------*/
//#define ORAP29044_DEBUG
#define APS_TAG                  "[ALS/PS] "
//#define ORAP29044_DEBUG
#if defined(ORAP29044_DEBUG)
#define APS_FUN(f)               printk(KERN_INFO APS_TAG"%s\n", __FUNCTION__)
#define APS_ERR(fmt, args...)    printk(KERN_ERR  APS_TAG"%s %d : "fmt, __FUNCTION__, __LINE__, ##args)
#define APS_LOG(fmt, args...)    printk(KERN_INFO APS_TAG fmt, ##args)
#define APS_DBG(fmt, args...)    printk(KERN_INFO fmt, ##args)
#else
#define APS_FUN(f)		 do{}while(0)
#define APS_ERR(fmt, args...)	 do{}while(0)
#define APS_LOG(fmt, args...)    do{}while(0)
#define APS_DBG(fmt, args...)    do{}while(0)
#endif                 
/******************************************************************************
 * extern functions
*******************************************************************************/
#ifdef MT6516
extern void MT6516_EINTIRQUnmask(unsigned int line);
extern void MT6516_EINTIRQMask(unsigned int line);
extern void MT6516_EINT_Set_Polarity(kal_uint8 eintno, kal_bool ACT_Polarity);
extern void MT6516_EINT_Set_HW_Debounce(kal_uint8 eintno, kal_uint32 ms);
extern kal_uint32 MT6516_EINT_Set_Sensitivity(kal_uint8 eintno, kal_bool sens);
extern void MT6516_EINT_Registration(kal_uint8 eintno, kal_bool Dbounce_En,
                                     kal_bool ACT_Polarity, void (EINT_FUNC_PTR)(void),
                                     kal_bool auto_umask);
#endif
/*----------------------------------------------------------------------------*/
#define mt6516_I2C_DATA_PORT        ((base) + 0x0000)
#define mt6516_I2C_SLAVE_ADDR       ((base) + 0x0004)
#define mt6516_I2C_INTR_MASK        ((base) + 0x0008)
#define mt6516_I2C_INTR_STAT        ((base) + 0x000c)
#define mt6516_I2C_CONTROL          ((base) + 0x0010)
#define mt6516_I2C_TRANSFER_LEN     ((base) + 0x0014)
#define mt6516_I2C_TRANSAC_LEN      ((base) + 0x0018)
#define mt6516_I2C_DELAY_LEN        ((base) + 0x001c)
#define mt6516_I2C_TIMING           ((base) + 0x0020)
#define mt6516_I2C_START            ((base) + 0x0024)
#define mt6516_I2C_FIFO_STAT        ((base) + 0x0030)
#define mt6516_I2C_FIFO_THRESH      ((base) + 0x0034)
#define mt6516_I2C_FIFO_ADDR_CLR    ((base) + 0x0038)
#define mt6516_I2C_IO_CONFIG        ((base) + 0x0040)
#define mt6516_I2C_DEBUG            ((base) + 0x0044)
#define mt6516_I2C_HS               ((base) + 0x0048)
#define mt6516_I2C_DEBUGSTAT        ((base) + 0x0064)
#define mt6516_I2C_DEBUGCTRL        ((base) + 0x0068)
/*----------------------------------------------------------------------------*/
static struct i2c_client *orap29044_i2c_client = NULL;
/*----------------------------------------------------------------------------*/
static const struct i2c_device_id orap29044_i2c_id[] = {{ORAP29044_DEV_NAME,0},{}};
static const struct i2c_board_info __initdata i2c_ORAP29044= {I2C_BOARD_INFO("orap29044",(0X88>>1))};
//the adapter id & i2c address will be available in customization*/
//static unsigned short orap29044_force[] = {0x00, ORAP29044_I2C_SLAVE_ADDR, I2C_CLIENT_END, I2C_CLIENT_END};
//static const unsigned short *const orap29044_forces[] = { orap29044_force, NULL };
//static struct i2c_client_address_data orap29044_addr_data = { .forces = orap29044_forces,};
/*----------------------------------------------------------------------------*/
static int orap29044_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id); 
static int orap29044_i2c_remove(struct i2c_client *client);
//#ifdef 1//MTK_AUTO_DETECT_ALSPS
#if 1//MTK_AUTO_DETECT_ALSPS
static int orap29044_i2c_detect(struct i2c_client *client, int kind, struct i2c_board_info *info);
#endif
/*----------------------------------------------------------------------------*/
static int orap29044_i2c_suspend(struct i2c_client *client, pm_message_t msg);
static int orap29044_i2c_resume(struct i2c_client *client);
//add at 20130926 by zhu
struct wake_lock orap29044_chrg_lock;
static int orap29044_lock = 0;
static DEFINE_MUTEX(orap29044_mutex);	

struct wake_lock ps_lock;
//add at 20130926 by zhu end
static struct orap29044_priv *g_orap29044_ptr = NULL;

/*----------------------------------------------------------------------------*/
typedef enum {
    CMC_TRC_APS_DATA = 0x0002,
    CMC_TRC_EINT    = 0x0004,
    CMC_TRC_IOCTL   = 0x0008,
    CMC_TRC_I2C     = 0x0010,
    CMC_TRC_CVT_ALS = 0x0020,
    CMC_TRC_CVT_PS  = 0x0040,
    CMC_TRC_DEBUG   = 0x8000,
} CMC_TRC;
/*----------------------------------------------------------------------------*/
typedef enum {
    CMC_BIT_ALS    = 1,
    CMC_BIT_PS     = 2,
} CMC_BIT;
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
 //#undef CONFIG_HAS_EARLYSUSPEND

struct orap29044_priv {
    struct alsps_hw  *hw;
    struct i2c_client *client;
    struct delayed_work  eint_work;
	struct timer_list   end_read_ps_timer;
	//struct timer_list   first_read_ps_timer;
	//struct timer_list   first_read_als_timer;
    
    /*misc*/
    atomic_t    trace;
    atomic_t    i2c_retry;
    atomic_t    als_suspend;
    atomic_t    als_debounce;   /*debounce time after enabling als*/
    atomic_t    als_deb_on;     /*indicates if the debounce is on*/
    atomic_t    als_deb_end;    /*the jiffies representing the end of debounce*/
    atomic_t    ps_mask;        /*mask ps: always return far away*/
    atomic_t    ps_debounce;    /*debounce time after enabling ps*/
    atomic_t    ps_deb_on;      /*indicates if the debounce is on*/
    atomic_t    ps_deb_end;     /*the jiffies representing the end of debounce*/
    atomic_t    ps_suspend;


    /*data*/
    //u8         als;
    u16         als;
    u8          ps;
    u8          _align;
    u16         als_level_num;
    u16         als_value_num;
    u32         als_level[C_CUST_ALS_LEVEL-1];
    u32         als_value[C_CUST_ALS_LEVEL];

	u8 ps_nf;
	u8 ps_ht;
	u8 ps_lt;
	u8 last_ps_status;
	u8 eint_work_running;

    bool    als_enable;    /*record current als status*/
	unsigned int    als_widow_loss; 
	
    bool    ps_enable;     /*record current ps status*/
    unsigned int    ps_thd_val;     /*the cmd value can't be read, stored in ram*/
    ulong       enable;         /*record HAL enalbe status*/
    ulong       pending_intr;   /*pending interrupt*/
    //ulong        first_read;   // record first read ps and als
    /*early suspend*/
#if defined(CONFIG_HAS_EARLYSUSPEND)
    struct early_suspend    early_drv;
#endif     
};
/*----------------------------------------------------------------------------*/
static struct i2c_driver orap29044_i2c_driver = {	
	.probe      = orap29044_i2c_probe,
	.remove     = orap29044_i2c_remove,
//#ifdef 1//MTK_AUTO_DETECT_ALSPS
#if 1//MTK_AUTO_DETECT_ALSPS

	.detect     = orap29044_i2c_detect,
#endif
	.suspend    = orap29044_i2c_suspend,
	.resume     = orap29044_i2c_resume,
	.id_table   = orap29044_i2c_id,
	//.address_data = &orap29044_addr_data,
	.driver = {
//		.owner          = THIS_MODULE,
		.name           = ORAP29044_DEV_NAME,
	},
};

static struct orap29044_priv *orap29044_obj = NULL;
static struct platform_driver orap29044_alsps_driver;

static int orap29044_get_ps_value(struct orap29044_priv *obj, u8 ps);
static int orap29044_get_als_value(struct orap29044_priv *obj, u16 als);

static struct alsps_hw cust_alsps_hw = {
    .i2c_num    = 3,//0,
    //.polling_mode =1,
    .polling_mode_ps=0,
	.polling_mode_als=1,
    .power_id   = MT65XX_POWER_NONE,    /*LDO is not used*/
    .power_vol  = VOL_DEFAULT,          /*LDO is not used*/
    .i2c_addr   = {0x88, 0x00, 0x00, 0x00},
	//[1]
    //.als_level  = { 0,  1,  1,   7,  15,  15,  100, 1000, 2000,  3000,  6000, 10000, 14000, 18000, 20000},
    //.als_value  = {40, 40, 90,  90, 160, 160,  225,  320,  640,  1280,  1280,  2600,  2600, 2600,  10240, 10240},

	//[2]
    //.als_level  = { 10,  20,  30,   40,  50,  100,  150, 1000, 2000,  3000,  6000, 10000, 14000, 18000, 20000},
    //.als_value  = {10, 20, 40,  90, 120, 160,  225,  280,  580,  980,  1280,  2600,  2600, 2600,  10240, 10240},

	//[3]
    //.als_value  = { 15,  31,  49,   99, 139,  179,  239,  299,  599,  999,  1999,  2999,  3999, 7999,  9999, 10240},
	//[2011-08-02] JasonJ. Tuned value, it's best User Experience value
    //.als_level  = { 10,  20,  25,   30,   35,  40,  50,   60,   70,   80,   100,   150, 1000, 2000,  3000,  4095},
    .als_level  = { 10,  20,  30,   50,  100,  200,  300,   500,  600,  800,  1000,  1200, 1500, 2000,  3000,  4095},
    .als_value  = { 15,  31,  49,   99, 139,  179,  239,  299,  599,  999,  1999,  2999,  3999, 7999,  9999, 10240},
    .ps_threshold = 200,
};
 struct alsps_hw *Orap29044_get_cust_alsps_hw(void)
{
    return &cust_alsps_hw;
}

/*----------------------------------------------------------------------------*/
static int hwmsen_read_byte_sr(struct i2c_client *client, u8 addr, u8 *data)
{
   u8 buf;
    int ret = 0;
	
    client->addr = (client->addr & I2C_MASK_FLAG) | I2C_WR_FLAG |I2C_RS_FLAG;
    buf = addr;
	//APS_ERR("send command ic_addr==%2x addr==%d !!\n",client->addr,buf);
	ret = i2c_master_send(client, (const char*)&buf, 1<<8 | 1);
    //ret = i2c_master_send(client, (const char*)&buf, 1);
    if (ret < 0) {
        APS_ERR("send command error!!\n");
        return -EFAULT;
    }

    *data = buf;
	client->addr = client->addr& I2C_MASK_FLAG;
    return 0;
}

static void orap29044_dumpReg(struct i2c_client *client)
{
  int i=0;
  u8 addr = 0x00;
  u8 regdata=0;
  for(i=0; i<=10; i++)
  {
    //dump all
    hwmsen_read_byte_sr(client,addr,&regdata);
	APS_LOG("Reg addr=%x regdata=%x\n",addr,regdata);
	//snprintf(buf,1,"%c",regdata);
	addr++;
	if(addr > 0x0a)
		break;
  }
}
/*
void orap29044_first_read_ps_func()
{
	
	if (orap29044_obj != NULL)
	{
		schedule_work(&orap29044_obj->eint_work);
	}
	
	if(orap29044_obj->first_read& 0x04 )
	{
	  mod_timer(&orap29044_obj->first_read_ps_timer, jiffies + 20); 
	}
}

void orap29044_first_read_als_func()
{
	
	if (orap29044_obj != NULL)
	{
		schedule_work(&orap29044_obj->eint_work);
	}
	
	if(orap29044_obj->first_read&0x02)
	{
	  mod_timer(&orap29044_obj->first_read_als_timer, jiffies + 20); 
	}
}

*/
/*----------------------------------------------------------------------------*/
int orap29044_get_timing(void)
{
return 200;
/*
	u32 base = I2C2_BASE; 
	return (__raw_readw(mt6516_I2C_HS) << 16) | (__raw_readw(mt6516_I2C_TIMING));
*/
}
/*----------------------------------------------------------------------------*/
/*
int orap29044_config_timing(int sample_div, int step_div)
{
	u32 base = I2C2_BASE; 
	unsigned long tmp;

	tmp  = __raw_readw(mt6516_I2C_TIMING) & ~((0x7 << 8) | (0x1f << 0));
	tmp  = (sample_div & 0x7) << 8 | (step_div & 0x1f) << 0 | tmp;

	return (__raw_readw(mt6516_I2C_HS) << 16) | (tmp);
}*/
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
#if 0
int orap29044_read_data(struct i2c_client *client, u8 *data)
{


	struct orap29044_priv *obj = i2c_get_clientdata(client);    
	int ret = 0;
	//u8 aps_data=0;
	//u8 addr = 0x00;

	//hwmsen_read_byte_sr(client,APS_BOTH_DATA,&aps_data);
	if(hwmsen_read_byte_sr(client,APS_BOTH_DATA,data))
	{
		APS_ERR("reads aps data = %d\n", ret);
		return -EFAULT;
	}
	
	if(atomic_read(&obj->trace) & CMC_TRC_APS_DATA)
	{
		APS_DBG("APS:  0x%04X\n", (u32)(*data));
	}
	return 0;    
}
#endif

int orap29044_read_als_data(struct i2c_client *client, u16 *data)
{
	struct orap29044_priv *obj = i2c_get_clientdata(client);    
	int ret = 0;
	u16 temp=0;
	u16 data0=0;
	//u8 aps_data=0;
	//u8 addr = 0x00;

	//hwmsen_read_byte_sr(client,APS_BOTH_DATA,&aps_data);
	if(ret=hwmsen_read_byte_sr(client,REG_DATA_LSB_ALS,data))
	{
		APS_ERR("reads aps data = %d\n", ret);
		return -EFAULT;
	}
	temp=(*data);
	temp&=0x00ff;
	if(ret=hwmsen_read_byte_sr(client,REG_DATA_MSB_ALS,data))
	{
		APS_ERR("reads aps data = %d\n", ret);
		return -EFAULT;
	}
	data0=(*data);
	data0<<=8;
	data0&= 0x0f00;
	data0|=temp;
	*data=data0;
	
	
	
	if(atomic_read(&obj->trace) & CMC_TRC_APS_DATA)
	{
		APS_DBG("APS:  0x%X\n", (u32)(*data));
	}
	return 0;    
}

#if 1
int orap29044_read_ps_data(struct i2c_client *client, u8 *data)
{
	struct orap29044_priv *obj = i2c_get_clientdata(client);    
	int ret = 0;
	//u8 aps_data=0;
	//u8 addr = 0x00;

	//hwmsen_read_byte_sr(client,APS_BOTH_DATA,&aps_data);
	if(hwmsen_read_byte_sr(client,REG_DATA_PROX,data))
	{
		APS_ERR("reads aps data = %d\n", ret);
		return -EFAULT;
	}
	
	if(atomic_read(&obj->trace) & CMC_TRC_APS_DATA)
	{
		APS_DBG("APS:  0x%04X\n", (u32)(*data));
	}
	return 0;    
}
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

int orap29044_init_device(struct i2c_client *client)
{
       int ret=0;
       // software reset
    //hwmsen_write_byte(client, REG_CMD_1, 0x00);
	//hwmsen_write_byte(client, 0x0f,  0x29);
	//hwmsen_write_byte(client, 0x0e, 0x00);
	//hwmsen_write_byte(client, 0x0f, 0x00);
	//end
	   
	ret = hwmsen_write_byte(client, REG_CMD_1, 0x30); // set Prox sleep period  0x20
	printk("orap29044_init_device:%d\n",ret);
	// 0x28 current 220ma
	if (ret < 0) {
 		APS_ERR(KERN_INFO": %s orap1201 set default config failed\n", __func__);
		return -EINVAL;
	}
	
	#ifdef OPEN_EINT
	ret = hwmsen_write_byte(client, REG_CMD_2, 0x02); // set INT period of ps and als to 8 cycles.
	if (ret < 0) {
 		APS_ERR(KERN_INFO": %s orap1201 set default config failed\n", __func__);
		return -EINVAL;
	}
	ret = hwmsen_write_byte(client, REG_INT_LOW_PROX, 0x02);
	ret = hwmsen_write_byte(client, REG_INT_HIGH_PROX, 0x05);
	ret = hwmsen_write_byte(client, REG_INT_LOW_ALS, 0x2C);
	ret = hwmsen_write_byte(client, REG_INT_LOW_HIGH_ALS, 0xC1);
	ret = hwmsen_write_byte(client, REG_INT_HIGH_ALS, 0x5D); // 8 bits intr high thresh for ALS-IR to 01.
	if (ret < 0) {
 		APS_DBG(KERN_INFO": %s orap1201 set intr high thresh for ALS-I failed\n", __func__);
		return -EINVAL;
	}
	#endif

	return 0;
}
#if  0
/*----------------------------------------------------------------------------*/
int orap29044_set_als_level(struct i2c_client *client, int level)
{
    int res =0;
	u8 data =0;
	res = hwmsen_read_byte_sr(client, APS_ALS_CTL, &data);
	if(res < 0)
	{
	   return -1;
	}
	data = data & 0x1f;//0001 1111 clear 7:5
	if(64 == level)
	{
	   data |= ALS_LEVEL_64;
	}
	else if(33 == level)
	{
	   data |= ALS_LEVEL_33;
	}
	else if(17 == level)
	{
	   data |= ALS_LEVEL_17;
	}
	else if(9 == level)
	{
	   data |= ALS_LEVEL_9;
	}
	else if(5 == level)
	{
	   data |= ALS_LEVEL_5;
	}
	else if(3 == level)
	{
	   //do nothing
	}
	else
	{
	   APS_ERR("do not support this level %d\n" ,level);
	   return -1;
	}

	res = hwmsen_write_byte(client,APS_ALS_CTL,data);
	if(res<0)
	{
	   return -1;
	}

	return res;
}
#endif
#if  0 // fixed by lpp
/*----------------------------------------------------------------------------*/
int orap29044_set_low_lux(struct i2c_client *client, u8 low_lux)
{
    int res =0;
	u8 data =0;
	if(low_lux > 0x0f)
	{
	   APS_ERR("do not support this low_lux %d\n" ,low_lux);
	   return -1;
	}
	res = hwmsen_read_byte_sr(client, APS_ALS_CTL, &data);
	if(res < 0)
	{
	   return -1;
	}
	data = data & 0xe0; //1110 0000 clear 4:0
	
    data = data | low_lux;
	res = hwmsen_write_byte(client,APS_ALS_CTL,data);
	if(res < 0)
	{
	   return -1;
	}

	return res;
}


/*----------------------------------------------------------------------------*/
//loss from 0~15
/*----------------------------------------------------------------------------*/

int orap29044_set_window_loss(struct i2c_client *client, unsigned int loss)
{
	int res =0;
	u8 data =0;
/*
	switch(loss)
	{
	case 0:
		data = 0x00;
		break;
	case 1:
		data = 0x01;
		break;
	case 2:
		data = 0x02;
		break;
	case 3:
		data = 0x03;
		break;
	case 1:
		data = 0x01;
		break;
	}
	*/
	APS_LOG("loss  %x\n" ,(u8)loss);
	
	res = hwmsen_write_byte(client, APS_ALS_WINDOW, (u8)loss);
	if(res < 0)
	{
	    return -1;
	}
	
	return res;
}

/*----------------------------------------------------------------------------*/

int orap29044_set_ps_threshold(struct i2c_client *client, unsigned int threshold)
{
    int res =0;
	u8 data =0;
	
	if(threshold > 32)
	{
	   APS_ERR("do not support this ps threshold %d\n" ,threshold);
	   return -1;
	}
	res = hwmsen_read_byte_sr(client, APS_PS_CTL, &data);
	if(res < 0)
	{
	   return -1;
	}
	APS_LOG("threshold  %x\n" ,(u8)threshold);

	data = data & 0xE0; //1110 0000 clear 4:0
	
	data |= (u8)threshold;
	res = hwmsen_write_byte(client,APS_PS_CTL,data);
	if(res < 0)
	{
	   return -1;
	}
	
	return res;
   
}
#endif


/*----------------------------------------------------------------------------*/
static void orap29044_power(struct alsps_hw *hw, unsigned int on) 
{
	static unsigned int power_on = 0;

	//APS_LOG("power %s\n", on ? "on" : "off");

	if(hw->power_id != POWER_NONE_MACRO)
	{
		if(power_on == on)
		{
			APS_LOG("ignore power control: %d\n", on);
		}
		else if(on)
		{
			if(!hwPowerOn(hw->power_id, hw->power_vol, "ORAP29044")) 
			{
				APS_ERR("power on fails!!\n");
			}
		}
		else
		{
			if(!hwPowerDown(hw->power_id, "ORAP29044")) 
			{
				APS_ERR("power off fail!!\n");   
			}
		}
	}
	power_on = on;
}
/*----------------------------------------------------------------------------*/
static int orap29044_enable_als(struct i2c_client *client, bool enable)
{
    APS_LOG(" orap29044_enable_als %d \n",enable); 
	struct orap29044_priv *obj = i2c_get_clientdata(client);
	int err=0;
	int trc = atomic_read(&obj->trace);
	u8 regdata=0;
	u8 ret=0;
	if(enable == obj->als_enable)
	{
	   return 0;
	}


	#if 0 //add by lpp
	ret=|regdata;
	if(enable == TRUE)//enable als
	{
		ret=|0x04;
		(hwmsen_write_byte(client, REG_CMD_1, &ret));
	
	}
	else
	{
		ret=&0xfb;
		(hwmsen_write_byte(client, REG_CMD_1, &ret));	
	}
	#endif
	APS_LOG(" orap29044_enable_als 00h=%x \n",regdata); 
#if 1
	if(hwmsen_read_byte_sr(client, REG_CMD_1, &regdata))
	{
		APS_ERR("read APS_CONFIGUATION register err!\n");
		return -1;
	}
#if 0
	//
    //APS_LOG(" orap29044_enable_als 00h=%x \n",regdata); 
    orap29044_enable_ps(client, true);
	//
#endif
	//regdata &= 0b11111100; //first set 00 to clear bit
	
	if(enable == TRUE)//enable als
	{
	     APS_LOG("first enable als!\n");
		 if(true == obj->ps_enable)
		 {
		   APS_LOG("ALS(1): enable both \n");
		   atomic_set(&obj->ps_deb_on, 1);
		   atomic_set(&obj->ps_deb_end, jiffies+atomic_read(&obj->ps_debounce)/(1000/HZ));
		   regdata |= 0b10000100; //enable both
		 }
		 if(false == obj->ps_enable)
		 {
		   APS_LOG("ALS(1): enable als only \n");
		   regdata |= 0b00000100; //only enable als
		 }
		 atomic_set(&obj->als_deb_on, 1);
		 atomic_set(&obj->als_deb_end, jiffies+atomic_read(&obj->als_debounce)/(1000/HZ));
		 set_bit(ALS_FLAG,  &obj->pending_intr);// CMC_BIT_ALS
		 #if defined OPEN_EINT
		 schedule_delayed_work(&obj->eint_work,230); //after enable the value is not accurate
		 #endif
		 APS_LOG("first enalbe als set pending interrupt %d\n",obj->pending_intr);
	}
	else
	{
		if(true == obj->ps_enable)
		 {
		   APS_LOG("ALS(0):enable ps only \n");
		   atomic_set(&obj->ps_deb_on, 1);
		   atomic_set(&obj->ps_deb_end, jiffies+atomic_read(&obj->ps_debounce)/(1000/HZ));
		   regdata |= 0b10000000;//only enable ps
		   set_bit(PROX_FLAG,  &obj->pending_intr);// CMC_BIT_PS
		   #ifdef  OPEN_EINT
		   schedule_delayed_work(&obj->eint_work,120);
		   #endif
		 }
		 if(false == obj->ps_enable)
		 {
		   APS_LOG("ALS(0): disable both \n");
		   regdata &= 0b01111011;//disable both
		 }
		 //del_timer_sync(&obj->first_read_als_timer);
	}
	

	if(hwmsen_write_byte(client,REG_CMD_1,regdata))
	{
		APS_LOG("orap29044_enable_als failed!\n");
		return -1;
	}
    obj->als_enable = enable;

#if 0
	if(hwmsen_read_byte_sr(client, APS_CONFIGUATION, &regdata))
	{
		APS_ERR("read APS_CONFIGUATION register err!\n");
		return -1;
	}
	//
	APS_LOG(" after orap29044_enable_als 00h=%x \n",regdata);
#endif

	if(trc & CMC_TRC_DEBUG)
	{
		APS_LOG("enable als (%d)\n", enable);
	}
#endif
	return err;
}
/*----------------------------------------------------------------------------*/
static int orap29044_enable_ps(struct i2c_client *client, bool enable)
{
    APS_LOG(" orap29044_enable_ps %d\n",enable); 
	struct orap29044_priv *obj = i2c_get_clientdata(client);
	int err=0;
	int trc = atomic_read(&obj->trace);
	u8 regdata=0;
	if(enable == obj->ps_enable)
	{
	   return 0;
	}
	
#if  1
	if(hwmsen_read_byte_sr(client, REG_CMD_1, &regdata))
	{
		APS_ERR("read APS_CONFIGUATION register err!\n");
		return -1;
	}

#if 0
	APS_LOG(" orap29044_enable_ps REG_CMD_1=%x \n",regdata); 
//
#endif
	//regdata &= 0b11111100; //first set 00 to clear bit
	
	if(enable == TRUE)//enable ps
	{
	     APS_LOG("first enable ps!\n");
//add at 2013926 by zhu
               wake_lock(&ps_lock);
//add at 2013926 by zhu emd
		//set ps_nf to max, for re-do a calibration when enable ps
		obj->ps_nf = ORAP29044_PS_MAX_NF;
		obj->ps_ht = ORAP29044_PS_MAX_NF + ORAP29044_PS_HT_ADD;
		obj->ps_lt = ORAP29044_PS_MAX_NF + ORAP29044_PS_LT_ADD;
		obj->last_ps_status = 1;
	
		 if(true == obj->als_enable)
		 {
		   regdata |= 0b10000100; //enable both
		   atomic_set(&obj->als_deb_on, 1);
		   atomic_set(&obj->als_deb_end, jiffies+atomic_read(&obj->als_debounce)/(1000/HZ));
		   APS_LOG("PS(1): enable ps both !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		 }
		 if(false == obj->als_enable)
		 {
		   regdata |= 0b10000000; //only enable ps
		   APS_LOG("PS(1): enable ps only !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		 }
		 atomic_set(&obj->ps_deb_on, 1);
		 atomic_set(&obj->ps_deb_end, jiffies+atomic_read(&obj->ps_debounce)/(1000/HZ));
		 set_bit(PROX_FLAG,  &obj->pending_intr);
		 //#ifdef OPEN_EINT
		 obj->eint_work_running=1;
		 schedule_delayed_work(&obj->eint_work,100);
		 //#endif
		 APS_LOG("first enalbe ps set pending interrupt %d\n",obj->pending_intr);
	}
	else//disable ps
	{
//add at 2013926 by zhu
               wake_unlock(&ps_lock);
//add at 2013926 by zhu emd
		 if(true == obj->als_enable)
		 {
		   APS_LOG("PS(0): disable ps only enalbe als !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		   regdata |= 0b00000100;//only enable als
		   atomic_set(&obj->als_deb_on, 1);//add 
		   atomic_set(&obj->als_deb_end, jiffies+atomic_read(&obj->als_debounce)/(1000/HZ));
		   set_bit(ALS_FLAG,  &obj->pending_intr);
		   #ifdef OPEN_EINT
		   schedule_delayed_work(&obj->eint_work,120);
		   #endif
		 }
		 if(false == obj->als_enable)
		 {
		   APS_LOG("PS(0): disable both !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		   regdata &= 0b01111011;//disable both
		 }
		 obj->eint_work_running=0;
		 //del_timer_sync(&obj->first_read_ps_timer);
	}
	

	if(hwmsen_write_byte(client,REG_CMD_1,regdata))
	{
		APS_LOG("orap29044_enable_als failed!\n");
		return -1;
	}
	obj->ps_enable = enable;

	
	if(trc & CMC_TRC_DEBUG)
	{
		APS_LOG("enable ps (%d)\n", enable);
	}
#endif

	return err;
}
/*----------------------------------------------------------------------------*/
static last_time_read_ps(unsigned long data)
{
	u8 *temp;
	u8 err;
	hwm_sensor_data sensor_data;
	APS_FUN(); 
	memset(&sensor_data, 0, sizeof(sensor_data));
	struct orap29044_priv *obj = (struct orap29044_priv *)data;
	if(NULL==obj)
		return NULL;
	
	if(0x0f<=*temp)
	{
		if(err = orap29044_read_ps_data(obj->client, &obj->ps))
	    {
		 	APS_ERR("orap29044 read ps data: %d\n", err);
	    }
	    APS_LOG("orap29044 read ps nodata:0x%x\n",obj->ps);
	    //map and store data to hwm_sensor_data
	    while(-1 == orap29044_get_ps_value(obj, obj->ps))
	    {
		    orap29044_read_ps_data(obj->client, &obj->ps);
		 	msleep(50);
		 	APS_LOG("orap29044 read ps data delay\n");
	     }
	  		sensor_data.values[0] = orap29044_get_ps_value(obj, obj->ps);
	  		sensor_data.value_divide = 1;
	  		sensor_data.status = SENSOR_STATUS_ACCURACY_MEDIUM;
	  		//let up layer to know
	  		APS_LOG("orap29044 read ps data  = %d \n",sensor_data.values[0]);
	  		if(err = hwmsen_get_interrupt_data(ID_PROXIMITY, &sensor_data))
	  		{
				APS_ERR("call hwmsen_get_interrupt_data fail = %d\n", err);
	  		}
		
		}
	
}

/*----------------------------------------------------------------------------*/

static int orap29044_check_intr(struct i2c_client *client) 
{
	struct orap29044_priv *obj = i2c_get_clientdata(client);
	int err;
	u8 data=0;
	u8 temp=0;

	err = hwmsen_read_byte_sr(client,REG_CMD_2,&data);
	APS_LOG("INT flage: = 0x%x\n", data);

	if(err)
	{
		APS_ERR("WARNING: read int status: %d\n", err);
		return 0;
	}
    
	if(data & 0x08)//als
	{
		set_bit(ALS_FLAG, &obj->pending_intr);
		temp=(data&0b11110111);
		APS_LOG("temp flage: = 0x%x\n", temp);
		err=hwmsen_write_byte(client,REG_CMD_2,temp);
		if(err)
		{
			APS_ERR("error: write REG_CMD_2: %d\n", err);
			return 0;
		}
	}
	else
	{
	   clear_bit(ALS_FLAG, &obj->pending_intr);
	}
	
	if(data & 0x80)//ps
	{
		set_bit(PROX_FLAG,  &obj->pending_intr);
		temp=(data&0b01111111);
		APS_LOG("temp flage: = 0x%x\n", temp);
		err=hwmsen_write_byte(client,REG_CMD_2,temp);
		if(err)
		{
			APS_ERR("error: write REG_CMD_2: %d\n", err);
			return 0;
		}
	}
	else
	{
	    clear_bit(PROX_FLAG, &obj->pending_intr);
	}
	
	if(atomic_read(&obj->trace) & CMC_TRC_DEBUG)
	{
		APS_LOG("check intr: 0x%08X\n", obj->pending_intr);
	}
	err = hwmsen_read_byte_sr(client,REG_CMD_2,&data);
	APS_LOG("after :INT flage: = 0x%x\n", data);


	return 0;
}

/*----------------------------------------------------------------------------*/
void orap29044_eint_func(void)
{
	struct orap29044_priv *obj = g_orap29044_ptr;
	APS_LOG("fwq interrupt fuc\n");
	if(!obj)
	{
		return;
	}
	
	schedule_delayed_work(&obj->eint_work,0);
	if(atomic_read(&obj->trace) & CMC_TRC_EINT)
	{
		APS_LOG("eint: als/ps intrs\n");
	}
}
/*----------------------------------------------------------------------------*/
static int intr_flag = 1;
static void orap29044_eint_work(struct work_struct *work)
{
	struct orap29044_priv *obj = (struct orap29044_priv *)container_of(work, struct orap29044_priv, eint_work);
	int err;
	hwm_sensor_data sensor_data;
	
	memset(&sensor_data, 0, sizeof(sensor_data));

	APS_LOG("orap29044_eint_work\n");
	#if 0
	//if(0 == atomic_read(&obj->ps_deb_on)) // first enable do not check interrupt
	{
	   err = orap29044_check_intr(obj->client);
	}
	
	if(err)
	{
		APS_ERR("check intrs: %d\n", err);
	}

    APS_LOG("orap29044_eint_work &obj->pending_intr =0x%x\n",obj->pending_intr);
	
	if((1<<ALS_FLAG) & obj->pending_intr)//CMC_BIT_ALS
	{
	  //get raw data
	  APS_LOG("fwq als INT\n");
	  if(err = orap29044_read_als_data(obj->client, &obj->als))
	  {
		 APS_ERR("orap29044 read als data: %d\n", err);
	  }
	  APS_LOG("orap29044 read als nodata:0x%x\n",obj->als);
	  //map and store data to hwm_sensor_data
	  while(-1 == orap29044_get_als_value(obj, obj->als))
	  {
		 orap29044_read_als_data(obj->client, &obj->als);
		 msleep(50);
	  }
 	  sensor_data.values[0] = orap29044_get_als_value(obj, obj->als);
	  sensor_data.value_divide = 1;
	  sensor_data.status = SENSOR_STATUS_ACCURACY_MEDIUM;
	  //let up layer to know
	  if(err = hwmsen_get_interrupt_data(ID_LIGHT, &sensor_data))
	  {
		APS_ERR("call hwmsen_get_interrupt_data fail = %d\n", err);
	  }
	  
	}
	if((1<<PROX_FLAG) &  obj->pending_intr)
#endif
	{

	  //get raw data
	  APS_LOG("fwq ps INT\n");
	  if(err = orap29044_read_ps_data(obj->client, &obj->ps))
	  {
		 APS_ERR("orap29044 read ps data: %d\n", err);
	  }
	  APS_LOG("orap29044 read ps nodata:0x%x\n",obj->ps);
	  //map and store data to hwm_sensor_data
	  while(-1 == orap29044_get_ps_value(obj, obj->ps))
	  {
		 orap29044_read_ps_data(obj->client, &obj->ps);
		 msleep(50);
		 APS_LOG("orap29044 read ps data delay\n");
	  }
	  sensor_data.values[0] = orap29044_get_ps_value(obj, obj->ps);
	  sensor_data.value_divide = 1;
	  sensor_data.status = SENSOR_STATUS_ACCURACY_MEDIUM;
	  //let up layer to know
	  APS_LOG("orap29044 read ps data  = %d \n",sensor_data.values[0]);
	 // if(err = hwmsen_get_interrupt_data(ID_PROXIMITY, &sensor_data))
	  //{
		//APS_ERR("call hwmsen_get_interrupt_data fail = %d\n", err);
	  //}
	  intr_flag = sensor_data.values[0];
	  if(err = ps_report_interrupt_data(intr_flag)) 	
	  {
			APS_ERR("call hwmsen_get_interrupt_data fail = %d\n", err);
	  }

	  if(obj->eint_work_running==1)
	  {
	 		schedule_delayed_work(&obj->eint_work,5);
	  }
	 #if 0
	  //add by lpp
	init_timer(&obj->end_read_ps_timer);
	obj->end_read_ps_timer.expires	= jiffies + 1000/(1000/HZ);
	obj->end_read_ps_timer.function	= last_time_read_ps;
	obj->end_read_ps_timer.data		= (unsigned long)obj;
	add_timer(&obj->end_read_ps_timer);
	#endif
	}
	
	#ifdef MT6516
	MT6516_EINTIRQUnmask(CUST_EINT_ALS_NUM);      
	#endif     
	#ifdef MT6573
	mt65xx_eint_unmask(CUST_EINT_ALS_NUM);  
	#endif
      #ifdef MT6589
      mt65xx_eint_unmask(CUST_EINT_ALS_NUM);      
      #endif			

}

/*----------------------------------------------------------------------------*/
int orap29044_setup_eint(struct i2c_client *client)
{
	struct orap29044_priv *obj = i2c_get_clientdata(client);        

	g_orap29044_ptr = obj;
	/*configure to GPIO function, external interrupt*/

	#if 0
	mt_set_gpio_mode(GPIO_ALS_EINT_PIN, GPIO_ALS_EINT_PIN_M_EINT);
    	mt_set_gpio_dir(GPIO_ALS_EINT_PIN, GPIO_DIR_IN);
	mt_set_gpio_pull_enable(GPIO_ALS_EINT_PIN, GPIO_PULL_ENABLE);
	mt_set_gpio_pull_select(GPIO_ALS_EINT_PIN, GPIO_PULL_UP);
	#endif
	//mt_set_gpio_dir(GPIO_ALS_EINT_PIN, GPIO_DIR_IN);
	//mt_set_gpio_mode(GPIO_ALS_EINT_PIN, GPIO_ALS_EINT_PIN_M_EINT);
	//mt_set_gpio_pull_enable(GPIO_ALS_EINT_PIN, GPIO_PULL_ENABLE);
	//mt_set_gpio_pull_select(GPIO_ALS_EINT_PIN, GPIO_PULL_UP);

#ifdef MT6516

	MT6516_EINT_Set_Sensitivity(CUST_EINT_ALS_NUM, CUST_EINT_ALS_SENSITIVE);
	MT6516_EINT_Set_Polarity(CUST_EINT_ALS_NUM, CUST_EINT_ALS_POLARITY);
	MT6516_EINT_Set_HW_Debounce(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_CN);
	MT6516_EINT_Registration(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_EN, CUST_EINT_ALS_POLARITY, orap29044_eint_func, 0);
	MT6516_EINTIRQUnmask(CUST_EINT_ALS_NUM);  
#endif
    //
#ifdef MT6573
	
    mt65xx_eint_set_sens(CUST_EINT_ALS_NUM, CUST_EINT_ALS_SENSITIVE);
	mt65xx_eint_set_polarity(CUST_EINT_ALS_NUM, CUST_EINT_ALS_POLARITY);
	mt65xx_eint_set_hw_debounce(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_CN);
	mt65xx_eint_registration(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_EN, CUST_EINT_ALS_POLARITY, orap29044_eint_func, 0);
	mt65xx_eint_unmask(CUST_EINT_ALS_NUM);  
#endif  

#ifdef MT6575
		mt65xx_eint_set_sens(CUST_EINT_ALS_NUM, CUST_EINT_ALS_SENSITIVE);
		mt65xx_eint_set_polarity(CUST_EINT_ALS_NUM, CUST_EINT_ALS_POLARITY);
		mt65xx_eint_set_hw_debounce(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_CN);
		mt65xx_eint_registration(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_EN, CUST_EINT_ALS_POLARITY, orap29044_eint_func, 0);
		mt65xx_eint_unmask(CUST_EINT_ALS_NUM);	
#endif 

#ifdef MT6577
		mt65xx_eint_set_sens(CUST_EINT_ALS_NUM, CUST_EINT_ALS_SENSITIVE);
		mt65xx_eint_set_polarity(CUST_EINT_ALS_NUM, CUST_EINT_ALS_POLARITY);
		mt65xx_eint_set_hw_debounce(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_CN);
		mt65xx_eint_registration(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_EN, CUST_EINT_ALS_POLARITY, orap29044_eint_func, 0);
		mt65xx_eint_unmask(CUST_EINT_ALS_NUM);	
#endif 

#ifdef MT6589
		mt65xx_eint_set_sens(CUST_EINT_ALS_NUM, CUST_EINT_ALS_SENSITIVE);
		mt65xx_eint_set_polarity(CUST_EINT_ALS_NUM, CUST_EINT_ALS_POLARITY);
		mt65xx_eint_set_hw_debounce(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_CN);
		mt65xx_eint_registration(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_EN, CUST_EINT_ALS_POLARITY, orap29044_eint_func, 0);
		mt65xx_eint_unmask(CUST_EINT_ALS_NUM);	
#endif 
#ifdef MT6572
		mt65xx_eint_set_sens(CUST_EINT_ALS_NUM, CUST_EINT_ALS_SENSITIVE);
		mt65xx_eint_set_polarity(CUST_EINT_ALS_NUM, CUST_EINT_ALS_POLARITY);
		mt65xx_eint_set_hw_debounce(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_CN);
		mt65xx_eint_registration(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_EN, CUST_EINT_ALS_POLARITY, orap29044_eint_func, 0);
		mt65xx_eint_unmask(CUST_EINT_ALS_NUM);	
#endif 	
    return 0;
}

/*----------------------------------------------------------------------------*/
static int orap29044_init_client(struct i2c_client *client)
{
	struct orap29044_priv *obj = i2c_get_clientdata(client);
	int err=0;
	APS_LOG("orap29044_init_client.........\r\n");
#ifdef  OPEN_EINT
	if((err = orap29044_setup_eint(client)))
	{
		APS_ERR("setup eint: %d\n", err);
		return err;
	}
#endif
	
	if((err = orap29044_init_device(client)))
	{
		APS_ERR("init dev: %d\n", err);
		return err;
	}
	#if 0 //add by lpp
	//set window loss
	if(err = orap29044_set_window_loss(client,(unsigned int)(obj->als_widow_loss)))
	{
	  APS_ERR("set window loss error %d\n", err);
	  return err;
	}
	//set ps threshold
	if(err = orap29044_set_ps_threshold(client,(unsigned int)(obj->ps_thd_val)))
	{
	  APS_ERR("set ps threshold error %d\n", err);
	  return err;
	}
	#endif
	
	
	return err;
}
/******************************************************************************
 * Sysfs attributes
*******************************************************************************/
static ssize_t orap29044_show_config(struct device_driver *ddri, char *buf)
{
	ssize_t res;
	
	if(!orap29044_obj)
	{
		APS_ERR("orap29044_obj is null!!\n");
		return 0;
	}
	
	res = snprintf(buf, PAGE_SIZE, "(%d %d %d %d %d)\n", 
		atomic_read(&orap29044_obj->i2c_retry), atomic_read(&orap29044_obj->als_debounce), 
		atomic_read(&orap29044_obj->ps_mask), orap29044_obj->ps_thd_val, atomic_read(&orap29044_obj->ps_debounce));     
	return res;    
}
/*----------------------------------------------------------------------------*/
static ssize_t orap29044_store_config(struct device_driver *ddri, char *buf, size_t count)
{
	int retry, als_deb, ps_deb, mask, thres;
	if(!orap29044_obj)
	{
		APS_ERR("orap29044_obj is null!!\n");
		return 0;
	}
	
	if(5 == sscanf(buf, "%d %d %d %d %d", &retry, &als_deb, &mask, &thres, &ps_deb))
	{ 
		atomic_set(&orap29044_obj->i2c_retry, retry);
		atomic_set(&orap29044_obj->als_debounce, als_deb);
		atomic_set(&orap29044_obj->ps_mask, mask);
		orap29044_obj->ps_thd_val= thres;        
		atomic_set(&orap29044_obj->ps_debounce, ps_deb);
	}
	else
	{
		APS_ERR("invalid content: '%s', length = %d\n", buf, count);
	}
	return count;    
}
/*----------------------------------------------------------------------------*/
static ssize_t orap29044_show_trace(struct device_driver *ddri, char *buf)
{
	ssize_t res;
	if(!orap29044_obj)
	{
		APS_ERR("orap29044_obj is null!!\n");
		return 0;
	}

	res = snprintf(buf, PAGE_SIZE, "0x%04X\n", atomic_read(&orap29044_obj->trace));     
	return res;    
}
/*----------------------------------------------------------------------------*/
static ssize_t orap29044_store_trace(struct device_driver *ddri, char *buf, size_t count)
{
    int trace;
    if(!orap29044_obj)
	{
		APS_ERR("orap29044_obj is null!!\n");
		return 0;
	}
	
	if(1 == sscanf(buf, "0x%x", &trace))
	{
		atomic_set(&orap29044_obj->trace, trace);
	}
	else 
	{
		APS_ERR("invalid content: '%s', length = %d\n", buf, count);
	}
	return count;    
}
/*----------------------------------------------------------------------------*/
static ssize_t orap29044_show_als(struct device_driver *ddri, char *buf)
{
	int res;
	u16 dat = 0;
	
	if(!orap29044_obj)
	{
		APS_ERR("orap29044_obj is null!!\n");
		return 0;
	}
	if(res = orap29044_read_als_data(orap29044_obj->client, &orap29044_obj->als))
	{
		return snprintf(buf, PAGE_SIZE, "ERROR: %d\n", res);
	}
	else
	{   dat = orap29044_obj->als ;
		return snprintf(buf, PAGE_SIZE, "0x%X\n", dat);     
	}
}
/*----------------------------------------------------------------------------*/
static ssize_t orap29044_show_ps(struct device_driver *ddri, char *buf)
{
	//ssize_t res;
	int res;
	u8 dat=0;
	if(!orap29044_obj)
	{
		APS_ERR("orap29044_obj is null!!\n");
		return 0;
	}
	
	if(res = orap29044_read_ps_data(orap29044_obj->client, &orap29044_obj->ps))
	{
		return snprintf(buf, PAGE_SIZE, "ERROR: %d\n", res);
	}
	else
	{
	    dat = orap29044_obj->ps;
		return snprintf(buf, PAGE_SIZE, "0x%X\n", dat);     
	}
}
/*----------------------------------------------------------------------------*/
static ssize_t orap29044_show_reg(struct device_driver *ddri, char *buf)
{
	if(!orap29044_obj)
	{
		APS_ERR("orap29044_obj is null!!\n");
		return 0;
	}
	
	/*read*/
	orap29044_dumpReg(orap29044_obj->client);
	
	return 0;
}

/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
static ssize_t orap29044_show_status(struct device_driver *ddri, char *buf)
{
	ssize_t len = 0;
	
	if(!orap29044_obj)
	{
		APS_ERR("orap29044_obj is null!!\n");
		return 0;
	}
	
	if(orap29044_obj->hw)
	{
	
		len += snprintf(buf+len, PAGE_SIZE-len, "CUST: %d, (%d %d)\n", 
			orap29044_obj->hw->i2c_num, orap29044_obj->hw->power_id, orap29044_obj->hw->power_vol);
		
	}
	else
	{
		len += snprintf(buf+len, PAGE_SIZE-len, "CUST: NULL\n");
	}

	#ifdef MT6516
	len += snprintf(buf+len, PAGE_SIZE-len, "EINT: %d (%d %d %d %d)\n", mt_get_gpio_in(GPIO_ALS_EINT_PIN),
				CUST_EINT_ALS_NUM, CUST_EINT_ALS_POLARITY, CUST_EINT_ALS_DEBOUNCE_EN, CUST_EINT_ALS_DEBOUNCE_CN);

	len += snprintf(buf+len, PAGE_SIZE-len, "GPIO: %d (%d %d %d %d)\n",	GPIO_ALS_EINT_PIN, 
				mt_get_gpio_dir(GPIO_ALS_EINT_PIN), mt_get_gpio_mode(GPIO_ALS_EINT_PIN), 
				mt_get_gpio_pull_enable(GPIO_ALS_EINT_PIN), mt_get_gpio_pull_select(GPIO_ALS_EINT_PIN));
	#endif

	len += snprintf(buf+len, PAGE_SIZE-len, "MISC: %d %d\n", atomic_read(&orap29044_obj->als_suspend), atomic_read(&orap29044_obj->ps_suspend));

	return len;
}
/*----------------------------------------------------------------------------*/
static ssize_t orap29044_show_i2c(struct device_driver *ddri, char *buf)
{
/*	ssize_t len = 0;
	u32 base = I2C2_BASE;

	if(!orap29044_obj)
	{
		APS_ERR("orap29044_obj is null!!\n");
		return 0;
	}
	
	len += snprintf(buf+len, PAGE_SIZE-len, "DATA_PORT      = 0x%08X\n", __raw_readl(mt6516_I2C_DATA_PORT    ));
	len += snprintf(buf+len, PAGE_SIZE-len, "SLAVE_ADDR     = 0x%08X\n", __raw_readl(mt6516_I2C_SLAVE_ADDR));
	len += snprintf(buf+len, PAGE_SIZE-len, "INTR_MASK      = 0x%08X\n", __raw_readl(mt6516_I2C_INTR_MASK));
	len += snprintf(buf+len, PAGE_SIZE-len, "INTR_STAT      = 0x%08X\n", __raw_readl(mt6516_I2C_INTR_STAT));
	len += snprintf(buf+len, PAGE_SIZE-len, "CONTROL        = 0x%08X\n", __raw_readl(mt6516_I2C_CONTROL));
	len += snprintf(buf+len, PAGE_SIZE-len, "TRANSFER_LEN   = 0x%08X\n", __raw_readl(mt6516_I2C_TRANSFER_LEN));
	len += snprintf(buf+len, PAGE_SIZE-len, "TRANSAC_LEN    = 0x%08X\n", __raw_readl(mt6516_I2C_TRANSAC_LEN));
	len += snprintf(buf+len, PAGE_SIZE-len, "DELAY_LEN      = 0x%08X\n", __raw_readl(mt6516_I2C_DELAY_LEN));
	len += snprintf(buf+len, PAGE_SIZE-len, "TIMING         = 0x%08X\n", __raw_readl(mt6516_I2C_TIMING));
	len += snprintf(buf+len, PAGE_SIZE-len, "START          = 0x%08X\n", __raw_readl(mt6516_I2C_START));
	len += snprintf(buf+len, PAGE_SIZE-len, "FIFO_STAT      = 0x%08X\n", __raw_readl(mt6516_I2C_FIFO_STAT));
	len += snprintf(buf+len, PAGE_SIZE-len, "FIFO_THRESH    = 0x%08X\n", __raw_readl(mt6516_I2C_FIFO_THRESH));
	len += snprintf(buf+len, PAGE_SIZE-len, "FIFO_ADDR_CLR  = 0x%08X\n", __raw_readl(mt6516_I2C_FIFO_ADDR_CLR));
	len += snprintf(buf+len, PAGE_SIZE-len, "IO_CONFIG      = 0x%08X\n", __raw_readl(mt6516_I2C_IO_CONFIG));
	len += snprintf(buf+len, PAGE_SIZE-len, "DEBUG          = 0x%08X\n", __raw_readl(mt6516_I2C_DEBUG));
	len += snprintf(buf+len, PAGE_SIZE-len, "HS             = 0x%08X\n", __raw_readl(mt6516_I2C_HS));
	len += snprintf(buf+len, PAGE_SIZE-len, "DEBUGSTAT      = 0x%08X\n", __raw_readl(mt6516_I2C_DEBUGSTAT));
	len += snprintf(buf+len, PAGE_SIZE-len, "DEBUGCTRL      = 0x%08X\n", __raw_readl(mt6516_I2C_DEBUGCTRL));    

	return len;*/
	return 0;
}
/*----------------------------------------------------------------------------*/
static ssize_t orap29044_store_i2c(struct device_driver *ddri, char *buf, size_t count)
{
/*	int sample_div, step_div;
	unsigned long tmp;
	u32 base = I2C2_BASE;    

	if(!orap29044_obj)
	{
		APS_ERR("orap29044_obj is null!!\n");
		return 0;
	}
	else if(2 != sscanf(buf, "%d %d", &sample_div, &step_div))
	{
		APS_ERR("invalid format: '%s'\n", buf);
		return 0;
	}
	tmp  = __raw_readw(mt6516_I2C_TIMING) & ~((0x7 << 8) | (0x1f << 0));
	tmp  = (sample_div & 0x7) << 8 | (step_div & 0x1f) << 0 | tmp;
	__raw_writew(tmp, mt6516_I2C_TIMING);        

	return count;
	*/
	return 0;
}
/*----------------------------------------------------------------------------*/
#define IS_SPACE(CH) (((CH) == ' ') || ((CH) == '\n'))
/*----------------------------------------------------------------------------*/
static int read_int_from_buf(struct orap29044_priv *obj, const char* buf, size_t count,
                             u32 data[], int len)
{
	int idx = 0;
	char *cur = (char*)buf, *end = (char*)(buf+count);

	while(idx < len)
	{
		while((cur < end) && IS_SPACE(*cur))
		{
			cur++;        
		}

		if(1 != sscanf(cur, "%d", &data[idx]))
		{
			break;
		}

		idx++; 
		while((cur < end) && !IS_SPACE(*cur))
		{
			cur++;
		}
	}
	return idx;
}
/*----------------------------------------------------------------------------*/
static ssize_t orap29044_show_alslv(struct device_driver *ddri, char *buf)
{
	ssize_t len = 0;
	int idx;
	if(!orap29044_obj)
	{
		APS_ERR("orap29044_obj is null!!\n");
		return 0;
	}
	
	for(idx = 0; idx < orap29044_obj->als_level_num; idx++)
	{
		len += snprintf(buf+len, PAGE_SIZE-len, "%d ", orap29044_obj->hw->als_level[idx]);
	}
	len += snprintf(buf+len, PAGE_SIZE-len, "\n");
	return len;    
}
/*----------------------------------------------------------------------------*/
static ssize_t orap29044_store_alslv(struct device_driver *ddri, char *buf, size_t count)
{
	struct orap29044_priv *obj;
	if(!orap29044_obj)
	{
		APS_ERR("orap29044_obj is null!!\n");
		return 0;
	}
	else if(!strcmp(buf, "def"))
	{
		memcpy(orap29044_obj->als_level, orap29044_obj->hw->als_level, sizeof(orap29044_obj->als_level));
	}
	else if(orap29044_obj->als_level_num != read_int_from_buf(orap29044_obj, buf, count, 
			orap29044_obj->hw->als_level, orap29044_obj->als_level_num))
	{
		APS_ERR("invalid format: '%s'\n", buf);
	}    
	return count;
}
/*----------------------------------------------------------------------------*/
static ssize_t orap29044_show_alsval(struct device_driver *ddri, char *buf)
{
	ssize_t len = 0;
	int idx;
	if(!orap29044_obj)
	{
		APS_ERR("orap29044_obj is null!!\n");
		return 0;
	}
	
	for(idx = 0; idx < orap29044_obj->als_value_num; idx++)
	{
		len += snprintf(buf+len, PAGE_SIZE-len, "%d ", orap29044_obj->hw->als_value[idx]);
	}
	len += snprintf(buf+len, PAGE_SIZE-len, "\n");
	return len;    
}
/*----------------------------------------------------------------------------*/
static ssize_t orap29044_store_alsval(struct device_driver *ddri, char *buf, size_t count)
{
	if(!orap29044_obj)
	{
		APS_ERR("orap29044_obj is null!!\n");
		return 0;
	}
	else if(!strcmp(buf, "def"))
	{
		memcpy(orap29044_obj->als_value, orap29044_obj->hw->als_value, sizeof(orap29044_obj->als_value));
	}
	else if(orap29044_obj->als_value_num != read_int_from_buf(orap29044_obj, buf, count, 
			orap29044_obj->hw->als_value, orap29044_obj->als_value_num))
	{
		APS_ERR("invalid format: '%s'\n", buf);
	}    
	return count;
}

/*----------------------------------------------------------------------------*/
static DRIVER_ATTR(als,     S_IWUSR | S_IRUGO, orap29044_show_als,   NULL);
static DRIVER_ATTR(ps,      S_IWUSR | S_IRUGO, orap29044_show_ps,    NULL);
static DRIVER_ATTR(config,  S_IWUSR | S_IRUGO, orap29044_show_config,orap29044_store_config);
static DRIVER_ATTR(alslv,   S_IWUSR | S_IRUGO, orap29044_show_alslv, orap29044_store_alslv);
static DRIVER_ATTR(alsval,  S_IWUSR | S_IRUGO, orap29044_show_alsval,orap29044_store_alsval);
static DRIVER_ATTR(trace,   S_IWUSR | S_IRUGO, orap29044_show_trace, orap29044_store_trace);
static DRIVER_ATTR(status,  S_IWUSR | S_IRUGO, orap29044_show_status,  NULL);
static DRIVER_ATTR(reg,     S_IWUSR | S_IRUGO, orap29044_show_reg,   NULL);
static DRIVER_ATTR(i2c,     S_IWUSR | S_IRUGO, orap29044_show_i2c,   orap29044_store_i2c);
/*----------------------------------------------------------------------------*/
static struct device_attribute *orap29044_attr_list[] = {
    &driver_attr_als,
    &driver_attr_ps,    
    &driver_attr_trace,        /*trace log*/
    &driver_attr_config,
    &driver_attr_alslv,
    &driver_attr_alsval,
    &driver_attr_status,
    &driver_attr_i2c,
    &driver_attr_reg,
};
/*----------------------------------------------------------------------------*/
static int orap29044_create_attr(struct device_driver *driver) 
{
	int idx, err = 0;
	int num = (int)(sizeof(orap29044_attr_list)/sizeof(orap29044_attr_list[0]));
	if (driver == NULL)
	{
		return -EINVAL;
	}

	for(idx = 0; idx < num; idx++)
	{
		if(err = driver_create_file(driver, orap29044_attr_list[idx]))
		{            
			APS_ERR("driver_create_file (%s) = %d\n", orap29044_attr_list[idx]->attr.name, err);
			break;
		}
	}    
	return err;
}
/*----------------------------------------------------------------------------*/
	static int orap29044_delete_attr(struct device_driver *driver)
	{
	int idx ,err = 0;
	int num = (int)(sizeof(orap29044_attr_list)/sizeof(orap29044_attr_list[0]));

	if (!driver)
	return -EINVAL;

	for (idx = 0; idx < num; idx++) 
	{
		driver_remove_file(driver, orap29044_attr_list[idx]);
	}
	
	return err;
}
/****************************************************************************** 
 * Function Configuration
******************************************************************************/
static int orap29044_get_als_value(struct orap29044_priv *obj, u16 als)
{
	int idx;
	int invalid = 0;
	//als = als & 0x3f;
	for(idx = 0; idx < obj->als_level_num; idx++)
	{
		if(als < obj->hw->als_level[idx])
		{
			break;
		}
	}
	
	if(idx >= obj->als_value_num)
	{
		APS_ERR("exceed range\n"); 
		idx = obj->als_value_num - 1;
	}
	
	if(1 == atomic_read(&obj->als_deb_on))
	{
		unsigned long endt = atomic_read(&obj->als_deb_end);
		if(time_after(jiffies, endt))
		{
			atomic_set(&obj->als_deb_on, 0);
			//clear_bit(CMC_BIT_ALS, &obj->first_read);
		}
		
		if(1 == atomic_read(&obj->als_deb_on))
		{
			invalid = 1;
		}
	}

	if(!invalid)
	{
		if (atomic_read(&obj->trace) & CMC_TRC_CVT_ALS)
		{
			APS_DBG("ALS: %05d => %05d\n", als, obj->hw->als_value[idx]);
		}
		
		return obj->hw->als_value[idx];
	}
	else
	{
		if(atomic_read(&obj->trace) & CMC_TRC_CVT_ALS)
		{
			APS_DBG("ALS: %05d => %05d (-1)\n", als, obj->hw->als_value[idx]);    
		}
		return -1;
	}
}
/*----------------------------------------------------------------------------*/

static int orap29044_get_ps_value(struct orap29044_priv *obj, u8 ps)
{
  
    int val= -1;
	int invalid = 0;

	/* add by simon */
	if(obj->ps_nf > ps)
	{
		obj->ps_nf = ps;
		if(obj->ps_nf > ORAP29044_PS_MAX_NF)
		{
			obj->ps_nf = ORAP29044_PS_MAX_NF;
			obj->ps_ht = ORAP29044_PS_MAX_NF + ORAP29044_PS_HT_ADD;
			obj->ps_lt = ORAP29044_PS_MAX_NF + ORAP29044_PS_LT_ADD;

		}
		else
		{
			obj->ps_ht = obj->ps_nf + ORAP29044_PS_HT_ADD;
			obj->ps_lt = obj->ps_nf + ORAP29044_PS_LT_ADD;
		}
	}

	if(ps > obj->ps_ht)
	{
		obj->last_ps_status = 0;
	}
	else if(ps < obj->ps_lt)
	{
		obj->last_ps_status = 1;
	}

	APS_DBG("PS_NF:%d, PS_HT:%d, PS_LT:%d, LAST_PS:%d, PS_DAT:%d\n", obj->ps_nf, 
		obj->ps_ht, obj->ps_lt, obj->last_ps_status, ps);

	return obj->last_ps_status;

#if 0
	if(0x80 & ps)
	{
		val = 0;  /*close*/
	}
	else
	{
		val = 1;  /*far away*/
	}
	
	if(atomic_read(&obj->ps_suspend))
	{
		invalid = 1;
	}
	else if(1 == atomic_read(&obj->ps_deb_on))
	{
		unsigned long endt = atomic_read(&obj->ps_deb_end);
		if(time_after(jiffies, endt))
		{
			atomic_set(&obj->ps_deb_on, 0);
			//clear_bit(CMC_BIT_PS, &obj->first_read);
		}
		
		if (1 == atomic_read(&obj->ps_deb_on))
		{
			invalid = 1;
		}
	}

	if(!invalid)
	{
		if(unlikely(atomic_read(&obj->trace) & CMC_TRC_CVT_PS))
		{
		   APS_DBG("PS:  %05d => %05d\n", ps, val);
		}
		return val;
		
	}	
	else
	{
		if(unlikely(atomic_read(&obj->trace) & CMC_TRC_CVT_PS))
		{
			APS_DBG("PS:  %05d => %05d (-1)\n", ps, val);    
		}
		return -1;
	}	
#endif

}

/****************************************************************************** 
 * Function Configuration
******************************************************************************/
static int orap29044_open(struct inode *inode, struct file *file)
{
	file->private_data = orap29044_i2c_client;

	if (!file->private_data)
	{
		APS_ERR("null pointer!!\n");
		return -EINVAL;
	}
	
	return nonseekable_open(inode, file);
}
/*----------------------------------------------------------------------------*/
static int orap29044_release(struct inode *inode, struct file *file)
{
	file->private_data = NULL;
	return 0;
}
/*----------------------------------------------------------------------------*/
static int orap29044_unlocked_ioctl(struct file *file, unsigned int cmd,
       unsigned long arg)
{
	struct i2c_client *client = (struct i2c_client*)file->private_data;
	struct orap29044_priv *obj = i2c_get_clientdata(client);  
	int err = 0;
	void __user *ptr = (void __user*) arg;
	int dat;
	uint32_t enable;

	switch (cmd)
	{
		case ALSPS_SET_PS_MODE:
			if(copy_from_user(&enable, ptr, sizeof(enable)))
			{
				err = -EFAULT;
				goto err_out;
			}
			if(enable)
			{
				if(err = orap29044_enable_ps(obj->client, true))
				{
					APS_ERR("enable ps fail: %d\n", err); 
					goto err_out;
				}
				
				set_bit(CMC_BIT_PS, &obj->enable);
			}
			else
			{
				if(err = orap29044_enable_ps(obj->client, false))
				{
					APS_ERR("disable ps fail: %d\n", err); 
					goto err_out;
				}
				
				clear_bit(CMC_BIT_PS, &obj->enable);
			}
			break;

		case ALSPS_GET_PS_MODE:
			enable = test_bit(CMC_BIT_PS, &obj->enable) ? (1) : (0);
			if(copy_to_user(ptr, &enable, sizeof(enable)))
			{
				err = -EFAULT;
				goto err_out;
			}
			break;

		case ALSPS_GET_PS_DATA:    
			if(err = orap29044_read_ps_data(obj->client, &obj->ps))
			{
				goto err_out;
			}
			dat = orap29044_get_ps_value(obj, obj->ps);
			if(copy_to_user(ptr, &dat, sizeof(dat)))
			{
				err = -EFAULT;
				goto err_out;
			}  
			break;

		case ALSPS_GET_PS_RAW_DATA:    
			if(err = orap29044_read_ps_data(obj->client, &obj->ps))
			{
				goto err_out;
			}
			
			dat = obj->ps;// & 0x80; // add by lpp for factory mode
			if(copy_to_user(ptr, &dat, sizeof(dat)))
			{
				err = -EFAULT;
				goto err_out;
			}  
			break;            

		case ALSPS_SET_ALS_MODE:
			if(copy_from_user(&enable, ptr, sizeof(enable)))
			{
				err = -EFAULT;
				goto err_out;
			}
			if(enable)
			{
				if(err = orap29044_enable_als(obj->client, true))
				{
					APS_ERR("enable als fail: %d\n", err); 
					goto err_out;
				}
				set_bit(CMC_BIT_ALS, &obj->enable);
			}
			else
			{
				if(err = orap29044_enable_als(obj->client, false))
				{
					APS_ERR("disable als fail: %d\n", err); 
					goto err_out;
				}
				clear_bit(CMC_BIT_ALS, &obj->enable);
			}
			break;

		case ALSPS_GET_ALS_MODE:
			enable = test_bit(CMC_BIT_ALS, &obj->enable) ? (1) : (0);
			if(copy_to_user(ptr, &enable, sizeof(enable)))
			{
				err = -EFAULT;
				goto err_out;
			}
			break;

		case ALSPS_GET_ALS_DATA: 
			if(err = orap29044_read_als_data(obj->client, &obj->als))
			{
				goto err_out;
			}

			dat = orap29044_get_als_value(obj, obj->als);
			if(copy_to_user(ptr, &dat, sizeof(dat)))
			{
				err = -EFAULT;
				goto err_out;
			}              
			break;

		case ALSPS_GET_ALS_RAW_DATA:    
			if(err = orap29044_read_als_data(obj->client, &obj->als))
			{
				goto err_out;
			}

			dat = obj->als;// & 0x3f; //add by lpp for factory mode
			if(copy_to_user(ptr, &dat, sizeof(dat)))
			{
				err = -EFAULT;
				goto err_out;
			}              
			break;

		default:
			//APS_ERR("%s not supported = 0x%04x", __FUNCTION__, cmd);
			err = -ENOIOCTLCMD;
			break;
	}

	err_out:
	return err;    
}
/*----------------------------------------------------------------------------*/
static struct file_operations orap29044_fops = {
//	.owner = THIS_MODULE,
	.open = orap29044_open,
	.release = orap29044_release,
	.unlocked_ioctl = orap29044_unlocked_ioctl,
};
/*----------------------------------------------------------------------------*/
static struct miscdevice orap29044_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "als_ps",
	.fops = &orap29044_fops,
};
/*----------------------------------------------------------------------------*/
static int orap29044_i2c_suspend(struct i2c_client *client, pm_message_t msg) 
{
	struct orap29044_priv *obj = i2c_get_clientdata(client);    
	int err;
	APS_FUN();    
//add at 20130910 by zhu
      /*  if(obj->ps_enable)
        {
             return 0;
        }*/
//add at 20130910 by zhu end
#if 0
        if(!obj)
		{
			APS_ERR("null pointer!!\n");
			return -EINVAL;
		}
	if(msg.event == PM_EVENT_SUSPEND)
	{   
		if(!obj)
		{
			APS_ERR("null pointer!!\n");
			return -EINVAL;
		}
		
		atomic_set(&obj->als_suspend, 1);
		if(err = orap29044_enable_als(client, false))
		{
			APS_ERR("disable als: %d\n", err);
			return err;
		}

		atomic_set(&obj->ps_suspend, 1);
		if(err = orap29044_enable_ps(client, false))
		{
			APS_ERR("disable ps:  %d\n", err);
			return err;
		}
		
		orap29044_power(obj->hw, 0);
	}
#endif
	return 0;
}
/*----------------------------------------------------------------------------*/
static int orap29044_i2c_resume(struct i2c_client *client)
{
	struct orap29044_priv *obj = i2c_get_clientdata(client);        
	int err;
	APS_FUN();
#if 0
	if(!obj)
	{
		APS_ERR("null pointer!!\n");
		return -EINVAL;
	}

	orap29044_power(obj->hw, 1);
	if(err = orap29044_init_client(client))
	{
		APS_ERR("initialize client fail!!\n");
		return err;        
	}
	atomic_set(&obj->als_suspend, 0);
	if(test_bit(CMC_BIT_ALS, &obj->enable))
	{
		if(err = orap29044_enable_als(client, true))
		{
			APS_ERR("enable als fail: %d\n", err);        
		}
	}
	atomic_set(&obj->ps_suspend, 0);
	if(test_bit(CMC_BIT_PS,  &obj->enable))
	{
		if(err = orap29044_enable_ps(client, true))
		{
			APS_ERR("enable ps fail: %d\n", err);                
		}
	}
#endif
	return 0;
}
/*----------------------------------------------------------------------------*/
static void orap29044_early_suspend(struct early_suspend *h) 
{   /*early_suspend is only applied for ALS*/
	struct orap29044_priv *obj = container_of(h, struct orap29044_priv, early_drv);   
	int err;
	u8 data;
	u8 temp;
	APS_FUN();    
#if 0
	if(!obj)
	{
		APS_ERR("null pointer!!\n");
		return;
	}
	#if 0
	err = hwmsen_read_byte_sr(obj->client,REG_CMD_2,&data);
	APS_LOG("suspend data flage: = 0x%x\n", data);

	if(err)
	{
		APS_ERR("WARNING: read int status: %d\n", err);
		return 0;
	}
	if(data & 0x08)//als
	{
		set_bit(ALS_FLAG, &obj->pending_intr);
		temp=(data&0b11110111);
		APS_LOG("suspend temp flage: = 0x%x\n", temp);
		err=hwmsen_write_byte(obj->client,REG_CMD_2,temp);
		if(err)
		{
			APS_ERR("error: write REG_CMD_2: %d\n", err);
			return 0;
		}
	}
	else
	{
	   clear_bit(ALS_FLAG, &obj->pending_intr);
	}
	atomic_set(&obj->als_suspend, 1);    
	if(err = orap29044_enable_als(obj->client, false))
	{
		APS_ERR("disable als fail: %d\n", err); 
	}
	#endif
#else
        if(!obj)
	{
		APS_ERR("null pointer!!\n");
		return;
	}

	atomic_set(&obj->als_suspend, 1);
	if(test_bit(CMC_BIT_ALS, &obj->enable))
	{
		if(err = orap29044_enable_als(obj->client, 0))
		{
			APS_ERR("disable als fail: %d\n", err); 
		}
	}
	if (test_bit(CMC_BIT_PS, &obj->enable))
	{
		wake_lock(&orap29044_chrg_lock);
		orap29044_lock = 1;
	}
#endif
}
/*----------------------------------------------------------------------------*/
static void orap29044_late_resume(struct early_suspend *h)
{   /*early_suspend is only applied for ALS*/
	struct orap29044_priv *obj = container_of(h, struct orap29044_priv, early_drv);         
	int err;
	APS_FUN();
#if 0
	if(!obj)
	{
		APS_ERR("null pointer!!\n");
		return;
	}
	#if 0
	orap29044_init_device(obj->client);

	atomic_set(&obj->als_suspend, 0);
	if(test_bit(CMC_BIT_ALS, &obj->enable))
	{
		if(err = orap29044_enable_als(obj->client, true))
		{
			APS_ERR("enable als fail: %d\n", err);        

		}
	 }
	#endif
#else
        if(!obj)
	{
		APS_ERR("null pointer!!\n");
		return;
	}

	atomic_set(&obj->als_suspend, 0);
	if(test_bit(CMC_BIT_ALS, &obj->enable))
	{
		if(err = orap29044_enable_als(obj->client, 1))
		{
			APS_ERR("enable als fail: %d\n", err);        

		}
	}
        if (orap29044_lock)
        {
		wake_unlock(&orap29044_chrg_lock);
		orap29044_lock = 0;
        }
#endif
}

int orap29044_ps_operate(void* self, uint32_t command, void* buff_in, int size_in,
		void* buff_out, int size_out, int* actualout)
{
	int err = 0;
	int value;
	hwm_sensor_data* sensor_data;
	struct orap29044_priv *obj = (struct orap29044_priv *)self;
	
	//APS_FUN(f);
	switch (command)
	{
		case SENSOR_DELAY:
			if((buff_in == NULL) || (size_in < sizeof(int)))
			{
				APS_ERR("Set delay parameter error!\n");
				err = -EINVAL;
			}
			// Do nothing
			break;

		case SENSOR_ENABLE:
			if((buff_in == NULL) || (size_in < sizeof(int)))
			{
				APS_ERR("Enable sensor parameter error!\n");
				err = -EINVAL;
			}
			else
			{				
				value = *(int *)buff_in;
				if(value)
				{
					if(err = orap29044_enable_ps(obj->client, true))
					{
						APS_ERR("enable ps fail: %d\n", err); 
						return -1;
					}
					set_bit(CMC_BIT_PS, &obj->enable);
				}
				else
				{
					if(err = orap29044_enable_ps(obj->client, false))
					{
						APS_ERR("disable ps fail: %d\n", err); 
						return -1;
					}
					clear_bit(CMC_BIT_PS, &obj->enable);
				}
			}
			break;

		case SENSOR_GET_DATA:
			//APS_LOG("fwq get ps data !!!!!!\n");
			if((buff_out == NULL) || (size_out< sizeof(hwm_sensor_data)))
			{
				APS_ERR("get sensor data parameter error!\n");
				err = -EINVAL;
			}
			else
			{
				sensor_data = (hwm_sensor_data *)buff_out;				
				
				if(err = orap29044_read_ps_data(obj->client, &obj->ps))
				{
					err = -1;;
				}
				else
				{
				    while(-1 == orap29044_get_ps_value(obj, obj->ps))
				    {
				      orap29044_read_ps_data(obj->client, &obj->ps);
				      msleep(50);
				    }
				   
					sensor_data->values[0] = orap29044_get_ps_value(obj, obj->ps);
					sensor_data->value_divide = 1;
					sensor_data->status = SENSOR_STATUS_ACCURACY_MEDIUM;
					APS_LOG("fwq get ps data =%d\n",sensor_data->values[0]);
				    
					
				}				
			}
			break;
		default:
			APS_ERR("proxmy sensor operate function no this parameter %d!\n", command);
			err = -1;
			break;
	}
	
	return err;
}

int orap29044_als_operate(void* self, uint32_t command, void* buff_in, int size_in,
		void* buff_out, int size_out, int* actualout)
{
	int err = 0;
	int value;
	hwm_sensor_data* sensor_data;
	struct orap29044_priv *obj = (struct orap29044_priv *)self;
	
	//APS_FUN(f);
	switch (command)
	{
		case SENSOR_DELAY:
			if((buff_in == NULL) || (size_in < sizeof(int)))
			{
				APS_ERR("Set delay parameter error!\n");
				err = -EINVAL;
			}
			// Do nothing
			break;

		case SENSOR_ENABLE:
			if((buff_in == NULL) || (size_in < sizeof(int)))
			{
				APS_ERR("Enable sensor parameter error!\n");
				err = -EINVAL;
			}
			else
			{
				value = *(int *)buff_in;				
				if(value)
				{
					if(err = orap29044_enable_als(obj->client, true))
					{
						APS_ERR("enable als fail: %d\n", err); 
						return -1;
					}
					set_bit(CMC_BIT_ALS, &obj->enable);
				}
				else
				{
					if(err = orap29044_enable_als(obj->client, false))
					{
						APS_ERR("disable als fail: %d\n", err); 
						return -1;
					}
					clear_bit(CMC_BIT_ALS, &obj->enable);
				}
				
			}
			break;

		case SENSOR_GET_DATA:
			//APS_LOG("fwq get als data !!!!!!\n");
			if((buff_out == NULL) || (size_out< sizeof(hwm_sensor_data)))
			{
				APS_ERR("get sensor data parameter error!\n");
				err = -EINVAL;
			}
			else
			{
				sensor_data = (hwm_sensor_data *)buff_out;
								
				if(err = orap29044_read_als_data(obj->client, &obj->als))
				{
					err = -1;;
				}
				else
				{
					APS_LOG("fwq get als nodata =%d\n",obj->als);
				
				    while(-1 == orap29044_get_als_value(obj, obj->als))
				    {
				      orap29044_read_als_data(obj->client, &obj->als);
				      msleep(50);
				    }
					sensor_data->values[0] = orap29044_get_als_value(obj, obj->als);
					sensor_data->value_divide = 1;
					sensor_data->status = SENSOR_STATUS_ACCURACY_MEDIUM;
					//APS_LOG("fwq get als data =%d\n",sensor_data->values[0]);
				}				
			}
			break;
		default:
			APS_ERR("light sensor operate function no this parameter %d!\n", command);
			err = -1;
			break;
	}
	
	return err;
}

// if use  this typ of enable , Gsensor should report inputEvent(x, y, z ,stats, div) to HAL
static int als_open_report_data(int open)
{
	//should queuq work to report event if  is_report_input_direct=true
	return 0;
}

// if use  this typ of enable , Gsensor only enabled but not report inputEvent to HAL

static int als_enable_nodata(int en)
{
	int res = 0;
#ifdef CUSTOM_KERNEL_SENSORHUB
    SCP_SENSOR_HUB_DATA req;
    int len;
#endif //#ifdef CUSTOM_KERNEL_SENSORHUB

    APS_LOG("orap29044_obj als enable value = %d\n", en);

#ifdef CUSTOM_KERNEL_SENSORHUB
    if (atomic_read(&orap29044_obj->init_done))
    {
        req.activate_req.sensorType = ID_LIGHT;
        req.activate_req.action = SENSOR_HUB_ACTIVATE;
        req.activate_req.enable = en;
        len = sizeof(req.activate_req);
        res = SCP_sensorHub_req_send(&req, &len, 1);
    }
    else
    {
        APS_ERR("sensor hub has not been ready!!\n");
    }
 	mutex_lock(&orap29044_mutex);
	if (en)
		set_bit(CMC_BIT_ALS, &orap29044_obj->enable);
	else
		clear_bit(CMC_BIT_ALS, &orap29044_obj->enable);
	mutex_unlock(&orap29044_mutex);
#else //#ifdef CUSTOM_KERNEL_SENSORHUB
	mutex_lock(&orap29044_mutex);
	if (en)
		set_bit(CMC_BIT_ALS, &orap29044_obj->enable);
	else
		clear_bit(CMC_BIT_ALS, &orap29044_obj->enable);
	mutex_unlock(&orap29044_mutex);
	if(!orap29044_obj)
	{
		APS_ERR("orap29044_obj is null!!\n");
		return -1;
	}
	res=orap29044_enable_als(orap29044_obj->client, en);
#endif //#ifdef CUSTOM_KERNEL_SENSORHUB
	if(res){
		APS_ERR("als_enable_nodata is failed!!\n");
		return -1;
	}
	return 0;
}

static int als_set_delay(u64 ns)
{
	return 0;
}

static int als_get_data(int* value, int* status)
{
	int err = 0;
#ifdef CUSTOM_KERNEL_SENSORHUB
    SCP_SENSOR_HUB_DATA req;
    int len;
#else
    struct orap29044_priv *obj = NULL;
#endif //#ifdef CUSTOM_KERNEL_SENSORHUB

#ifdef CUSTOM_KERNEL_SENSORHUB
    if (atomic_read(&orap29044_obj->init_done))
    {
        req.get_data_req.sensorType = ID_LIGHT;
        req.get_data_req.action = SENSOR_HUB_GET_DATA;
        len = sizeof(req.get_data_req);
        err = SCP_sensorHub_req_send(&req, &len, 1);
        if (err)
        {
            APS_ERR("SCP_sensorHub_req_send fail!\n");
        }
        else
        {
            *value = req.get_data_rsp.int16_Data[0];
            *status = SENSOR_STATUS_ACCURACY_MEDIUM;
        }

        if(atomic_read(&orap29044_obj->trace) & CMC_TRC_PS_DATA)
        {
            APS_LOG("value = %d\n", *value);
            //show data
        }
    }
    else
    {
        APS_ERR("sensor hub hat not been ready!!\n");
        err = -1;
    }
#else //#ifdef CUSTOM_KERNEL_SENSORHUB
	if(!orap29044_obj)
	{
		APS_ERR("orap29044_obj is null!!\n");
		return -1;
	}
	obj = orap29044_obj;
	if((err = orap29044_read_als_data(obj->client, &obj->als)))
	{
		err = -1;
	}
	else
	{
		*value = orap29044_get_als_value(obj, obj->als);
		*status = SENSOR_STATUS_ACCURACY_MEDIUM;
	}
#endif //#ifdef CUSTOM_KERNEL_SENSORHUB

	return err;
}

// if use  this typ of enable , Gsensor should report inputEvent(x, y, z ,stats, div) to HAL
static int ps_open_report_data(int open)
{
	//should queuq work to report event if  is_report_input_direct=true
	return 0;
}

// if use  this typ of enable , Gsensor only enabled but not report inputEvent to HAL

static int ps_enable_nodata(int en)
{
	int res = 0;
#ifdef CUSTOM_KERNEL_SENSORHUB
    SCP_SENSOR_HUB_DATA req;
    int len;
#endif //#ifdef CUSTOM_KERNEL_SENSORHUB

    APS_LOG("orap29044_obj als enable value = %d\n", en);

#ifdef CUSTOM_KERNEL_SENSORHUB
    if (atomic_read(&orap29044_obj->init_done))
    {
        req.activate_req.sensorType = ID_PROXIMITY;
        req.activate_req.action = SENSOR_HUB_ACTIVATE;
        req.activate_req.enable = en;
        len = sizeof(req.activate_req);
        res = SCP_sensorHub_req_send(&req, &len, 1);
    }
    else
    {
        APS_ERR("sensor hub has not been ready!!\n");
    }
    mutex_lock(&orap29044_mutex);
    if (en)
        set_bit(CMC_BIT_PS, &orap29044_obj->enable);
    else
        clear_bit(CMC_BIT_PS, &orap29044_obj->enable);
    mutex_unlock(&orap29044_mutex);
#else //#ifdef CUSTOM_KERNEL_SENSORHUB
	mutex_lock(&orap29044_mutex);
	if (en)
		set_bit(CMC_BIT_PS, &orap29044_obj->enable);

	else
		clear_bit(CMC_BIT_PS, &orap29044_obj->enable);

	mutex_unlock(&orap29044_mutex);
	if(!orap29044_obj)
	{
		APS_ERR("orap29044_obj is null!!\n");
		return -1;
	}
	res=	orap29044_enable_ps(orap29044_obj->client, en);
#endif //#ifdef CUSTOM_KERNEL_SENSORHUB
    
	if(res){
		APS_ERR("als_enable_nodata is failed!!\n");
		return -1;
	}
	return 0;

}

static int ps_set_delay(u64 ns)
{
	return 0;
}

static int ps_get_data(int* value, int* status)
{
    int err = 0;
#ifdef CUSTOM_KERNEL_SENSORHUB
    SCP_SENSOR_HUB_DATA req;
    int len;
#endif //#ifdef CUSTOM_KERNEL_SENSORHUB

#ifdef CUSTOM_KERNEL_SENSORHUB
    if (atomic_read(&orap29044_obj->init_done))
    {
        req.get_data_req.sensorType = ID_PROXIMITY;
        req.get_data_req.action = SENSOR_HUB_GET_DATA;
        len = sizeof(req.get_data_req);
        err = SCP_sensorHub_req_send(&req, &len, 1);
        if (err)
        {
            APS_ERR("SCP_sensorHub_req_send fail!\n");
            *value = -1;
            err = -1;
        }
        else
        {
            *value = req.get_data_rsp.int16_Data[0];
            *status = SENSOR_STATUS_ACCURACY_MEDIUM;
        }

        if(atomic_read(&orap29044_obj->trace) & CMC_TRC_PS_DATA)
        {
            APS_LOG("value = %d\n", *value);
            //show data
        }
    }
    else
    {
        APS_ERR("sensor hub has not been ready!!\n");
        err = -1;
    }
#else //#ifdef CUSTOM_KERNEL_SENSORHUB
    if(!orap29044_obj)
	{
		APS_ERR("orap29044_obj is null!!\n");
		return -1;
	}
    
    if((err = orap29044_read_ps_data(orap29044_obj->client, &orap29044_obj->ps)))
    {
        err = -1;;
    }
    else
    {
        *value = orap29044_get_ps_value(orap29044_obj, orap29044_obj->ps);
        *status = SENSOR_STATUS_ACCURACY_MEDIUM;
    }
#endif //#ifdef CUSTOM_KERNEL_SENSORHUB
    
	return 0;
}


/*----------------------------------------------------------------------------*/
static int orap29044_i2c_detect(struct i2c_client *client, int kind, struct i2c_board_info *info) 
{    
	strcpy(info->type, ORAP29044_DEV_NAME);
	return 0;
}

/*----------------------------------------------------------------------------*/
 static int orap29044_init_flag =0;
static int orap29044_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
      
	struct orap29044_priv *obj;
	//struct hwmsen_object obj_ps, obj_als;
	int err = 0;
	u8 buf;
	struct als_control_path als_ctl;
	struct als_data_path als_data;
	struct ps_control_path ps_ctl;
	struct ps_data_path ps_data;
	APS_ERR("enter orap29044_i2c_probe is ok!!!!!!");

	if(!(obj = kzalloc(sizeof(*obj), GFP_KERNEL)))
	{
		err = -ENOMEM;
		goto exit;
	}
	memset(obj, 0, sizeof(*obj));
	orap29044_obj = obj;

	obj->hw = Orap29044_get_cust_alsps_hw();

	if(1 != obj->hw->polling_mode_ps)
	{
		INIT_DELAYED_WORK(&obj->eint_work, orap29044_eint_work);
	}
	obj->client = client;
	//while(1)
	  i2c_set_clientdata(client, obj);	
	atomic_set(&obj->als_debounce, 1000);
	atomic_set(&obj->als_deb_on, 0);
	atomic_set(&obj->als_deb_end, 0);
	atomic_set(&obj->ps_debounce, 1000);
	atomic_set(&obj->ps_deb_on, 0);
	atomic_set(&obj->ps_deb_end, 0);
	atomic_set(&obj->ps_mask, 0);
	atomic_set(&obj->trace, 0x00);
	atomic_set(&obj->als_suspend, 0);
	obj->ps_nf = ORAP29044_PS_MAX_NF;
	obj->ps_ht = ORAP29044_PS_MAX_NF + ORAP29044_PS_HT_ADD;
	obj->ps_lt = ORAP29044_PS_MAX_NF + ORAP29044_PS_LT_ADD;
	obj->last_ps_status = 1;
	obj->eint_work_running=0;

	obj->ps_enable = 0;
	obj->als_enable = 0;
	obj->enable = 0;
	obj->pending_intr = 0;
	obj->als_level_num = sizeof(obj->hw->als_level)/sizeof(obj->hw->als_level[0]);
	obj->als_value_num = sizeof(obj->hw->als_value)/sizeof(obj->hw->als_value[0]);   
	BUG_ON(sizeof(obj->als_level) != sizeof(obj->hw->als_level));
	memcpy(obj->als_level, obj->hw->als_level, sizeof(obj->als_level));
	BUG_ON(sizeof(obj->als_value) != sizeof(obj->hw->als_value));
	memcpy(obj->als_value, obj->hw->als_value, sizeof(obj->als_value));
	atomic_set(&obj->i2c_retry, 3);
    //pre set ps threshold
	obj->ps_thd_val = obj->hw->ps_threshold;
	//pre set window loss
    obj->als_widow_loss = obj->hw->als_window_loss;
	
	orap29044_i2c_client = client;

	
	if(err = orap29044_init_client(client))
	{
		goto exit_init_failed;
	}
	
	if(err = misc_register(&orap29044_device))
	{
		APS_ERR("orap29044_device register failed\n");
		goto exit_misc_device_register_failed;
	}
#ifdef MTK_AUTO_DETECT_ALSPS
#else
	//if(err = orap29044_create_attr(&orap29044_alsps_driver.driver))
	{
	//	APS_ERR("create attribute err = %d\n", err);
	//	goto exit_create_attr_failed;
	}
#endif
	als_ctl.is_use_common_factory =false;
	ps_ctl.is_use_common_factory = false;
	APS_LOG("orap29044_device misc_register OK!\n");

	/*------------------------orap29044 attribute file for debug--------------------------------------*/
	als_ctl.open_report_data= als_open_report_data;
	als_ctl.enable_nodata = als_enable_nodata;
	als_ctl.set_delay  = als_set_delay;
	als_ctl.is_report_input_direct = false;
#ifdef CUSTOM_KERNEL_SENSORHUB
	als_ctl.is_support_batch = obj->hw->is_batch_supported_als;
#else
    als_ctl.is_support_batch = false;
#endif
	
	err = als_register_control_path(&als_ctl);
	if(err)
	{
		APS_ERR("register fail = %d\n", err);
		goto exit_sensor_obj_attach_fail;
	}

	als_data.get_data = als_get_data;
	als_data.vender_div = 100;
	err = als_register_data_path(&als_data);	
	if(err)
	{
		APS_ERR("tregister fail = %d\n", err);
		goto exit_sensor_obj_attach_fail;
	}

	
	ps_ctl.open_report_data= ps_open_report_data;
	ps_ctl.enable_nodata = ps_enable_nodata;
	ps_ctl.set_delay  = ps_set_delay;
	ps_ctl.is_report_input_direct = false;
#ifdef CUSTOM_KERNEL_SENSORHUB
	ps_ctl.is_support_batch = obj->hw->is_batch_supported_ps;
#else
    ps_ctl.is_support_batch = false;
#endif
	
	err = ps_register_control_path(&ps_ctl);
	if(err)
	{
		APS_ERR("register fail = %d\n", err);
		goto exit_sensor_obj_attach_fail;
	}

	ps_data.get_data = ps_get_data;
	ps_data.vender_div = 100;
	err = ps_register_data_path(&ps_data);	
	if(err)
	{
		APS_ERR("tregister fail = %d\n", err);
		goto exit_sensor_obj_attach_fail;
	}

	err = batch_register_support_info(ID_LIGHT,als_ctl.is_support_batch, 1, 0);
	if(err)
	{
		APS_ERR("register light batch support err = %d\n", err);
	}
	
	err = batch_register_support_info(ID_PROXIMITY,ps_ctl.is_support_batch, 1, 0);
	if(err)
	{
		APS_ERR("register proximity batch support err = %d\n", err);
	}

#if 0
	obj_ps.self = orap29044_obj;
	if(1 == obj->hw->polling_mode_ps)
	{
	  obj_ps.polling = 1;
	}
	else
	{
	  obj_ps.polling = 0;//interrupt mode
	}
	obj_ps.sensor_operate = orap29044_ps_operate;
	if(err = hwmsen_attach(ID_PROXIMITY, &obj_ps))
	{
		APS_ERR("attach fail = %d\n", err);
		goto exit_create_attr_failed;
	}
	
	obj_als.self = orap29044_obj;
	if(1 == obj->hw->polling_mode_als)
	{
	  obj_als.polling = 1;
	  APS_ERR("polling mode\n");
	}
	else
	{
	  obj_als.polling = 0;//interrupt mode
	  APS_ERR("interrupt mode\n");
	}
	obj_als.sensor_operate = orap29044_als_operate;
	if(err = hwmsen_attach(ID_LIGHT, &obj_als))
	{
		APS_ERR("attach fail = %d\n", err);
		goto exit_create_attr_failed;
	}
#endif

#if defined(CONFIG_HAS_EARLYSUSPEND)
	obj->early_drv.level    = EARLY_SUSPEND_LEVEL_DISABLE_FB - 1,
	obj->early_drv.suspend  = orap29044_early_suspend,
	obj->early_drv.resume   = orap29044_late_resume,    
	register_early_suspend(&obj->early_drv);
        wake_lock_init(&orap29044_chrg_lock, WAKE_LOCK_SUSPEND, "orap29044_wake_lock");
#endif

	orap29044_init_flag = 0;
	return 0;

	exit_create_attr_failed:
	exit_sensor_obj_attach_fail:
	exit_misc_device_register_failed:
	misc_deregister(&orap29044_device);
	exit_init_failed:
	//i2c_detach_client(client);
	exit_kfree:
	kfree(obj);
	exit:
	orap29044_i2c_client = NULL;           
	#ifdef MT6516        
	MT6516_EINTIRQMask(CUST_EINT_ALS_NUM);  /*mask interrupt if fail*/
	#endif
	APS_ERR("%s: err = %d\n", __func__, err);
	orap29044_init_flag = -1;
	return err;
}
/*----------------------------------------------------------------------------*/
static int orap29044_i2c_remove(struct i2c_client *client)
{
	int err;	
	
	if(err = orap29044_delete_attr(&orap29044_i2c_driver.driver))
	{
		APS_ERR("orap29044_delete_attr fail: %d\n", err);
	} 

	if(err = misc_deregister(&orap29044_device))
	{
		APS_ERR("misc_deregister fail: %d\n", err);    
	}
	
	orap29044_i2c_client = NULL;
	i2c_unregister_device(client);
	kfree(i2c_get_clientdata(client));

	return 0;
}
/*----------------------------------------------------------------------------*/
//#ifdef 1//MTK_AUTO_DETECT_ALSPS
#if 1//MTK_AUTO_DETECT_ALSPS

static int orap29044_local_init(void) 
#else
static int orap29044_probe(struct platform_device *pdev) 
#endif
{
	struct alsps_hw *hw = Orap29044_get_cust_alsps_hw();
	//struct orap29044_i2c_addr addr;
	APS_ERR("enter \n");
	
	orap29044_power(hw, 1);    
//add at 20130926 by zhu
        wake_lock_init(&ps_lock,WAKE_LOCK_SUSPEND,"orap29044ps wakelock");
//add at 20130926 by zhu end 
	//orap29044_get_addr(hw, &addr);
//	orap29044_force[0] = hw->i2c_num;
	//orap29044_force[1] = addr.init;
	if(i2c_add_driver(&orap29044_i2c_driver))
	{
		APS_ERR("add driver error\n");
		return -1;
	} 
     printk("ALSPS-->orap29044_init_flag = %d\n",orap29044_init_flag);
	if(-1 == orap29044_init_flag)
	{
	   return -1;
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
//static int orap29044_remove(struct platform_device *pdev)
static int orap29044_remove(void)

{
	struct alsps_hw *hw = Orap29044_get_cust_alsps_hw();
	APS_FUN();    
	orap29044_power(hw, 0);    	 
	i2c_del_driver(&orap29044_i2c_driver);
	return 0;
}
static int orap29044_local_init(void);
//#ifdef 1//MTK_AUTO_DETECT_ALSPS
#if 1//MTK_AUTO_DETECT_ALSPS

 static struct sensor_init_info orap29044_init_info = {
		 .name = ORAP29044_DEV_NAME,
		 .init = orap29044_local_init,//orap29044_probe,
		 .uninit = orap29044_remove,
	 
 };
#else
/*----------------------------------------------------------------------------*/
static struct platform_driver orap29044_alsps_driver = {
	.probe      = orap29044_probe,
	.remove     = orap29044_remove,    
	.driver     = {
		.name  = "als_ps",
 #if (LINUX_VERSION_CODE<KERNEL_VERSION(3,0,0))	
		.owner = THIS_MODULE,
#endif
	}
};
#endif
#if 0//MTK_AUTO_DETECT_ALSPS
 extern int hwmsen_psensor_add(struct sensor_init_info* obj) ;//brian-add
#endif
/*----------------------------------------------------------------------------*/
static int __init orap29044_init(void)
{
	APS_FUN();
	APS_ERR("enter to orap29044_init driver");
	struct alsps_hw *hw = Orap29044_get_cust_alsps_hw();
	//struct i2c_board_info i2c_orap29044={ I2C_BOARD_INFO(CM36652_DEV_NAME,hw->i2c_addr[0] )};
	struct i2c_board_info i2c_orap29044={ I2C_BOARD_INFO(ORAP29044_DEV_NAME,hw->i2c_addr[0] )};
   #if (LINUX_VERSION_CODE>=KERNEL_VERSION(3,0,0))	
	i2c_register_board_info(hw->i2c_num,&i2c_ORAP29044,1);
   #endif
   alsps_driver_add(&orap29044_init_info);
 #if 0
   #ifdef MTK_AUTO_DETECT_ALSPS
    hwmsen_psensor_add(&orap29044_init_info);
   #else
	if(platform_driver_register(&orap29044_alsps_driver))
	{

		APS_ERR("failed to register driver");
		return -ENODEV;
	}
  #endif
 #endif
	return 0;
}
/*----------------------------------------------------------------------------*/
static void __exit orap29044_exit(void)
{
	APS_FUN();
	#ifdef MTK_AUTO_DETECT_ALSPS
	#else
	platform_driver_unregister(&orap29044_alsps_driver);
	#endif
}
/*----------------------------------------------------------------------------*/
module_init(orap29044_init);
module_exit(orap29044_exit);
/*----------------------------------------------------------------------------*/
MODULE_AUTHOR("PERRA LIU");
MODULE_DESCRIPTION("ORAP29044 light&proxiy driver");
MODULE_LICENSE("GPL");
