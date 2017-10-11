
#ifdef BUILD_LK
#include <string.h>
#include <mt_gpio.h>
#else
#include <linux/string.h>
#if defined(BUILD_UBOOT)
#include <asm/arch/mt_gpio.h>
#else
#include <mach/mt_gpio.h>
#include <mach/mt_pm_ldo.h>
#endif
#endif
#include "lcm_drv.h"
#include <cust_gpio_usage.h>


#if defined(BUILD_LK)
#define LCM_PRINT printf
#elif defined(BUILD_UBOOT)
#define LCM_PRINT printf
#else
#define LCM_PRINT printk
#endif

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH (480) // pixel
#define FRAME_HEIGHT (854)// (800) // pixel

#define REGFLAG_DELAY 0xAB
#define REGFLAG_END_OF_TABLE 0xAA // END OF REGISTERS MARKER

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v) (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

extern void sub_power(int val);

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd) lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums) lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd) lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size) lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)






// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------
static void lcm_set_util_funcs(const LCM_UTIL_FUNCS * util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS * params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type = LCM_TYPE_DSI;

	params->width = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	// enable tearing-free
	params->dbi.te_mode = LCM_DBI_TE_MODE_DISABLED;
	params->dbi.te_edge_polarity = LCM_POLARITY_RISING;

	params->dsi.mode = SYNC_PULSE_VDO_MODE;

	// DSI
	/* Command mode setting */
	params->dsi.LANE_NUM = LCM_TWO_LANE;

	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format = LCM_DSI_FORMAT_RGB888;

	// Highly depends on LCD driver capability.
	// Not support in MT6573
	params->dsi.packet_size = 256;

	// Video mode setting	
	params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;

	params->dsi.vertical_sync_active = 4;
	params->dsi.vertical_backporch = 7;
	params->dsi.vertical_frontporch = 6;
	params->dsi.vertical_active_line = FRAME_HEIGHT; 

	params->dsi.horizontal_sync_active = 10;//3;
	params->dsi.horizontal_backporch = 10;
	params->dsi.horizontal_frontporch = 10;
	params->dsi.horizontal_active_pixel = FRAME_WIDTH;

	// Bit rate calculation
	//params->dsi.pll_div1 = 29; // fref=26MHz, fvco=fref*(div1+1)	(div1=0~63, fvco=500MHZ~1GHz)
	//params->dsi.pll_div2 = 1; // div2=0~15: fout=fvo/(2*div2)
	//params->dsi.fbk_div = 434;


	params->dsi.PLL_CLOCK = 182; //260; ///////params->dsi.PLL_CLOCK = 234����ʾ234MHZ
}

static void init_lcm_registers(void)
{
	unsigned int data_array[16];

	data_array[0] = 0x00043902; //  Password   
	data_array[1] = 0xf26191bf;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902; //VCOM 
	data_array[1] = 0x004c00b3;//2d
	
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902; //VCOM_R   
	data_array[1] = 0x006c00b4;//2d
	
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00073902; ////VGMP, VGSP, VGMN, VGSN 
	data_array[1] = 0x019f00b8;
	data_array[2] = 0x00019f00;
		 
	dsi_set_cmdq(data_array, 3, 1);

	
	data_array[0] = 0x00043902; //GIP output voltage level
	data_array[1] = 0x00233eba;
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00023902; //	// SET RGB CYC
	data_array[1] = 0x000004c3;  //�з�ת
	//data_array[1] = 0x000002c3; //�㷴ת//��������ֲ���
	dsi_set_cmdq(data_array, 2, 1);						

	data_array[0] = 0x00033902; //SET TCON  
	data_array[1] = 0x006a30c4;
	
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x000a3902; ///POWER CTRL 
	data_array[1] = 0x310100c7;
	data_array[2] = 0x132a6505;
	data_array[3] = 0x0000a5a5;
	
	dsi_set_cmdq(data_array, 4, 1);
	
	data_array[0] = 0x00273902; //Gamma  
	data_array[1] = 0x53667fc8;
	data_array[2] = 0x2f2c3d44;
	data_array[3] = 0x32333319;
	data_array[4] = 0x30403b4f;
	data_array[5] = 0x000b1b29;
	data_array[6] = 0x4453667f;
	data_array[7] = 0x192f2c3d;
	data_array[8] = 0x4f323333;
	data_array[9] = 0x2930403b;	
	data_array[10] = 0x00000b1b;
		
	dsi_set_cmdq(data_array, 11, 1);



	data_array[0] = 0x00113902; ///CGOUTx_L GS=0  
	data_array[1] = 0x1f1f1fd4;
	data_array[2] = 0x07050103;
	data_array[3] = 0x13110b09;
	data_array[4] = 0x1f1f1f1f;
	data_array[5] = 0x0000001f;
	
	dsi_set_cmdq(data_array, 6, 1);

	data_array[0] = 0x00113902; ///CGOUTx_R GS=0  
	data_array[1] = 0x1f1f1fd5;
	data_array[2] = 0x06040002;
	data_array[3] = 0x12100a08;
	data_array[4] = 0x1f1f1f1f;
	data_array[5] = 0x0000001f;
	
	dsi_set_cmdq(data_array, 6, 1);

	data_array[0] = 0x00113902; ///CGOUTx_L GS=1  
	data_array[1] = 0x1f1f1fd6;
	data_array[2] = 0x0a041210;
	data_array[3] = 0x00020608;
	data_array[4] = 0x1f1f1f1f;
	data_array[5] = 0x0000001f;
	
	dsi_set_cmdq(data_array, 6, 1);
	
	data_array[0] = 0x00113902; ///CGOUTx_R GS=1  
	data_array[1] = 0x1f1f1fd7;
	data_array[2] = 0x0b051311;
	data_array[3] = 0x01030709;
	data_array[4] = 0x1f1f1f1f;
	data_array[5] = 0x0000001f;
	
	dsi_set_cmdq(data_array, 6, 1);
	
	data_array[0] = 0x00153902; //SETGIP1   
	data_array[1] = 0x000020d8;
	data_array[2] = 0x01300330;
	data_array[3] = 0x02013002;
	data_array[4] = 0x5d737006;
	data_array[5] = 0x70060673;
	data_array[6] = 0x00000008;
	dsi_set_cmdq(data_array, 7, 1);
	
	data_array[0] = 0x00143902; //SETGIP2   
	data_array[1] = 0x0a0a00d9;
	data_array[2] = 0x06000088;
	data_array[3] = 0x0080007b;
	data_array[4] = 0x001f333b;
	data_array[5] = 0x70060000;
	//data_array[6] = 0x00000008;
	dsi_set_cmdq(data_array, 6, 1);
	
	data_array[0] = 0x00110500;   // Sleep Out 
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(120);

	data_array[0] = 0x00290500;  // Display On 
	dsi_set_cmdq(data_array, 1, 1);

}

static void lcm_init(void)
{

#ifdef BUILD_UBOOT 
#elif defined(BUILD_LK) 
#else
	hwPowerOn(MT6328_POWER_LDO_VCAMD, VOL_1800, "1V8_LCD_VIO");
	hwPowerOn(MT6328_POWER_LDO_VCAMA, VOL_2800, "2V8_LCD_VCI");
	MDELAY(5);
	hwPowerDown(MT6328_POWER_LDO_VCAMA, "2V8_LCD_VCI");
	MDELAY(5);
	hwPowerOn(MT6328_POWER_LDO_VCAMA, VOL_2800, "2V8_LCD_VCI" );
#endif

	mt_set_gpio_mode(GPIO_LCM_RST, GPIO_MODE_00);
	mt_set_gpio_pull_enable(GPIO_LCM_RST, GPIO_PULL_ENABLE);
	mt_set_gpio_dir(GPIO_LCM_RST, GPIO_DIR_OUT);
	
	mt_set_gpio_out(GPIO_LCM_RST, GPIO_OUT_ONE);
	MDELAY(10);
	mt_set_gpio_out(GPIO_LCM_RST, GPIO_OUT_ZERO);
	MDELAY(10);
	mt_set_gpio_out(GPIO_LCM_RST, GPIO_OUT_ONE);
	MDELAY(100);

	init_lcm_registers();
}

static void lcm_suspend(void)
{

	  unsigned int data_array[16];
	  
	  data_array[0] = 0x00280500;	
	  dsi_set_cmdq(data_array, 1, 1);
	  MDELAY(120);
  
	  data_array[0] = 0x00100500;	 
	  dsi_set_cmdq(data_array, 1, 1);
	  MDELAY(120);

	  mt_set_gpio_mode(GPIO_LCM_RST, GPIO_MODE_00);
	  mt_set_gpio_pull_enable(GPIO_LCM_RST, GPIO_PULL_ENABLE);
	  mt_set_gpio_dir(GPIO_LCM_RST, GPIO_DIR_OUT);			
	  mt_set_gpio_out(GPIO_LCM_RST, GPIO_OUT_ZERO);
	  MDELAY(150);


          sub_power(0);//close etk power,,,,,,et4007.c
	

}

static void lcm_resume(void)
{
	
	mt_set_gpio_mode(GPIO_LCM_RST, GPIO_MODE_00);
	mt_set_gpio_pull_enable(GPIO_LCM_RST, GPIO_PULL_ENABLE);
	mt_set_gpio_dir(GPIO_LCM_RST, GPIO_DIR_OUT);		  
	mt_set_gpio_out(GPIO_LCM_RST, GPIO_OUT_ONE);
	MDELAY(150);
	
	lcm_init();
	MDELAY(15);
	 sub_power(1);//close etk power,,,,,,et4007.c
	
}
/*
#if (!defined(BUILD_UBOOT) && !defined(BUILD_LK))
static void lcm_esd_recover(void)
{
	lcm_suspend();
	lcm_resume();
}
#endif
*/

static unsigned int lcm_compare_id(void)
{
	
	unsigned int id=0;
	unsigned char buffer[3];
	unsigned int array[16];  

	SET_RESET_PIN(1); 
	MDELAY(10); 
	SET_RESET_PIN(0); 
	MDELAY(50); 
	SET_RESET_PIN(1); 
	MDELAY(120); 

	array[0] = 0x00063902;
	array[1] = 0x0698FFFF;
	array[2] = 0x00000104;
	dsi_set_cmdq(array, 3, 1); 

	// Set Maximum return byte = 1 
	array[0] = 0x00033700;// read id return two byte,version and id 
	dsi_set_cmdq(array, 1, 1); 
	//MDELAY(10); 

	read_reg_v2(0x04, buffer, 2);  //buffer[0]=0x91;buffer[1]=0x61; 	
	LCM_PRINT("jd9161--lk----id1=0x%02x-----id=0x%x---\n",buffer[0],buffer[1]);
	id=buffer[0];
	return id;
}





// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER jd9161ba_wvga_dsi_vdo_drv = {
	.name = "jd9161ba_wvga_dsi_vdo_drv",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.compare_id = lcm_compare_id,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
	//.set_backlight = lcm_setbacklight,
	//.set_pwm = lcm_setpwm,
	//.get_pwm = lcm_getpwm,
	//.update = lcm_update,
//#if (!defined(BUILD_UBOOT) && !defined(BUILD_LK))
//	.esd_recover = lcm_esd_recover,
//#endif
};

